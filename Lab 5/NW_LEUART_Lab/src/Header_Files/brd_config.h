#ifndef BRD_CONFIG_HG
#define BRD_CONFIG_HG

//***********************************************************************************
// Include files
//***********************************************************************************
/* System include statements */


/* Silicon Labs include statements */
#include "em_cmu.h"
#include "em_gpio.h"

/* The developer's include statements */


//***********************************************************************************
// defined files
//***********************************************************************************

#define chooseI2C0
#ifdef	chooseI2C0
	#define I2C01		I2C0
	#define	SDALOC		I2C_ROUTELOC0_SDALOC_LOC15
	#define	SCLLOC		I2C_ROUTELOC0_SCLLOC_LOC15

#else
	#define	I2C01		I2C1
	#define	SDALOC		I2C_ROUTELOC0_SDALOC_LOC19
	#define	SCLLOC		I2C_ROUTELOC0_SCLLOC_LOC19
#endif

#define SLADD 			0b1000000
#define SLREG 			0xF3
#define I2C_FAST_FREQ			I2C_FREQ_FAST_MAX

#define SI7021_MCMD_TEMP_NHOLD_MASTER 	0xF3  //command for measuring temp in no hold master mode

// GPIO pin setup
#define STRONG_DRIVE

// LED 0 pin is
#define	LED0_PORT				gpioPortF
#define LED0_PIN				04u
#define LED0_DEFAULT			false 	// Default false (0) = off, true (1) = on
#define LED0_GPIOMODE			gpioModePushPull

// LED 1 pin is
#define LED1_PORT				gpioPortF
#define LED1_PIN				05u
#define LED1_DEFAULT			false	// Default false (0) = off, true (1) = on
#define LED1_GPIOMODE			gpioModePushPull

#ifdef STRONG_DRIVE
	#define LED0_DRIVE_STRENGTH		gpioDriveStrengthStrongAlternateStrong
	#define LED1_DRIVE_STRENGTH		gpioDriveStrengthStrongAlternateStrong
#else
	#define LED0_DRIVE_STRENGTH		gpioDriveStrengthWeakAlternateWeak
	#define LED1_DRIVE_STRENGTH		gpioDriveStrengthWeakAlternateWeak
#endif


// System Clock setup
#define MCU_HFXO_FREQ			cmuHFRCOFreq_32M0Hz



// LETIMER PWM Configuration

#define		PWM_ROUTE_0			LETIMER_ROUTELOC0_OUT0LOC_LOC28
#define		PWM_ROUTE_1			LETIMER_ROUTELOC0_OUT1LOC_LOC28

//Sensor Defines
#define SI7021_GPIOMODE			gpioModeWiredAnd
#define SI7021_PUSHPULL			gpioModePushPull
#define SI7021_DEFAULT			true
#define SI7021_DRIVE_STRENGTH	gpioDriveStrengthWeakAlternateWeak

#define SI7021_SCL_PORT 		gpioPortC
#define SI7021_SCL_PIN 			11u
#define SI7021_SDA_PORT 		gpioPortC
#define SI7021_SDA_PIN 			10u
#define SI7021_SENSOR_EN_PORT 	gpioPortB
#define SI7021_SENSOR_EN_PIN 	10u

#define RX_PIN 				11u
#define RX_PORT 			gpioPortD
#define RX_DEFAULT 			true

#define TX_PORT 			gpioPortD
#define TX_PIN 				10u
#define TX_DEFAULT 			true
#define TX_DRIVE_STRENGTH 	gpioDriveStrengthStrongAlternateWeak

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************

#endif
