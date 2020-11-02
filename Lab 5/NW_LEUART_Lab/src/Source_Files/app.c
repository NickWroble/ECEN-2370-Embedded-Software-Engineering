/**
 * @file app.c
 *
 * @author Nicholas Wroblewski
 *
 * @date September 10th, 2020
 *
 * @brief Setting up peripherals
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************

#include "app.h"
#include "Si7021.h"
#include "i2c.h"
#include "HW_delay.h"
#include "stdlib.h"

// #define BLE_TEST_ENABLE //comment this out to rename the bluetooth module


//***********************************************************************************
// Private functions
//***********************************************************************************

static void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route,
		uint32_t comp0_cb, uint32_t comp1_cb, uint32_t uf_cb);

//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief Initializes APP_LETIMER_PWM_TypeDef for temperature measurement
 *
 *
 * @details Enables cmu_open and gpio_open and initializes inputs for the
 * app_letimer_pwm function. Enables app_letimer_pwm and inputs in letimer_start. Lastly it enables the
 * si7021_i2c_open function to begin communication with I2C0
 *
 *
 * @note
 * cmu_open : enables oscillators and routes clock tree
 * gpio_open : enables gpio
 * app_letimer_pwm_open(PWM_PER, PWM_ACT_PER, PWM_ROUTE_0, PWM_ROUTE_1): enables letimer and inputs
 * 	PWM_PER: period in seconds (2.7)
 * 	PWM_ACT_PER: active period in seconds (.15)
 * 	PWM_ROUTE_0: Register route location for LED0
 * 	PWM_ROUTE_1: Register route location for LED1
 *
 *
 ******************************************************************************/
void app_peripheral_setup(void){
	sleep_open();
	scheduler_open();
	cmu_open();
	gpio_open();
	app_letimer_pwm_open(PWM_PER, PWM_ACT_PER, PWM_ROUTE_0, PWM_ROUTE_1,LETIMER0_COMP0_CB, LETIMER0_COMP1_CB,LETIMER0_UF_CB);
	si7021_i2c_open(I2C0);
	sleep_block_mode(SYSTEM_BLOCK_EM);
	ble_open(TXEVENT, RXEVENT);
	add_scheduled_event(BOOT_UP_CB);
}

/***************************************************************************//**
 * @brief Initializes LETIMER0
 *
 *
 * @details letimer_pwm_struct is initialized
 * for the APP_LETIMER_PWM_TypeDef.
 *
 *
 * @note letimer_pwm_open(LETIMER0, &letimer_pwm_struct) passes the created
 * STRUCT: letimer_pwm_struct to open the LETIMER0 peripheral
 *
 *
 * @param[in] period defines period as float for the LETIMER0 to run PWM_PER
 *
 *
 * @param[in] act_period defines the active period as a float for the LETIMER0 to run PWM_ACT_PER
 *
 *
 * @param[in] uint32_t out0_route defines the route in order to locate PWM_ROUTE_0
 *
 *
 * @param[in] uint32_t out1_route defines the route in order to locate PWM_ROUTE_1
 ******************************************************************************/
void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route,
		uint32_t comp0_cb, uint32_t comp1_cb, uint32_t uf_cb){

	// Initializing LETIMER0 for PWM operation by creating the
	// letimer_pwm_struct and initializing all of its elements

	APP_LETIMER_PWM_TypeDef letimer_pwm_struct;

	letimer_pwm_struct.debugRun = false;
	letimer_pwm_struct.enable = false;
	letimer_pwm_struct.out_pin_route0 = out0_route;
	letimer_pwm_struct.out_pin_route1 = out1_route;
	letimer_pwm_struct.out_pin_0_en = false;
	letimer_pwm_struct.out_pin_1_en = false;
	letimer_pwm_struct.period =  period;
	letimer_pwm_struct.active_period = act_period;

	letimer_pwm_struct.uf_irq_enable = true;
	letimer_pwm_struct.comp0_irq_enable = false;
	letimer_pwm_struct.comp1_irq_enable = false;
	letimer_pwm_struct.comp0_cb = comp0_cb;
	letimer_pwm_struct.comp1_cb = comp1_cb;
	letimer_pwm_struct.uf_cb = LETIMER0_UF_CB;
	letimer_pwm_open(LETIMER0, &letimer_pwm_struct);
}

