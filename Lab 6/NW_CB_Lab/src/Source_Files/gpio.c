/**
 * @file gpio.c
 * @author Nicholas Wroblewski
 * @date 9/12/2020
 * @brief Contains Application Function
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
/***************************************************************************//**
 * @brief
 *  Gives LED0 and LED1 an assigned location. It also sets up the SCL and SDA lines and enable bit.
 *
 * @details
 * This gives the board the location to send the signals.
 *
 * @note
 * A key function in setting up the peripheral.
 *
 *
 ******************************************************************************/

void gpio_open(void){

	CMU_ClockEnable(cmuClock_GPIO, true);

	// Configure LED pins
	GPIO_DriveStrengthSet(LED0_PORT, LED0_DRIVE_STRENGTH);
	GPIO_PinModeSet(LED0_PORT, LED0_PIN, LED0_GPIOMODE, LED0_DEFAULT);

	GPIO_DriveStrengthSet(LED1_PORT, LED1_DRIVE_STRENGTH);
	GPIO_PinModeSet(LED1_PORT, LED1_PIN, LED1_GPIOMODE, LED1_DEFAULT);

	//Configure Sensor enable pins
	GPIO_DriveStrengthSet(SI7021_SENSOR_EN_PORT, gpioDriveStrengthWeakAlternateWeak );
	GPIO_PinModeSet(SI7021_SENSOR_EN_PORT,SI7021_SENSOR_EN_PIN, gpioModePushPull,true);

	//Configure SDA and SCL
	GPIO_PinModeSet(SI7021_SCL_PORT,SI7021_SCL_PIN,gpioModeWiredAnd,true);
	GPIO_PinModeSet(SI7021_SDA_PORT,SI7021_SDA_PIN,gpioModeWiredAnd,true);

	//Configure RX and TX
	GPIO_DriveStrengthSet(UART_TX_PORT, gpioDriveStrengthStrongAlternateWeak);
	GPIO_PinModeSet(UART_TX_PORT,UART_TX_PIN,gpioModePushPull,true);

	GPIO_PinModeSet(UART_RX_PORT,UART_RX_PIN,gpioModeInput,true);

}
