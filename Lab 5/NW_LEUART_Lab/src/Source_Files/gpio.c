/**
 * @file gpio.c
 *
 * @author Nicholas Wroblewski
 *
 * @date September 10th, 2020
 *
 * @brief Sets up GPIO drivers
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

	// Configure LED pins
	GPIO_DriveStrengthSet(LED0_PORT, LED0_DRIVE_STRENGTH);
	GPIO_PinModeSet(LED0_PORT, LED0_PIN, LED0_GPIOMODE, LED0_DEFAULT);

	GPIO_DriveStrengthSet(LED1_PORT, LED1_DRIVE_STRENGTH);
	GPIO_PinModeSet(LED1_PORT, LED1_PIN, LED1_GPIOMODE, LED1_DEFAULT);

	// Configure sensor
	GPIO_DriveStrengthSet(SI7021_SENSOR_EN_PORT , SI7021_DRIVE_STRENGTH);
	GPIO_PinModeSet(SI7021_SENSOR_EN_PORT, SI7021_SENSOR_EN_PIN, SI7021_PUSHPULL,SI7021_DEFAULT);
	GPIO_PinModeSet(SI7021_SDA_PORT, SI7021_SDA_PIN , gpioModeWiredAnd, SI7021_DEFAULT);
	GPIO_PinModeSet(SI7021_SCL_PORT , SI7021_SCL_PIN , gpioModeWiredAnd, SI7021_DEFAULT);

	GPIO_DriveStrengthSet(TX_PORT , TX_DRIVE_STRENGTH);
	GPIO_PinModeSet(TX_PORT , TX_PIN , gpioModePushPull, TX_DEFAULT);
	GPIO_PinModeSet(RX_PORT , RX_PIN , gpioModeInput, RX_DEFAULT);
}
