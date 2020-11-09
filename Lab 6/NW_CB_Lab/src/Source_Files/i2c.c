/**
 * @file i2c.c
 * @author Nicholas Wroblewski
 * @date 9/12/2020
 * @brief Contains I2C driver functions
 *
 */
//***********************************************************************************
// Include files
//***********************************************************************************
#include "gpio.h"
#include "i2c.h"

//***********************************************************************************
// defined files
//***********************************************************************************
#define TXB_STATUS_MASK 0b1000000
#define I2C_FLAG_MSTOP 	0b0000001
#define I2C_IEN_CLEAR   0x0
//***********************************************************************************
// Private variables
//***********************************************************************************
static I2C_STATE_MACHINE_STRUCT STATE_MACHINE;
//static uint32_t read_cb;

//***********************************************************************************
// Private functions
//***********************************************************************************
static void i2c_bus_reset(I2C_TypeDef *i2c);
//static void i2c_Interrupt_State();
//static void si7021_Read();
static void i2c_Ack(void);
static void i2c_Nack(void);
static void i2c_Rxdatav(void);
static void i2c_Mstop(void);
//***********************************************************************************
// Global functions
//***********************************************************************************
/***************************************************************************//**
 * @brief
 *  Low level driver to setup I2C peripheral
 *
 * @details
 *  This routine is a low level driver.  The application code calls this function
 * 	 to open one of the I2C peripherals for operation to directly drive
 * 	 GPIO output pins of the device and/or create interrupts that can be used as
 * 	 a system "heart beat" or by a scheduler to determine whether any system
 * 	 functions need to be serviced.
 *
 * @note
 *Two Inputs
 *@param[in] I2C_TypeDef *i2c
 * An i2c clock
 *@param[in] I2C_OPEN_STRUCT *i2cOpenStruct
 * an open struct
 ******************************************************************************/

void i2c_open(I2C_TypeDef *i2c,I2C_OPEN_STRUCT *i2cOpenStruct){
	if(i2c == I2C0){
		CMU_ClockEnable(cmuClock_I2C0, true);
	}
	if(i2c == I2C1){
		CMU_ClockEnable(cmuClock_I2C1, true);
	}
	if((i2c->IF& 0x01) == 0) {
		i2c->IFS= 0x01;
		EFM_ASSERT(i2c->IF& 0x01);
		i2c->IFC= 0x01;
	}
	else{
		i2c->IFC= 0x01;
		EFM_ASSERT(!(i2c->IF& 0x01));
	}
	I2C_Init_TypeDef i2c_values;
	//Initializes Init Values
	i2c_values.clhr = i2cOpenStruct->clhr;
	i2c_values.enable = i2cOpenStruct->enable;
	i2c_values.freq = i2cOpenStruct->freq;
	i2c_values.master = i2cOpenStruct->master;
	i2c_values.refFreq = i2cOpenStruct->refFreq;

	I2C_Init(i2c,&i2c_values);


	i2c->ROUTELOC0 = i2cOpenStruct->out_SCL_route|i2cOpenStruct->out_SDA_route;
	i2c->ROUTEPEN = (i2cOpenStruct->out_SCL_en *_I2C_ROUTEPEN_SCLPEN_MASK ) | (i2cOpenStruct->out_SDA_en * _I2C_ROUTEPEN_SDAPEN_MASK);

	i2c_bus_reset(i2c);

	I2C_IntClear(i2c,i2c->IF);
	I2C_IntClear(i2c,i2c->IEN);
	I2C_IntEnable(i2c,(I2C_IEN_ACK | I2C_IEN_NACK | I2C_IEN_RXDATAV | I2C_IEN_MSTOP));

	NVIC_EnableIRQ(I2C0_IRQn);
//	NVIC_EnableIRQ(I2C1_IRQn);

}
/***************************************************************************//**
 * @brief
 *  Function to reset I2C
 *
 * @details
 * Clears any interrupts, resets the data buffer. Starts and stops the clock
 *
 * @note
 * If not used, could have errors in code
 *
 * @param[in] I2C_TypeDef *i2c \
 * i2c you want reset before use
 *
 *
 ******************************************************************************/
