/**
 * @file leuart.c
 * @author Nicholas Wroblewski
 * @date November 1st 2020
 * @brief Contains all the functions of the LEUART peripheral
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************

//** Standard Library includes
#include <string.h>
#include <stdio.h>

//** Silicon Labs include files
#include "em_gpio.h"
#include "em_cmu.h"

//** Developer/user include files
#include "leuart.h"
#include "scheduler.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// private variables
//***********************************************************************************
uint32_t	rx_done_evt;
uint32_t	tx_done_evt;
bool		leuart0_tx_busy;
LEUART_STATE_MACHINE SM;

/***************************************************************************//**
 * @brief LEUART driver
 * @details
 *  This module contains all the functions to support the driver's state
 *  machine to transmit a string of data across the LEUART bus.  There are
 *  additional functions to support the Test Driven Development test that
 *  is used to validate the basic set up of the LEUART peripheral.  The
 *  TDD test for this class assumes that the LEUART is connected to the HM-18
 *  BLE module.  These TDD support functions could be used for any TDD test
 *  to validate the correct setup of the LEUART.
 *
 ******************************************************************************/

//***********************************************************************************
// Private functions
//***********************************************************************************
void leuart_Txbl(void);
void leuart_Txc(void);


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 * LEUART OPEN sets up the clock to the LEUART peripheral and enables interrupts
 * @details
 *  * @details
 *  This routine is a low level driver.  The application code calls this function
 * 	 to open one of the LEUART peripherals for operation to directly drive
 * 	 GPIO output pins of the device and/or create interrupts that can be used as
 * 	 a system "heart beat" or by a scheduler to determine whether any system
 * 	 functions need to be serviced.
 *
 * @note
 *Two Inputs
 *@param[in] LEUART_TypeDef *leuart
 * An leuart clock
 *@param[in] I2C_OPEN_STRUCT *leuart_settings
 * an open struct
 *
 ******************************************************************************/

void leuart_open(LEUART_TypeDef *leuart, LEUART_OPEN_STRUCT *leuart_settings){
	if(leuart == LEUART0){
		CMU_ClockEnable(cmuClock_LEUART0, true);
	}
	if((leuart->STARTFRAME & 0xFF) == 0) {
			leuart->STARTFRAME |= 0x01;
			EFM_ASSERT(leuart->STARTFRAME& 0x01);
			leuart->STARTFRAME &=~ leuart->STARTFRAME;
		}
		else{
			leuart->STARTFRAME &=~ leuart->STARTFRAME;
			EFM_ASSERT(!(leuart->IFC & 0xFF));
		}

	LEUART_Init_TypeDef LE_init;
	LE_init.baudrate = leuart_settings->baudrate;
	LE_init.databits = leuart_settings->databits;
	LE_init.enable   = leuart_settings->enable;
	LE_init.parity   = leuart_settings->parity;
	LE_init.refFreq  = HM10_REFFREQ;
	SM.rx_done_evt = leuart_settings->rx_done_evt;
	SM.tx_done_evt = leuart_settings->tx_done_evt;
	LEUART_Init(leuart,&LE_init);

	leuart->ROUTELOC0 = leuart_settings->rx_loc|leuart_settings->tx_loc;
	leuart->ROUTEPEN = (leuart_settings->rx_pin_en * _LEUART_ROUTEPEN_RXPEN_MASK)|(leuart_settings->tx_pin_en * _LEUART_ROUTEPEN_TXPEN_MASK);

	while(leuart->SYNCBUSY);
	leuart->CMD = LEUART_CMD_CLEARTX|LEUART_CMD_CLEARRX;


	LEUART_Enable(leuart,leuartEnable);
	EFM_ASSERT(leuart->STATUS & (LEUART_STATUS_TXENS|LEUART_STATUS_RXENS));

	LEUART_IntClear(leuart,leuart->IF);
	LEUART_IntClear(leuart,leuart->IEN);
	NVIC_EnableIRQ(LEUART0_IRQn);



}




