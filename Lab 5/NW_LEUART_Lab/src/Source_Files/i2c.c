/**
 * @file i2c.c
 *
 * @author Nicholas Wroblewski
 *
 * @date October 11th, 2020
 *
 * @brief Setting up I2C bus comms
 *
 */
//***********************************************************************************
// Include files
//***********************************************************************************

#include "i2c.h"
#include "cmu.h"
#include "brd_config.h"


//***********************************************************************************
// defined files
//***********************************************************************************

#define READ 1
#define WRITE 0

//***********************************************************************************
// function prototypes (private)
//***********************************************************************************

static void i2c_ACK(void);
static void i2c_NACK(void);
static void i2c_RXDATAV(void);
static void i2c_bus_reset(I2C_TypeDef *i2c);
static void send_data(void);
static void end_i2c(void);

//***********************************************************************************
// Static / Private Variables
//***********************************************************************************

static I2C_STATE_MACHINE i2c_state;

//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief Initializes elements for I2C
 *
 *
 * @details Begins by enabling clocks for I2C operation. Then it checks that the
 * interrupt flag is cleared. Then it initializes necessary values of I2C_Init_TypeDef. Then it routes
 * the SCL and SDA lines and enables them. Lastly, I2C interrupts are enabled

 *
 * @param[in] I2C_TypeDef *i2c
 * Contains the registers of I2C
 *
 *
 * @param[in] I2C_OPEN_STRUCT *i2c_setup
 * This input parameter allows for the setup of SDA and SCL lines
 *
 ******************************************************************************/
void i2c_open(I2C_TypeDef *i2c, I2C_OPEN_STRUCT *i2c_setup){
	I2C_Init_TypeDef I2C_Init_Vals;
	if(i2c==I2C0){
		CMU_ClockEnable(cmuClock_I2C0, true);
	}
	if(i2c==I2C1){
		CMU_ClockEnable(cmuClock_I2C1, true);
		}

	if ((i2c->IF & 0x01) == 0) {
		i2c->IFS = 0x01;
		EFM_ASSERT(i2c->IF & 0x01);
		i2c->IFC = 0x01;
	}
	else {
		i2c->IFC = 0x01;
		EFM_ASSERT(!(i2c->IF & 0x01));
	}

	I2C_Init_Vals.clhr	 		= i2c_setup->clhr;
	I2C_Init_Vals.enable		= true;
	I2C_Init_Vals.freq 			= i2c_setup->freq;
	I2C_Init_Vals.master	 	= true; //master mode - true, slave mode - false
	I2C_Init_Vals.refFreq 		= i2c_setup->refFreq;

	I2C_Init(i2c, &I2C_Init_Vals);

	i2c->ROUTEPEN 			= (I2C_ROUTEPEN_SCLPEN* i2c_setup->out_route_SCLEN )| (I2C_ROUTEPEN_SDAPEN*i2c_setup -> out_route_SDAEN);
	i2c->ROUTELOC0 			= SCLLOC;
	i2c->ROUTELOC0			|= SDALOC;

	i2c->IEN = I2C_IEN_RXDATAV | I2C_IEN_ACK  | I2C_IEN_NACK | I2C_IEN_MSTOP;

	if(i2c==I2C0){
		NVIC_EnableIRQ(I2C0_IRQn);
	}
	if(i2c==I2C1){
		NVIC_EnableIRQ(I2C1_IRQn);;
			}

}
/***************************************************************************//**
 * @brief Starts the I2C communication
 *
 *
 * @details Initializes all the values in the I2C_STATE_MACHINE which is signified by i2c_state. Next the bus is reset
 * and then the send_data is called
 *
 *
 * @note The pearl gecko must be blocked from sleep mode using the sleep_block_mode function while running the state machine
 *
 *
 * @param[in]I2C_TypeDef *i2c
 * Allows control of the I2C registers
 *
 * @param[in]uint32_t SA
 * Slave address
 *
 * @param[in]uint32_t SR
 * This represents the measurement command
 *
 * @param[in]uint32_t callback
 * stores the value of CB of I2C_STATE_MACHINE in callback
 *
 *
 *
 ******************************************************************************/