/***************************************************************************//**
 * @brief Clears scheduled event "LETIMER0_UF_CB"
 *
 *
 * @details Function prevents changes in energy modes and reads the current state of the Si7021 callback
 * in either I2C0 or I2C1
 *
 *
 * @note This event is cleared so that it can be called again
 *
 ******************************************************************************/

void scheduled_letimer0_uf_cb(void){
	EFM_ASSERT(get_scheduled_events() & LETIMER0_UF_CB);
	remove_scheduled_event(LETIMER0_UF_CB);
	si7021_read(I2C01, SI7021_READ_CB);

}


/***************************************************************************//**
 * @brief Clears scheduled event "LETIMER0_COMP0_CB"
 *
 *
 * @details Uses assertion to make sure event is cleared
 *
 *
 * @note This event is cleared so that it can be called again
 *
 ******************************************************************************/
void scheduled_letimer0_comp0_cb(void){
	EFM_ASSERT(false);
	remove_scheduled_event(LETIMER0_COMP0_CB);
}
/***************************************************************************//**
 * @brief Clears scheduled event "LETIMER0_COMP1_CB"
 *
 *
 * @details Uses assertion to make sure event cleared
 *
 *
 * @note This event is cleared so that it can be called again
 *
 ******************************************************************************/
void scheduled_letimer0_comp1_cb(void){
	EFM_ASSERT(false);
	remove_scheduled_event(LETIMER0_COMP1_CB);
}

/***************************************************************************//**
 * @brief Turns LED1 on when Si7021 reads above 80 degrees (F)
 *
 *
 * @details Uses assertion to make sure event is properly cleared
 *
 *
 * @note The LED is off when the sensor measures less than 80 degrees (F)
 *
 ******************************************************************************/

void scheduled_si7021_done(void){
	EFM_ASSERT(get_scheduled_events() & SI7021_READ_CB);
	remove_scheduled_event(SI7021_READ_CB);
	float temp = si7021_temp();
	char temp_data[25];
	int tempInt;
	tempInt = sprintf(temp_data, "\nTemp = %2.1f F\n", temp);
	EFM_ASSERT(tempInt);
	ble_write(temp_data, BLE_CALLBACK);
	if(temp >= 80)
		GPIO_PinOutSet(LED1_PORT, LED1_PIN);
	else
		GPIO_PinOutClear(LED1_PORT, LED1_PIN);
}

/***************************************************************************//**
 * @brief Handling routine for starting up the BLE module.
 *
 *
 * @details Ensures that the boot up callback is enabled, then disables it. See notes for renaming the device.
 * Transmits "Hello World" at startup.
 *
 *
 * @note Uncomment the BLE_TEST_ENABLE definition at the top of app.c to rename the BLE module
 *
 ******************************************************************************/
void scheduled_boot_up_cb(void){
    EFM_ASSERT(get_scheduled_events() & BOOT_UP_CB);
    remove_scheduled_event(BOOT_UP_CB);

	#ifdef BLE_TEST_ENABLE
		bool boolean = ble_test("Nick_BLE");
		timer_delay(2000);
		EFM_ASSERT(boolean);
	#endif

    ble_write("\nHello World\n", BLE_CALLBACK);
    letimer_start(LETIMER0, true);
}

/***************************************************************************//**
 * @brief Removes BLE from scheduled events
 *
 * @details Uses an assertion to check if the BLE callback is true
 *
 ******************************************************************************/
void schdeduled_ble_done(void){
    EFM_ASSERT(get_scheduled_events () & BLE_CALLBACK);
    remove_scheduled_event(BLE_CALLBACK);
}
