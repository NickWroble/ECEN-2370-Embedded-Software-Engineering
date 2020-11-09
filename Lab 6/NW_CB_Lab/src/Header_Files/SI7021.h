//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef SRC_HEADER_FILES_SI7021_H_
#define SRC_HEADER_FILES_SI7021_H_

/* System include statements */


/* Silicon Labs include statements */
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_assert.h"
#include "i2c.h"
/* The developer's include statements */
#include "brd_config.h"

//***********************************************************************************
// defined files
//***********************************************************************************
#define I2C_FREQ_FAST_MAX   392157

#define SI7021_I2C_PERIPHERAL_0
#ifdef SI7021_I2C_PERIPHERAL_0
	#define SI7021_I2C  	I2C0
	#define SI7021_SCL_LOC 	I2C_ROUTELOC0_SCLLOC_LOC15
	#define SI7021_SDA_LOC 	I2C_ROUTELOC0_SDALOC_LOC15
#else
	#define SI7021_I2C  	I2C1
	#define SI7021_SCL_LOC 	I2C_ROUTELOC0_SCLLOC_LOC19
	#define SI7021_SDA_LOC 	I2C_ROUTELOC0_SDALOC_LOC19
#endif

#define SI7021_ADDRESS  0x40
#define SI7021_CMD 		0xf3
//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void si7021_i2c_open(void);
void si7021_Read(uint32_t done_cb);
float si7021_convert(void);

#endif /* SRC_HEADER_FILES_SI7021_H_ */