void i2c_start(I2C_TypeDef *i2c, uint32_t SA, uint32_t SR, uint32_t callback){
	EFM_ASSERT((I2C0->STATE & _I2C_STATE_STATE_MASK) == I2C_STATE_STATE_IDLE);
	sleep_block_mode(I2C_EM_BLOCK);

	i2c_state.I2Cn 				= i2c;
	i2c_state.SM_busy 			= true;
	i2c_state.current_state 	= start;
	i2c_state.slave_addr 		= SA;
	i2c_state.slave_reg			= SR;
	i2c_state.CB				= callback;
	i2c_bus_reset(i2c);
	send_data();
}
/***************************************************************************//**
 * @brief Handles used interrupts for I2C0
 *
 *
 * @details Begins by clearing the interrupt flag. If a specific interrupt
 * is triggered then function that handles said interrupt is called.
 *
 *
 ******************************************************************************/
void I2C0_IRQHandler(void){
	int int_flag = I2C0->IF;
	I2C0->IFC=int_flag;
	if (int_flag & I2C_IF_ACK)
		i2c_ACK();
	if (int_flag & I2C_IF_NACK)
		i2c_NACK();
	if (int_flag & I2C_IF_RXDATAV)
		i2c_RXDATAV();
}
/***************************************************************************//**
 * @brief Handles the used interrupts for I2C1
 *
 *
 * @details This first begins by clearing the interrupt flag. Then using if statements, if a specific interrupt
 * is triggered, then the function that handles said interrupt is called.
 *
 ******************************************************************************/
void I2C1_IRQHandler(void){
	int int_flag = I2C1->IF;
	I2C1->IFC=int_flag;
		if (int_flag & I2C_IF_ACK)
			i2c_ACK();
		if (int_flag & I2C_IF_NACK)
			i2c_NACK();
		if (int_flag & I2C_IF_RXDATAV)
			i2c_RXDATAV();
}

/***************************************************************************//**
 * @brief Reads the data sent back from the slave
 *
 *
 * @details Uses an if statement to check if the state machine is busy. If not busy then the
 * data is pointed to by the data_location
 *
 *
 * @note An assertion is used in order to make sure the correct data is being collected
 *
 *
 * @param[in] uint32_t *data_location
 * This parameter is used in order to place the i2c_state.data in a specific location
 *
 ******************************************************************************/
void read_data(uint32_t *data_location){
	if(!(i2c_state.SM_busy))
		 *data_location=i2c_state.data;
		else
			EFM_ASSERT(false);
		return;
}
//***********************************************************************************
// Private functions
//**********************************************************************************
/***************************************************************************//**
 * @brief Resets the bus
 *
 *
 * @details This function resets the bus to the assigned "default" values in order to keep the state machine
 * running properly
 *
 * @param[in]I2C_TypeDef *i2c
 *  This input argument allows us to control the registers of the I2C
 *
 ******************************************************************************/
static void i2c_bus_reset(I2C_TypeDef *i2c){

	if(i2c->STATE & I2C_STATE_BUSY){
		i2c->CMD = I2C_CMD_ABORT;
		while(i2c->STATE & I2C_STATE_BUSY);
	}

	uint32_t state				= i2c->IEN; 						//save state of the I2C->IEN register
	i2c->IEN 					= 0;								//disable all interrupts using the I2C IEN register
	i2c->IFC 					= i2c->IF;							//clear the interrupt flag via IFC register
	i2c->CMD  					= I2C_CMD_CLEARTX; 					//clear the I2C transmit buffer
	i2c->CMD 					= I2C_CMD_START | I2C_CMD_STOP;		//perform the simultaneous setting of the START and STOP bits in the i2c command register

	while(!(i2c->IF & _I2C_IF_MSTOP_MASK)){}							//stall until STOP has been completed by checking the MSTOP but in the IF register
		i2c->IFC 				= i2c->IF;							//clear all interrupts by writing to the IFC register
		i2c->IEN 				= state;							//put state back of the IEN register by using the saved state at start
		i2c->CMD 				= I2C_CMD_ABORT;					//write ABORT bit to I2C command register (reset I2C)

}
/***************************************************************************//**
 * @brief Handles the ACK interrupt
 *
 *
 * @details Using case statements this function switches between states dependent on if the ACK interrupt is fired
 *
 *
 * @note If the ACK interrupt is not fired an assertion is used
 *
 ******************************************************************************/
