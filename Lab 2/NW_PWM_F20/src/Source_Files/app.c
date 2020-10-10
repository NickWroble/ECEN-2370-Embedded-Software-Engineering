/**
 * @file app.c
 * @author Nihcolas Wroblewski
 * @date 13 September 2020
 * @biref Setting up peripherals
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "app.h"


//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Static / Private Variables
//***********************************************************************************


//***********************************************************************************
// Private functions
//***********************************************************************************

static void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route);

//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief Sets vars of APP_LETIMER_PWM_TypeDef for pwm
 *
 *
 * @details Calls gpio_open and cmu_open and inits vars for app_letimer_pwm function
 * Enables app_letimer_pwm and calls letimer_start
 *
 *
 * @note
 * cmu_open:  routes clock tree and enabled oscillator
 * gpio_open: enables gpio
 * app_letimer_pwm_opne(****); enables letimer and sets inputs
 *
 *
 *
 ******************************************************************************/

void app_peripheral_setup(void){
	cmu_open();
	gpio_open();
  	app_letimer_pwm_open(PWM_PER, PWM_ACT_PER, PWM_ROUTE_0, PWM_ROUTE_1);
	letimer_start(LETIMER0, true);
}

/***************************************************************************//**
 * @brief Inits letimer0 for pwm
 *
 *
 * @details letimer_pwm_struct used for values in APP_LETIMER_PWM_TypeDef
 *
 *
 * @note letimer_pwm_open(****) passes letimer_pwm_struct for LETIMER0
 *
 *
 * @param[in] period Total period of LETIMER0
 *
 *
 * @param[in] act_period Active period of LETIMER0
 *
 ******************************************************************************/
void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route){
	APP_LETIMER_PWM_TypeDef letimer_pwm_struct;

	letimer_pwm_struct.debugRun = false;
	letimer_pwm_struct.enable = false;
	letimer_pwm_struct.out_pin_route0 = out0_route;
	letimer_pwm_struct.out_pin_route1 = out1_route;
	letimer_pwm_struct.out_pin_0_en = true;
	letimer_pwm_struct.out_pin_1_en = true;
	letimer_pwm_struct.period = period;
	letimer_pwm_struct.active_period = act_period;

	// Initializing LETIMER0 for PWM operation by creating the
	// letimer_pwm_struct and initializing all of its elements
	letimer_pwm_open(LETIMER0, &letimer_pwm_struct);

	// letimer_start will inform the LETIMER0 peripheral to begin counting.
//	letimer_start(LETIMER0, true);
}