/***************************************************************************//**
 * @brief  * @brief
 * Starts the State Machine
 *
 * @details
 * Initializes all of the values in the State Machine previously declared in the Private variables.
 * Once it initializes it, it sends a command to the I2c to start the
 * state machine.
 *
 * @param[in] *leuart
 * the leuart
 * @param[in] *string
 * string you want to transmit
 * @param[in] string_len
 * length of string
 *
 ******************************************************************************/

void leuart_start(LEUART_TypeDef *leuart, char *string, uint32_t string_len){
	while(leuart_tx_busy(leuart));
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	SM.leuart = leuart;
	SM.length = string_len;
	SM.STM = Start;
	SM.busy = true;
	SM.n = 0;
	strcpy(SM.strg ,string);
	sleep_block_mode(LEUART_TX_EM);
	leuart->IEN |= LEUART_IEN_TXBL;
	CORE_EXIT_CRITICAL();

}

/***************************************************************************//**
 * @brief
 * LEUART BUSY returns whether or not the TX buffer is busy
 *
 * @details
 * This function reads the status register provided outside of this function to read the TXBL buffer
 *
 * @parmam[in] *leuart
 * Defines the LEUART peripheral to access.
 *
 * @return
 * 	returns a boolean on whether its false
 ******************************************************************************/

bool leuart_tx_busy(LEUART_TypeDef *leuart){
	if(leuart->STATUS & LEUART_STATUS_TXBL) return false;
	else return true;
}

/***************************************************************************//**
 * @brief
 *   LEUART STATUS function returns the STATUS of the peripheral for the
 *   TDD test
 *
 * @details
 * 	 This function enables the LEUART STATUS register to be provided to
 * 	 a function outside this .c module.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @return
 * 	 Returns the STATUS register value as an uint32_t value
 *
 ******************************************************************************/

uint32_t leuart_status(LEUART_TypeDef *leuart){
	uint32_t	status_reg;
	status_reg = leuart->STATUS;
	return status_reg;
}

/***************************************************************************//**
 * @brief
 *   LEUART CMD Write sends a command to the CMD register
 *
 * @details
 * 	 This function is used by the TDD test function to program the LEUART
 * 	 for the TDD tests.
 *
 * @note
 *   Before exiting this function to update  the CMD register, it must
 *   perform a SYNCBUSY while loop to ensure that the CMD has by synchronized
 *   to the lower frequency LEUART domain.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @param[in] cmd_update
 * 	 The value to write into the CMD register
 *
 ******************************************************************************/

void leuart_cmd_write(LEUART_TypeDef *leuart, uint32_t cmd_update){

	leuart->CMD = cmd_update;
	while(leuart->SYNCBUSY);
}

/***************************************************************************//**
 * @brief
 *   LEUART IF Reset resets all interrupt flag bits that can be cleared
 *   through the Interrupt Flag Clear register
 *
 * @details
 * 	 This function is used by the TDD test program to clear interrupts before
 * 	 the TDD tests and to reset the LEUART interrupts before the TDD
 * 	 exits
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 ******************************************************************************/

void leuart_if_reset(LEUART_TypeDef *leuart){
	leuart->IFC = 0xffffffff;
}

/***************************************************************************//**
 * @brief
 *   LEUART App Transmit Byte transmits a byte for the LEUART TDD test
 *
 * @details
 * 	 The BLE module will respond to AT commands if the BLE module is not
 * 	 connected to the phone app.  To validate the minimal functionality
 * 	 of the LEUART peripheral, write and reads to the LEUART will be
 * 	 performed by polling and not interrupts.
 *
 * @note
 *   In polling a transmit byte, a while statement checking for the TXBL
 *   bit in the Interrupt Flag register is required before writing the
 *   TXDATA register.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @param[in] data_out
 *   Byte to be transmitted by the LEUART peripheral
 *
 ******************************************************************************/

