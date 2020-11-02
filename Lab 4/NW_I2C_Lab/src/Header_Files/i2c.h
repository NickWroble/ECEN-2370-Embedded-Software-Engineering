#ifndef SRC_HEADER_FILES_I2CH_
#define SRC_HEADER_FILES_I2CH_


//**
// Include files
#include "sleep_routines.h" //doesn't like bool types for some reason unless you include
#include"brd_config.h"
#include"em_i2c.h"
//**

//**
// global variables
#define I2C_EM_BLOCK 								EM2
//**

//**
// structs
typedef struct{
	bool 					enable;
	bool 					master;
	uint32_t 				freq;
	uint32_t 				refFreq;
	I2C_ClockHLR_TypeDef 	clhr;

	bool 					OUT_ROUTE_SCL_EN;
	bool 					OUT_ROUTE_SDA_EN;
	uint32_t				OUT_ROUTE_SCL;
	uint32_t				OUT_ROUTE_SDA;
} I2C_OPEN_STRUCT;

enum i2c_defined_states{
		idle, 		//state if state is idle
		start,		//initialize sensor
		measure,	//command to measure temp
		process,	//slave address processed
		MSB,		//most significant bit
		stop,		//send stop cmd
		done		//end of i2c sequence
};

typedef struct{
	enum i2c_defined_states 	state;
	uint32_t		slave_addr;
	uint32_t 		slave_reg;
	bool 			isBusy;
	I2C_TypeDef 	*I2Cn;
	uint32_t 		data;
	uint32_t 		callback;

} I2C_STATE_MACHINE ;
//*


//**
// defined files
//**


//**
// private function prototypes
//**



//**
// function prototypes
void i2c_open(I2C_TypeDef *i2c, I2C_OPEN_STRUCT *i2c_setup);
void I2C0_IRQHandler(void);
void I2C1_IRQHandler(void);
void i2c_start(I2C_TypeDef *i2c, uint32_t SA, uint32_t SR, uint32_t callback);
void read_data(uint32_t *data_loc);
//**



#endif /* SRC_HEADER_FILES_I2CH */