static void i2c_ACK(void){
	switch (i2c_state.current_state){
		case start:
			i2c_state.current_state= measure;
			send_data();
			break;
		case measure:
			i2c_state.current_state = process;
			send_data();
			break;
		case process:
			i2c_state.current_state = MSB;
			break;
		case MSB:
			EFM_ASSERT(false);
			break;
		case stop:
			EFM_ASSERT(false);
			break;
		case done:
			EFM_ASSERT(false);
			break;
		default:
			EFM_ASSERT(false);
	}
}
/***************************************************************************//**
 * @brief Handles the NACK interrupt
 *
 *
 * @details Using case statements this function switches between states dependent on if the NACK interrupt is fired
 *
 *
 * @note If the NACK interrupt is not fired an assertion is used
 *
 ******************************************************************************/


static void i2c_NACK(void){
switch (i2c_state.current_state){
	case start:
		EFM_ASSERT(false);
		break;
	case measure:
		EFM_ASSERT(false);
		break;
	case process:
		send_data();
		break;
	case MSB:
		EFM_ASSERT(false);
		break;
	case stop:
		EFM_ASSERT(false);
		break;
	case done:
		EFM_ASSERT(false);
		break;
	default:
		EFM_ASSERT(false);
	}
}
/***************************************************************************//**
 * @brief Handles the RXDATAV interrupt
 *
 *
 * @details Using case statements this function switches between states dependent on if the RXDATAV interrupt is fired.
 * This function also uses the end_i2c function as the function I2C communicaiton is finished
 *
 *
 * @note If the RXDATAV interrupt is not fired an assertion is used
 *
 ******************************************************************************/
static void i2c_RXDATAV(void){
	switch (i2c_state.current_state){
			case start:
				EFM_ASSERT(false);
				break;
			case measure:
				EFM_ASSERT(false);
				break;
			case process:
				EFM_ASSERT(false);
				break;
			case MSB:
				i2c_state.data = i2c_state.I2Cn->RXDATA<<8;
				i2c_state.current_state= stop;
				send_data();
				break;
			case stop:
				i2c_state.data |= i2c_state.I2Cn->RXDATA;
				i2c_state.current_state= done;
				send_data();
				end_i2c();
				break;
			case done:
				EFM_ASSERT(false);
				break;
			default:
				EFM_ASSERT(false);
	}
}


/***************************************************************************//**
 * @brief Handles the jobs of each state
 *
 *
 * @details The commands for each state can be found in the flowchart
 *
 *
 * @note An assertion is used in the readytostart state as this state is simply an "idle" where nothing occurs
 * except waiting for the state machine to start
 *
 *
 ******************************************************************************/
static void send_data(){
	switch (i2c_state.current_state){
			case readytostart :
				EFM_ASSERT(false);
				break;
			case start:
				i2c_state.I2Cn->CMD 	= I2C_CMD_START;
				i2c_state.I2Cn->TXDATA	= (i2c_state.slave_addr<<1)|WRITE;
				break;
			case measure:
				i2c_state.I2Cn->TXDATA  = i2c_state.slave_reg;
				break;
			case process:
				i2c_state.I2Cn->CMD 	= I2C_CMD_START;
				i2c_state.I2Cn->TXDATA 	= (i2c_state.slave_addr<<1)|READ;
				break;
			case MSB:
				EFM_ASSERT(false);
				break;
			case stop:
				i2c_state.I2Cn->CMD 	= I2C_CMD_ACK;
				break;
			case done:
				i2c_state.I2Cn->CMD 	=I2C_CMD_NACK;
				i2c_state.I2Cn->CMD 	=I2C_CMD_STOP;
				break;
		}

}
/***************************************************************************//**
 * @brief Ends the I2C operation
 *
 *
 * @details This function first sets the state machine to no longer be busy and resets the state machine
 * to the readytostart or "idle" condition
 *
 *
 * @note This function unblocks sleep mode as the state machine is no longer running
 *
 ******************************************************************************/
static void end_i2c(){
	i2c_state.SM_busy = false;
	i2c_state.current_state = readytostart;
	sleep_unblock_mode(I2C_EM_BLOCK);
	add_scheduled_event(i2c_state.CB);
}


