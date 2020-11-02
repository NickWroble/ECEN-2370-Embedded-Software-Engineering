/**
 * @file leuart.c
 * @author Nicholas Wroblewski
 * @date November 1st, 2020
 * @brief Contains all the functions of the LEUART peripheral
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************

//** Standard Library includes
#include <string.h>

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

enum leuart_states{
	idle,
	dataSent,
	dataReceived,
};

typedef struct{
	enum leuart_states 	state;
	uint32_t 			counter;
	uint32_t 			length;
	bool 				isBusy;
	LEUART_TypeDef 		*LEUARTn;
	char				out[16];
	uint32_t 			CB_numb;
}LEUART_STATE_MACHINE;

static LEUART_STATE_MACHINE LEUART_state;

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

static void leuart_TXBL(void);
static void leuart_TXC(void);


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief Opens the LEUART peripheral.
 *
 * @details Initializes the LEUART_Init_TypeDef, enables the LEUART clock. Calls
 * LEUART_Init to start bus comms. Routes and enables rx and tx pins. Clears rx and
 * tx interrupts from leuart interrupt flag register. Then it calls LEUART_Enable()
 *
 ******************************************************************************/

void leuart_open(LEUART_TypeDef *leuart, LEUART_OPEN_STRUCT *leuart_settings){
    LEUART_Init_TypeDef LEUART_Init_Vals;

    //enable clock peripheral
    if(leuart==LEUART0)
        CMU_ClockEnable(cmuClock_LEUART0, true);

    //clock test
    if(leuart->STARTFRAME &= LEUART_STARTFRAME_STARTFRAME_DEFAULT){
        while(leuart->SYNCBUSY);
        EFM_ASSERT(leuart->STARTFRAME & LEUART_STARTFRAME_STARTFRAME_DEFAULT);
    }
    else{
        leuart->STARTFRAME &= LEUART_STARTFRAME_STARTFRAME_DEFAULT;
    }

    //clear interrupt flag
    if ((leuart->IF & 0x01) == 0) {
            leuart->IFS = 0x01;
            EFM_ASSERT(leuart->IF & 0x01);
            leuart->IFC = 0x01;
        }
        else {
            leuart->IFC = 0x01;
            EFM_ASSERT(!(leuart->IF & 0x01));
        }
    //Initialize values
    LEUART_Init_Vals.baudrate             = leuart_settings->baudrate;
    LEUART_Init_Vals.databits            = leuart_settings->databits;
    LEUART_Init_Vals.enable             = false;
    LEUART_Init_Vals.parity                 = leuart_settings->parity; //master mode - true, slave mode - false
    LEUART_Init_Vals.refFreq             = leuart_settings->refFreq;
    LEUART_Init_Vals.stopbits              = leuart_settings->stopbits;

    LEUART_Init(leuart, &LEUART_Init_Vals);

    //Routing and Route enable
    leuart->ROUTEPEN             = (leuart_settings->rx_pin_en)| (leuart_settings -> tx_pin_en);
    leuart->ROUTELOC0            = (leuart_settings->rx_loc) | (leuart_settings->tx_loc);

    //clear proper interrupts
    leuart->CMD |= LEUART_CMD_CLEARRX | LEUART_CMD_CLEARTX;

    //enables TX and Rx
    LEUART_Enable(leuart, leuartEnable);

    while(leuart->SYNCBUSY);
    while((leuart->STATUS & LEUART_STATUS_RXENS) & (leuart->STATUS & LEUART_STATUS_TXENS));


    NVIC_EnableIRQ(LEUART0_IRQn);

}

/***************************************************************************//**
 * @brief LEUART interrupt flag handler.
 *
 * @details Checks for TXBL and TXC interrupts and handles them as needed
 *
 ******************************************************************************/

