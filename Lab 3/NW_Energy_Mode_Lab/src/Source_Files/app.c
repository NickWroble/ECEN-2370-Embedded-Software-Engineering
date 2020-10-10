/**
 * @file app.c
 *
 * @author Nicholas Wroblewski
 *
 * @date September 27th, 2020
 *
 * @brief Setting up peripherals
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

static void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route,
		uint32_t comp0_cb, uint32_t comp1_cb, uint32_t uf_cb);

//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief Defines values of APP_LETIMER_PWM_TypeDef for pwm
 *
 *
 *
 * @details Enables cmu_open and gpio_open and inits required inputs for
 * app_letimer_pwm. Also enables letimer_start and calls letimer_start(LETIMER0, true)
 *
 *
 * @note
 * cmu_open = Enables oscillators and routes clock tree
 * gpio_open = enables gpio
 * app_letimer_pwm_open(PWM_PER, PWM_ACT_PER, PWM_ROUTE_0, PWM_ROUTE_1): enables letimer and establishes parameters
 * 	PWM_PER= Period in seconds
 * 	PWM_ACT_PER= Active period in seconds
 * 	PWM_ROUTE_0= Register location for LED0
 * 	PWM_ROUTE_1= Register location for LED1
 *
 *
 ******************************************************************************/

void app_peripheral_setup(void){
	sleep_open();
	scheduler_open();
	cmu_open();
	gpio_open();
	app_letimer_pwm_open(PWM_PER, PWM_ACT_PER, PWM_ROUTE_0, PWM_ROUTE_1,LETIMER0_COMP0_CB, LETIMER0_COMP1_CB,LETIMER0_UF_CB);
	letimer_start(LETIMER0, true);
}

/***************************************************************************//**
 * @brief Inits LETIMER0 for pwm
 *
 *
 * @details letimer_pwm_struct is initted for all of the elements for APP_LETIMER_PWM_TypeDef
 *
 *
 *
 * @note letimer_pwm_open(LETIMER0, &letimer_pwm_struct) passes letimer_pwm_struct to open LETIMER0
 *
 *
 *
 * @param[in] float period
 *
 *
 * @param[in] float act_period
 *
 *
 * @param[in] uint32_t out0_route
 *
 *
 * @param[in] uint32_t out1_route
 ******************************************************************************/
void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route,
		uint32_t comp0_cb, uint32_t comp1_cb, uint32_t uf_cb){


	APP_LETIMER_PWM_TypeDef letimer_pwm_struct;

	letimer_pwm_struct.debugRun = false;			// True = keep LETIMER running will halted
	letimer_pwm_struct.enable = false;				// enable the LETIMER upon completion of open
	letimer_pwm_struct.out_pin_route0 = out0_route; 		// out 0 route to gpio port/pin
	letimer_pwm_struct.out_pin_route1 = out1_route;		// out 1 route to gpio port/pin
	letimer_pwm_struct.out_pin_0_en = true;		// enable out 0 route
	letimer_pwm_struct.out_pin_1_en = true;		// enable out 1 route
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
 * @brief Clears "LETIMER0_UF_CB"
 *
 *
 * @details Function saves energy modes that could change bc of external ISR events
 *
 *
 * @note
 *
 ******************************************************************************/

void scheduled_letimer0_uf_cb(void){
	EFM_ASSERT(get_scheduled_events() & LETIMER0_UF_CB);
	int i = current_block_energy_mode();
	sleep_unblock_mode(i);
	if(i<4)
		sleep_block_mode(i+1);
	else
		sleep_block_mode(EM0);
	remove_scheduled_event(LETIMER0_UF_CB);
}

/***************************************************************************//**
 * @brief Clears scheduled event "LETIMER0_COMP0_CB"
 *
 *
 * @details Uses assertion to make sure event is properly cleared
 *
 *
 * @note This event is cleared so that it can be called again and triggered
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
 * @details Uses assert to ensure clear of SE
 *
 *
 * @note
 *
 ******************************************************************************/
void scheduled_letimer0_comp1_cb(void){
	EFM_ASSERT(false);
	remove_scheduled_event(LETIMER0_COMP1_CB);
}
