
#ifndef SRC_HEADER_FILES_I2C_H_
#define SRC_HEADER_FILES_I2C_H_


//***********************************************************************************
// Include files
//***********************************************************************************

#include "sleep_routines.h"
#include "scheduler.h"

//***********************************************************************************
// defined files
//***********************************************************************************

#define I2C_EM_BLOCK 					EM2   // 2 = first mode it cannot enter

//***********************************************************************************
// structs
//***********************************************************************************

typedef struct{
	bool 					enable;
	bool 					master;
	uint32_t 				refFreq;
	uint32_t 				freq;
	I2C_ClockHLR_TypeDef 	clhr;
	bool 					out_route_SCLEN;
	bool 					out_route_SDAEN;
	uint32_t 				out_SCLLOC_route;
	uint32_t 				out_SDALOC_route;


} I2C_OPEN_STRUCT;

enum i2c_defined_state {
		readytostart, 	//waiting to start the state machine
		start, 			//initialize Si7021 peripheral
		measure, 		//measures temp
		process, 		//slave address processed
		MSB, 			//gets temperature MSB
		stop, 			//sends stop command
		done 			//finishes i2c sequence
	};

typedef struct{
	enum i2c_defined_state 		current_state;
	uint32_t 					slave_addr;
	uint32_t 					slave_reg;
	bool 						SM_busy;
	I2C_TypeDef 				*I2Cn;
	uint32_t					data;
	uint32_t 					CB;
} I2C_STATE_MACHINE;

//***********************************************************************************
// function prototypes (global)
//***********************************************************************************

void i2c_open(I2C_TypeDef *i2c, I2C_OPEN_STRUCT *i2c_setup);
void i2c_start(I2C_TypeDef *i2c, uint32_t SA, uint32_t SR, uint32_t callback);
void I2C0_IRQHandler(void);
void I2C1_IRQHandler(void);
void read_data(uint32_t *data_location);

#endif /* SRC_HEADER_FILES_I2C_H_ */