void LEUART0_IRQHandler(void){
    int int_flag = LEUART0->IF & LEUART0->IEN;
    LEUART0->IFC = int_flag;
	if (int_flag & LEUART_IF_TXBL)
		leuart_TXBL();
	if (int_flag & LEUART_IF_TXC)
		leuart_TXC();
}

/***************************************************************************//**
 * @brief Starts LEUART communications
 *
 * @details Blocks EM3 sleep mode and initializes LEUART_TypeDef variables.
 * Enables TXBL interrupts
 *
 ******************************************************************************/

void leuart_start(LEUART_TypeDef *leuart, char *string, uint32_t callback){
    while(LEUART_state.isBusy);
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_CRITICAL();

    sleep_block_mode(LEUART_RX_EM);

    LEUART_state.length             = strlen(string);
    LEUART_state.isBusy         	= true;
    LEUART_state.state              = dataSent;
    LEUART_state.LEUARTn            = leuart;
    LEUART_state.counter            = 0;
    strcpy(LEUART_state.out, string);

    leuart->IEN |= LEUART_IEN_TXBL;
    CORE_EXIT_CRITICAL();

}

//**//
// * @brief Checks if the state machine is busy
// *
// * @details: If !isBusy then the state machine is not busy
// *              If isBusy then the state machine is busy
// *
// **

bool leuart_tx_busy(LEUART_TypeDef *leuart){
    return LEUART_state.isBusy;
}

/***************************************************************************//**
 * @brief LEUART STATUS function returns the STATUS of the peripheral for the
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
 * @brief Interrupt handler for TXBL
 *
 * @details Uses case statements based on the leuart state machine to perform
 * various functions. If idle, the state is set to dataSent. If dataSent then
 * it transmits the data in LEUARTn and increments the counter. If the counter
 * equals the data length, TXBL is disabled, and TX is enabled. State is set to
 * dataReceived. dataReceived and default at both false EFM asserts.
 *
 ******************************************************************************/
static void leuart_TXBL(void){
    switch(LEUART_state.state){
        case idle:
            LEUART_state.state = dataSent;
            break;
        case dataSent:
            leuart_app_transmit_byte(LEUART_state.LEUARTn, LEUART_state.out[LEUART_state.counter++]);
            if (LEUART_state.counter == LEUART_state.length){
                LEUART_state.LEUARTn->IEN &=~ LEUART_IEN_TXBL;
                LEUART_state.LEUARTn->IEN |= LEUART_IEN_TXC;
                LEUART_state.state= dataReceived;
            }
            break;
        case dataReceived:
            EFM_ASSERT(false);
            break;
        default:
            EFM_ASSERT(false);
    }
}

/*************************************************************************************************************//**
 * @brief Interrupt handler for TXC
 *
 *
 * @details Using case statements this function switches between states dependent on if the TXC interrupt is fired.
 * In the Datarecieved state the TXC interrupt is disabled, a callback event is scheduled, and the energy mode EM2
 * is unblocked. Next the state machine is reset to the idle state and is no longer busy.
 *
 * @details Uses case statements based on the leuart state machine to perform
 * various functions. If idle, then a false EFM assert. If dataSent then it returns.
 * it transmits the data in LEUARTn and increments the counter. If dataReceived, then TX is disabled,
 * the leuart callback is scheduled, EM3 is unblocked, the state is set to idle, and isBusy is set to false
 *
 *****************************************************************************************************************/

static void leuart_TXC(void){
    switch(LEUART_state.state){
        case idle:
            EFM_ASSERT(false);
            break;
        case dataSent:
            break;
        case dataReceived:
            LEUART_state.LEUARTn->IEN &=~ LEUART_IEN_TXC;//Disable TXC
            add_scheduled_event(LEUART_state.CB_numb); //schedule callback event
            sleep_unblock_mode(LEUART_TX_EM);//unblock sleep mode
            LEUART_state.state=idle;
            LEUART_state.isBusy = false;
            break;
        default:
            EFM_ASSERT(false);
    }
}
