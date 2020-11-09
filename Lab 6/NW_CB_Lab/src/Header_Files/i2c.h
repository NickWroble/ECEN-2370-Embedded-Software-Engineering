
//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef SRC_HEADER_FILES_I2C_H_
#define SRC_HEADER_FILES_I2C_H_

/* System include statements */


/* Silicon Labs include statements */
#include "em_i2c.h"
#include "em_assert.h"
/* The developer's include statements */
#include "scheduler.h"
#include "sleep_routines.h"
#include "SI7021.h"

//***********************************************************************************
// defined files
//***********************************************************************************
#define I2C_EM_BLOCK 2

//#define Initialize  0
//#define Send 1
//#define Slave 2
//#define Get_Temp 3
//#define	Stop 4
//#define Close 5
//***********************************************************************************
// global variables
//***********************************************************************************
typedef struct{
	bool 	enable; //Enable I2C peripheral when initialization completed.
	bool 	master; //Set to master (true) or slave (false) mode.
	uint32_t 	refFreq; //I2C reference clock assumed when configuring bus frequency setup.
	uint32_t 	freq; //(Max) I2C bus frequency to use.
	I2C_ClockHLR_TypeDef 	clhr; //Clock low/high ratio control.
	uint32_t		out_SCL_route ;		// out 0 route to gpio port/pin
	uint32_t		out_SDA_route;		// out 1 route to gpio port/pin
	bool			out_SCL_en;		// enable out 0 route
	bool			out_SDA_en;

}I2C_OPEN_STRUCT;
typedef enum{Initialize,
	Send,
	Slave,
	Get_Temp_MSB,
	Get_Temp_LSB,
	Stop,

}enStates;
typedef struct{
	int states;
	I2C_TypeDef *i2c;
	uint32_t I2c_address;
	uint32_t I2C_reg_address;
	uint8_t cmd;
	uint32_t *data;
	uint32_t size;
	uint32_t		read_cb;
} I2C_STATE_MACHINE_STRUCT;


//***********************************************************************************
// function prototypes
//***********************************************************************************
void i2c_open(I2C_TypeDef *i2c,I2C_OPEN_STRUCT *i2cOpenStruct);

void I2C0_IRQHandler(void);
void I2C1_IRQHandler(void);

void i2c_Start(I2C_TypeDef *i2c, uint32_t I2c_address,
		uint32_t I2C_reg_address, uint8_t cmd, uint32_t *data,uint32_t read_cb);
//void i2c_Start(I2C_STATE_MACHINE_STRUCT STATE_MACHINE);
#endif /* SRC_HEADER_FILES_I2C_H_ */