void leuart_app_transmit_byte(LEUART_TypeDef *leuart, uint8_t data_out){
	while (!(leuart->IF & LEUART_IF_TXBL));
	leuart->TXDATA = data_out;
}


/***************************************************************************//**
 * @brief
 *   LEUART App Receive Byte polls a receive byte for the LEUART TDD test
 *
 * @details
 * 	 The BLE module will respond to AT commands if the BLE module is not
 * 	 connected to the phone app.  To validate the minimal functionality
 * 	 of the LEUART peripheral, write and reads to the LEUART will be
 * 	 performed by polling and not interrupts.
 *
 * @note
 *   In polling a receive byte, a while statement checking for the RXDATAV
 *   bit in the Interrupt Flag register is required before reading the
 *   RXDATA register.
 *
 * @param[in] leuart
 *   Defines the LEUART peripheral to access.
 *
 * @return
 * 	 Returns the byte read from the LEUART peripheral
 *
 ******************************************************************************/

uint8_t leuart_app_receive_byte(LEUART_TypeDef *leuart){
	uint8_t leuart_data;
	while (!(leuart->IF & LEUART_IF_RXDATAV));
	leuart_data = leuart->RXDATA;
	return leuart_data;
}
/***************************************************************************//**
* @brief
 *  Txbl service routine
*
 *
 * @details
* Works in all states besides complete
*
 * @note
 * No input or Outputs
 *
 *
 ******************************************************************************/
void leuart_Txbl(void){
	switch(SM.STM){
		case Start:
		SM.STM=Sending;
		 break;
	case Sending:
		 if(!(SM.n == (SM.length -2) )){ //not sure why necessary to send space between last interrupt but thats what this does
					 SM.leuart->TXDATA = SM.strg[SM.n];
					 SM.n++;
					 SM.STM = Wait;
				 }
		 else{
			SM.STM=Wait;
		 }
		 break;
	case Wait:
		if(!(SM.n == (SM.length - 2))){
			SM.STM = Start;
		}
		else{
			SM.leuart->TXDATA = SM.strg[SM.n];
			SM.n++;
			SM.leuart->IEN &= ~LEUART_IEN_TXBL;
			SM.leuart->IEN |= LEUART_IEN_TXC;
			SM.STM = Complete;
		}
		 break;
	case Complete:
		 EFM_ASSERT(false);
		 break;
	default:
		EFM_ASSERT(false);
		break;
	}
}
/***************************************************************************//**
* @brief
 *  TXC service routine
*
 *
 * @details
* should only work in Complete state
*
 * @note
 * No input or Outputs
 *
 *
 ******************************************************************************/
	void leuart_Txc(void){
	switch(SM.STM){
	case Start:
		 EFM_ASSERT(false);
		 break;
	case Sending:
		 EFM_ASSERT(false);
		 break;
	case Wait:
		 EFM_ASSERT(false);
		 break;
	case Complete:
		sleep_unblock_mode(LEUART_TX_EM);
		SM.busy = false;
		SM.leuart->IEN &=~ LEUART_IEN_TXC;
		add_scheduled_event(SM.tx_done_evt);
		break;
	default:
		EFM_ASSERT(false);
		break;
	}
}
/***************************************************************************//**
* @brief
 *  Interrupt Service Routine for LEAURT0.
*
 *
 * @details
* Always running in background,waits to add.
* Checks Interrupts of interest and will add them
*
 * @note
 * No input or Outputs
 *
 *
 ******************************************************************************/
void LEUART0_IRQHandler(void){
	uint32_t int_flag; //store the source interrupts
	int_flag = LEUART0->IF & LEUART0->IEN;   //source variable will only contain interrupts of interest
	LEUART0->IFC = int_flag;
	if(int_flag & LEUART_IF_TXBL){
		leuart_Txbl();
	}
	if(int_flag & LEUART_IF_TXC){
		leuart_Txc();
	}

}