void i2c_bus_reset(I2C_TypeDef *i2c){
	uint32_t int_flag;
	if (i2c->STATE & I2C_STATE_BUSY){
	      i2c->CMD = I2C_CMD_ABORT;
	      while (i2c->STATE & I2C_STATE_BUSY);    // wait for the Abort function to clear the busy bit
	    }
	int_flag = i2c->IEN;
	i2c->IEN &= ~int_flag;
	i2c->IFC = int_flag;
	i2c->CMD = I2C_CMD_CLEARTX;
	i2c->CMD = I2C_CMD_START | I2C_CMD_STOP;
	while(!(i2c->IF & I2C_FLAG_MSTOP));
	i2c->IFC |= I2C_IFC_START | I2C_IFC_SSTOP;
	i2c->IEN = int_flag;
	i2c->CMD = I2C_CMD_ABORT;

}
/***************************************************************************//**
 * @brief
 * Starts the State Machine
 *
 * @details
 * Initializes all of the values in the State Machine previously declared in the Private variables.
 * Once it initializes it, it sends a command to the I2c to start the
 * state machine.
 *
 * @note
 * A key function in setting up the peripheral.
 *@param[in] enStates states
 *Current state should be zero
 *@param[in] I2C_TypeDef *i2c
 *I2c currently using
 *@param[in] uint32_t I2C_address
 *ASk sam or Augy tomorrow
 *@param[in] uint32_t I2C_reg_address

 *@param[in] uint32_t *data
 *@param[in] uint32_t size
 *@param[in] uint32_t read_cb
 *
 ******************************************************************************/
void i2c_Start( I2C_TypeDef *i2c, uint32_t I2C_address, uint32_t I2C_reg_address, uint8_t cmd, uint32_t *data,uint32_t read_cb){
	EFM_ASSERT((i2c->STATE & _I2C_STATE_STATE_MASK) ==I2C_STATE_STATE_IDLE);
	sleep_block_mode(I2C_EM_BLOCK);

	STATE_MACHINE.states = Initialize;
	STATE_MACHINE.i2c = i2c;
	STATE_MACHINE.I2c_address=I2C_address;
	STATE_MACHINE.I2C_reg_address = I2C_reg_address;
	STATE_MACHINE.cmd = cmd;
	STATE_MACHINE.data = data;
	STATE_MACHINE.read_cb =read_cb;

	STATE_MACHINE.i2c->CMD = I2C_CMD_START;
	STATE_MACHINE.i2c->TXDATA = (STATE_MACHINE.I2c_address << 1) | 0b0;


}
/***************************************************************************//**
 * @brief
 *  The Ack interrupt service function
 * @details
 * When an Ack interrupt is called, depedning on the state will perform an
 * action. If in beginning state, will read slave address and then change state to Send command.
 * If in Send state, will send a repeated start and read the data shifted with a write bit.
 * If in Slave state will change it to Get_Temp_MSB, where it will chnage the state to Get_temp_LSB and send a nack.
 * Other states it should send a EFM_ASSERT and false.
 *
 * @note
 * Only one input
 * @param[in] i2c
 *
 *
 ******************************************************************************/
static void i2c_Ack(void){
	switch(STATE_MACHINE.states){
	case Initialize:
		STATE_MACHINE.i2c->TXDATA = STATE_MACHINE.I2C_reg_address;
		STATE_MACHINE.states = Send;
		 break;
	case Send:
		STATE_MACHINE.i2c->CMD = I2C_CMD_START;
		STATE_MACHINE.i2c->TXDATA = (STATE_MACHINE.I2c_address<<1) | 0b1;
		STATE_MACHINE.states = Slave;
		 break;
	case Slave:
		STATE_MACHINE.states = Get_Temp_MSB;
		 break;
	case Get_Temp_MSB:
		 EFM_ASSERT(false);
		 break;
	case Get_Temp_LSB:
		EFM_ASSERT(false);
		 break;
	case Stop:
		 EFM_ASSERT(false);
		 break;
	default:
		EFM_ASSERT(false);
		break;

	}
}
/***************************************************************************//**
 * @brief
 *  The Nack Service Function
 *
 * @details
 * Shouldn't do much unless in Slave state where it will keep
 * ending a repeated start command until an address is read.
 *
 * @note
 * One input
 * @param[in] I2C
 *
 *
 ******************************************************************************/
