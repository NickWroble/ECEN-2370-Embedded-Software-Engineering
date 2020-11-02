/**
 * @file gpio.c
 *
 * @author Nicholas Wroblewski
 *
 * @date September 2tth, 2020
 *
 * @brief Set up GPIO drivers
 *
 */
//***********************************************************************************
// Include files
//***********************************************************************************
#include "gpio.h"


//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************


//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

void gpio_open(void){
	CMU_ClockEnable(cmuClock_GPIO, true);

	//Set drive strength for enable pin on si7021 and set it
	GPIO_DriveStrengthSet(SI7021_SENSOR_EN_PORT, SI7021_DRIVE_STRENGTH);
	GPIO_PinModeSet(SI7021_SENSOR_EN_PORT, SI7021_SENSOR_EN_PIN, SI7021_PUSH_PULL, SI7021_DRIVE_STRENGTH);

	//GPIO_DriveStrengthSet not needed for SDA or SCL
	GPIO_PinModeSet(SI7021_SCL_PORT,  SI7021_SCL_PIN, SI7021_GPIOMODE, SI7021_DEFAULT);

	GPIO_PinModeSet(SI7021_SDA_PORT,  SI7021_SDA_PIN, SI7021_GPIOMODE, SI7021_DEFAULT);

	// Configure LED pins
	GPIO_DriveStrengthSet(LED0_PORT, LED0_DRIVE_STRENGTH);
	GPIO_PinModeSet(LED0_PORT, LED0_PIN, LED0_GPIOMODE, LED0_DEFAULT);

	GPIO_DriveStrengthSet(LED1_PORT, LED1_DRIVE_STRENGTH);
	GPIO_PinModeSet(LED1_PORT, LED1_PIN, LED1_GPIOMODE, LED1_DEFAULT);
}
