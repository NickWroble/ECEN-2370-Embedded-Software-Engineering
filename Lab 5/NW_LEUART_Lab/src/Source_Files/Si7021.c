/**
 * @file Si7021.c
 *
 * @author Nicholas Wroblewski
 *
 * @date October 11th, 2020
 *
 * @brief Setting up the Si7021
 *
 */
//***********************************************************************************
// Include files
//***********************************************************************************

#include "Si7021.h"

//***********************************************************************************
// Static / Private Variables
//**********************************************************************************
uint32_t data_read;

//***********************************************************************************
// Global functions
//**********************************************************************************

/***************************************************************************//**
 * @brief This function enables the communication between the si7021 and the pearl gecko
 *
 *
 * @details After the initialization of each variable to its proper function i2c_open is called
 * in order to start the state machine
 *
 *
 * @param[in]I2C_TypeDef *i2c
 * This input argument allows us to control the registers of the I2C
 *
 ******************************************************************************/
void si7021_i2c_open(I2C_TypeDef *i2c){
	I2C_OPEN_STRUCT i2copen_struct;
	i2copen_struct.clhr = i2cClockHLRAsymetric;
	i2copen_struct.enable = true;
	i2copen_struct.freq = I2C_FREQ_FAST_MAX;
	i2copen_struct.master = false;
	i2copen_struct.out_SCLLOC_route = SCLLOC;
	i2copen_struct.out_SDALOC_route = SDALOC;
	i2copen_struct.out_route_SCLEN = true;
	i2copen_struct.out_route_SDAEN = true;
	i2copen_struct.refFreq = 0;

	i2c_open(i2c, &i2copen_struct);

}

/***************************************************************************//**
 * @brief This function reads the temperature
 *
 *
 * @details Uses the I2C start function with inputs including i2c (which specifies the i2c in use),
 * SLADD (specific slave address), SLREG (specific slave register), stored callback value
 *
 *
 * @param[in] I2C_TypeDef *i2c
 *  This input argument allows us to control the registers of the I2C
 *
 * @param[in] uint32_t callback
 * Previously set in the i2c_start function
 *
 ******************************************************************************/
void si7021_read(I2C_TypeDef *i2c, uint32_t callback){
	i2c_start(i2c, SLADD, SLREG, callback);
}

/***************************************************************************//**
 * @brief This function converts the temperature code to a fahrenheit
 *
 *
 * @details Uses the read_data function in order to collect the temperature code from I2C
 *
 *
 * @note This function returns the fahrenheit temperature
 *
 ******************************************************************************/
float si7021_temp(){
	float c_temp;
	float f_temp;
	read_data(&data_read);// From I2c function
	c_temp = ((MUL*data_read)/DIV)-SUB;
	f_temp = (FMUL *c_temp)+ ADD;
	return f_temp;
}