static void i2c_Nack(void){
	switch(STATE_MACHINE.states){
	case Initialize:
		 EFM_ASSERT(false);
		 break;
	case Send:
		 EFM_ASSERT(false);
		 break;
	case Slave:
		STATE_MACHINE.i2c->CMD = I2C_CMD_START;
		STATE_MACHINE.i2c->TXDATA = (STATE_MACHINE.I2c_address<<1) | 0b1;
		break;
	case Get_Temp_MSB:
		EFM_ASSERT(false);
		 break;
	case Get_Temp_LSB:
		EFM_ASSERT(false);
		break;
	case Stop:
		 EFM_ASSERT(false);
		 break;
	default:
		EFM_ASSERT(false);
		break;
	}
}
/***************************************************************************//**
 * @brief
 *  RXDATAV service function
 *
 * @details
 * Shouldn't acitavted until Get_Temp_LSB state where it
 *  reads the data and sends a nack and a Stop command
 *
 * @note
 * Reads the data from SI7021
 *
 *@param[in] I2C
 *
 ******************************************************************************/
static void i2c_Rxdatav(void){
	switch(STATE_MACHINE.states){
	case Initialize:
		 EFM_ASSERT(false);
	     break;
	case Send:
		 EFM_ASSERT(false);
		 break;
	case Slave:
		 EFM_ASSERT(false);
		 break;
	case Get_Temp_MSB:
		*STATE_MACHINE.data= (STATE_MACHINE.i2c->RXDATA)<<8;

		STATE_MACHINE.i2c->CMD = I2C_CMD_ACK; //sends NACK interrupt
		STATE_MACHINE.states = Get_Temp_LSB;
		 break;
	case Get_Temp_LSB:
		*STATE_MACHINE.data|= STATE_MACHINE.i2c->RXDATA;
		STATE_MACHINE.i2c->CMD = I2C_CMD_NACK; //sends NACK interrupt
		STATE_MACHINE.i2c->CMD = I2C_CMD_STOP;
		STATE_MACHINE.states = Stop;
		 break;
	case Stop:
		 EFM_ASSERT(false);
		 break;
	default:
		EFM_ASSERT(false);
		break;

	}
}
/***************************************************************************//**
 * @brief
 *  Mstop Service Routine
 *
 * @details
 * Should make function fail unless in stop state, where it will add this to the scheduler
 * unblock the sleep mode and stop the interrupts.
 *
 * @note
 * One Input
 *
 * @param[in]I2c
 * Clock
 *
 ******************************************************************************/
static void i2c_Mstop(void){
	switch(STATE_MACHINE.states){
	case Initialize:
		 EFM_ASSERT(false);
		 break;
	case Send:
		 EFM_ASSERT(false);
		 break;
	case Slave:
		 EFM_ASSERT(false);
		 break;
	case Get_Temp_MSB:
		 EFM_ASSERT(false);
		 break;
	case Get_Temp_LSB:
		EFM_ASSERT(false);
		break;
	case Stop:
//		 STATE_MACHINE.i2c->CMD |= I2C_CMD_STOP;
		 sleep_unblock_mode(I2C_EM_BLOCK);
		 add_scheduled_event(STATE_MACHINE.read_cb);
		 STATE_MACHINE.states = Initialize;
		 break;
	default:
		EFM_ASSERT(false);
		break;

	}
}
/***************************************************************************//**
 * @brief
 *  Interrupt Service Routine for I2C0.
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
void I2C0_IRQHandler(void){
	uint32_t int_flag; //store the source interrupts
	int_flag = I2C0->IF & I2C0->IEN;   //source variable will only contain interrupts of interest
	I2C0->IFC = int_flag;
	if(int_flag & I2C_IF_ACK){
		i2c_Ack();
	}
	if(int_flag & I2C_IF_NACK){
		i2c_Nack();
	}
	if(int_flag & I2C_IF_RXDATAV){
			i2c_Rxdatav();
		}
	if(int_flag & I2C_IF_MSTOP){
			i2c_Mstop();
		}

}
/***************************************************************************//**
 * @brief
 *  Interrupt Service Routine for I2C1.
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
void I2C1_IRQHandler(void){
	uint32_t int_flag; //store the source interrupts
	int_flag = I2C1->IF & I2C1->IEN;   //source variable will only contain interrupts of interest
	I2C1->IFC = int_flag;
	if(int_flag & I2C_IF_ACK){
		i2c_Ack();
	}
	if(int_flag & I2C_IF_NACK){
		i2c_Nack();
	}
	if(int_flag & I2C_IF_RXDATAV){
			i2c_Rxdatav();
		}
	if(int_flag & I2C_IF_MSTOP){
			i2c_Mstop();
		}

}
