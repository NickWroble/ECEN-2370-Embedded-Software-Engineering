/**
 * @file SI7021.c
 * @author Nicholas Wroblewski
 * @date 9/12/2020
 * @brief Contains Application Functions for running the SI7021
 *
 */
//***********************************************************************************
// Include files
//***********************************************************************************
#include "SI7021.h"


//***********************************************************************************
// defined files
//***********************************************************************************
#define SI7021_READ_CB 0x00000005

//***********************************************************************************
// Private variables
//***********************************************************************************

static uint32_t result;
//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************
/***************************************************************************//**
 * @brief
 *  Initializes the I2C Open Struct and chooses which I2C will be used for the program
 *
 * @details
 * In the header file, if SI7021_I2C_PERIPHERAL_0 is defined then I2C0 will be used and if not then I2C1 will be used.
 *
 * @note
 * A key function in setting up the peripheral. No inputs or outputs
 *
 *
 ******************************************************************************/
void si7021_i2c_open(void){
	I2C_OPEN_STRUCT i2cOpenStruct;

	i2cOpenStruct.clhr = i2cClockHLRAsymetric;		// out 0 route to gpio port/pin
	i2cOpenStruct.enable = true;		// out 1 route to gpio port/pin
	i2cOpenStruct.master = true;
	i2cOpenStruct.freq = I2C_FREQ_FAST_MAX;		// enable out 0 route
	i2cOpenStruct.refFreq = 0;
	i2cOpenStruct.out_SCL_route = SI7021_SCL_LOC;		// out 0 route to gpio port/pin
	i2cOpenStruct.out_SDA_route = SI7021_SDA_LOC;		// out 1 route to gpio port/pin
	i2cOpenStruct.out_SCL_en = true;		// enable out 0 route
	i2cOpenStruct.out_SDA_en = true;
//	i2cOpenStruct.read_cb = SI7021_READ_CB;

	i2c_open(I2C0, &i2cOpenStruct);
//void i2c_Start(enStates states, I2C_TypeDef *i2c, uint32_t I2c_address,
//	uint32_t I2C_reg_address, bool write,bool read, uint32_t *data,uint32_t size,uint32_t read_cb);
}
/***************************************************************************//**
 * @brief
 *  Starts the State machine
 *
 * @details
 * Enters in I2c, Slave address, the command, our result and our callback event
 *
 * @note
 * A key function in setting up the peripheral. No inputs or outputs
 *
 *
 ******************************************************************************/
void si7021_Read(uint32_t done_cb){
	i2c_Start(I2C0,SI7021_ADDRESS, SI7021_CMD,0, &result, done_cb);
}
/***************************************************************************//**
 * @brief
 *  Converts result into readable value
 *
 * @details
 * uses conversion from si7021 datasheet
 *
 * @note
 * no inputs outputs float
 *
 *
 ******************************************************************************/
float si7021_convert(void){
	float temp = 0;
	temp = ((175.72 * ((float)result))/65536)-48.85;
	temp = 1.8*temp + 32;
	return temp;
}
