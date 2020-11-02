/*
 * Si7021.h
 *
 *  Created on: Oct 6, 2020
 *      Author: Nicholas Wroblewski
 */

#ifndef SRC_HEADER_FILES_SI7021_H_
#define SRC_HEADER_FILES_SI7021_H_


//***********************************************************************************
// Include files
//***********************************************************************************

#include "i2c.h"
#include "brd_config.h"

//***********************************************************************************
// defined files
//***********************************************************************************

#define MUL 175.72
#define DIV 65536
#define SUB 46.85
#define FMUL 1.8
#define ADD 32


//***********************************************************************************
// function prototypes (global)
//***********************************************************************************

void si7021_i2c_open(I2C_TypeDef *i2c);
void si7021_read(I2C_TypeDef *i2c, uint32_t callback);
float si7021_temp();

#endif /* SRC_HEADER_FILES_SI7021_H_ */
