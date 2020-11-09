/**
 * @file app.c
 * @author Nicholas Wroblewski
 * @date 9/12/2020
 * @brief Contains Application Function.
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "app.h"

//***********************************************************************************
// defined files
//***********************************************************************************
//#define BLE_TEST_ENABLED

//***********************************************************************************
// Static / Private Variables
//***********************************************************************************


//***********************************************************************************
// Private functions
//***********************************************************************************

static void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route,  uint32_t le_comp0_cb, uint32_t le_comp1_cb, uint32_t le_uf_cb);


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *  Sets up the peripheral
 *
 * @details
 *  First step is to enter CMU in order to start the clock.
 *  Next, initializes the pins to the correct location.
 *  Then opens the LETIMER peripheral
 *  and starts the program.
 *
 * @note
 * A key function in setting up the peripheral.
 *
 *
 ******************************************************************************/

void app_peripheral_setup(void){
	cmu_open();
	gpio_open();
	scheduler_open();
	sleep_open();
	si7021_i2c_open();
	sleep_block_mode(SYSTEM_BLOCK_EM);
	app_letimer_pwm_open(PWM_PER, PWM_ACT_PER, PWM_ROUTE_0, PWM_ROUTE_1, LETIMER0_COMP0_CB, LETIMER0_COMP1_CB, LETIMER0_UF_CB);
	ble_open(TX_DONE_EVENT, RX_DONE_EVENT);
	add_scheduled_event(BOOT_UP_CB);


}

/***************************************************************************//**
 * @brief
 * Initializes the peripheral LETIMER for the PWM operation.
 *
 *
 * @details
 * Initializes the STRUCT before putting it into LETIMER0;
 *
 * @note
 * Opens LETIMER_PWM_open and puts app_pwm into LETIMER_PWM_open
 *
 * @param[in] act_period
 * The time the LEDs are on
 *
 * @param[in] out0 route
 *  Where LED0 is routed.
 *
 * @param[in] out1 route
 * Where LED1 is routed.
 *
 ******************************************************************************/
void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route, uint32_t le_comp0_cb, uint32_t le_comp1_cb, uint32_t le_uf_cb){
	// Initializing LETIMER0 for PWM operation by creating the
	// letimer_pwm_struct and initializing all of its elements

	APP_LETIMER_PWM_TypeDef app_letimer;
	app_letimer.period = period;
	app_letimer.active_period = act_period;
	app_letimer.debugRun = false;
	app_letimer.enable = false;
	app_letimer.out_pin_0_en = false;
	app_letimer.out_pin_1_en = false;

	app_letimer.out_pin_route0 = out0_route;
	app_letimer.out_pin_route1 = out1_route;
	app_letimer.uf_irq_enable = true;
	app_letimer.comp0_irq_enable = false;
	app_letimer.comp1_irq_enable = false;

	app_letimer.comp0_cb = le_comp0_cb;
	app_letimer.comp1_cb = le_comp1_cb;
	app_letimer.uf_cb = le_uf_cb;

	letimer_pwm_open(LETIMER0, &app_letimer);


	// letimer_start will inform the LETIMER0 peripheral to begin counting.

}

/***************************************************************************//**
 * @brief
 * Removes the interrupt and then unblocks the current Energy mode and blocks the next one up
 *
 *
 * @details
 * First, it checks to make sure an event is there. then it removes the event. After removal the code then unblocks the current energy mode.
 *  If the Current energy mode is less than 4, then it will block the next one up. Other wise it will block EM0.
 *
 * @note
 * No inputs or outputs
 *
 *
 ******************************************************************************/


void scheduled_letimer0_uf_cb(void){
//	uint32_t CBEM;
	EFM_ASSERT(get_scheduled_events() & LETIMER0_UF_CB);
	remove_scheduled_event(LETIMER0_UF_CB);
//	CBEM = current_block_energy_mode();
//	sleep_unblock_mode(CBEM);
//	if(CBEM < 4){
//		sleep_block_mode(CBEM+1);
//	}
//	else if(CBEM == 4){
//		sleep_block_mode(EM0);
//	}

	si7021_Read(SI7021_READ_CB);
}
/***************************************************************************//**
 * @brief
 * Removes the interrupt and reads temp, if above 80 degrees it will light up
 *
 *
 * @details
 * runs after Uf callback to ensure interrupt is done
 *
 * @note
 * No inputs or outputs
 *
 *
 ******************************************************************************/

void si7021_temp_done(void){
	EFM_ASSERT(get_scheduled_events() & SI7021_READ_CB);
	remove_scheduled_event(SI7021_READ_CB);
	float temp = 0;
	temp = si7021_convert();
	char tempf[60];
	int error;
	error = sprintf(tempf,"\nTemperature  = %.1f F\n", temp);
	EFM_ASSERT(error != 0);
	ble_write(tempf);
	if(temp > 80.0){
		GPIO_PinOutSet(LED1_PORT,LED1_PIN);
	}
	else{
		GPIO_PinOutClear(LED1_PORT,LED1_PIN);
	}
}
/***************************************************************************//**
 * @brief
 * Removes the interrupt
 *
 *
 * @details
 * Removes interruput
 *
 * @note
 * No inputs or outputs
 *
 *
 ******************************************************************************/
void scheduled_letimer0_comp0_cb(void){
	EFM_ASSERT(false);
	remove_scheduled_event(LETIMER0_COMP0_CB);
}
/***************************************************************************//**
 * @brief
 * Removes the interrupt
 *
 *
 * @details
 * Removes interruput
 *
 * @note
 * No inputs or outputs
 *
 *
 ******************************************************************************/
void scheduled_letimer0_comp1_cb(void){
	EFM_ASSERT(false);
	remove_scheduled_event(LETIMER0_COMP1_CB);
}
/***************************************************************************//**
 * @brief
 *  runs ble test and a test write
 *
 *
 * @details
 * Removes interruput
 *
 * @note
 * No inputs or outputs
 *
 *
 ******************************************************************************/
void scheduled_boot_up_cb(void){
	EFM_ASSERT(get_scheduled_events() & BOOT_UP_CB);
	remove_scheduled_event(BOOT_UP_CB);
#ifdef BLE_TEST_ENABLED
	bool error =ble_test("Nick");
	EFM_ASSERT(error);
	timer_delay(2000);

#endif
	circular_buff_test();
	ble_write("\nHello World!\n");
	ble_write("\nCB Lab\n");
	ble_write("\nNick Wroble\n");
	letimer_start(LETIMER0, true);
}
/***************************************************************************//**
 * @brief
 * Removes the interrupt
 *
 *
 * @details
 * Removes interruput
 *
 * @note
 * No inputs or outputs
 *
 *
 ******************************************************************************/
void scheduled_tx_done_event(void){
	EFM_ASSERT(get_scheduled_events() & TX_DONE_EVENT);
	remove_scheduled_event(TX_DONE_EVENT);
	if(!ble_circ_pop(CIRC_OPER))
		ble_circ_pop(CIRC_OPER);
}

