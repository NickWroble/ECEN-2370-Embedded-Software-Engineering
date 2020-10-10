/**
 * @file letimer.c
 * @author Nicholas Wroblewski
 * @date September 27th, 2020
 * @brief LETIMER drivers
 *
 */


//***********************************************************************************
// Include files
//***********************************************************************************

//** Standard Libraries

//** Silicon Lab include files

//** User/developer include files
#include "letimer.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************
static uint32_t scheduled_comp0_cb;
static uint32_t scheduled_comp1_cb;
static uint32_t scheduled_uf_cb;
//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief Opens and sets up PWM for LETIMER0
 *
 * @details
 *	LETIMER PWM's the CPU's gpio pins for interrupts. Also sets up ISH
 *
 *
 *
 * @note Call function once to init then call letimer_start to toggle PWM

 *
 * @param[in] letimer
 * Pointer to the base peripheral address of LETIMER peripheral
 *
 * @param[in] app_letimer_struct
 *   Is the STRUCT that was previously setup
 *
 ******************************************************************************/
void letimer_pwm_open(LETIMER_TypeDef *letimer, APP_LETIMER_PWM_TypeDef *app_letimer_struct){
	LETIMER_Init_TypeDef letimer_pwm_values;
	unsigned int period_cnt; //lab2
	unsigned int period_active_cnt; //lab2

	/*  Initializing LETIMER for PWM mode */
	/*  Enable the routed clock to the LETIMER0 peripheral */

	if(letimer==LETIMER0){
		CMU_ClockEnable(cmuClock_LETIMER0, true);}

	letimer_start(letimer,false);

	letimer->CMD = LETIMER_CMD_START;

	while (letimer->SYNCBUSY);
	EFM_ASSERT(letimer->STATUS & LETIMER_STATUS_RUNNING);
	letimer->CMD =LETIMER_CMD_STOP;
	while(letimer->SYNCBUSY);



	/* Use EFM_ASSERT statements to verify whether the LETIMER clock tree is properly
	 * configured and enabled
	 * You must select a register that utilizes the clock enabled to be tested
	 * With the LETIMER regiters being in the low frequency clock tree, you must
	 * use a while SYNCBUSY loop to verify that the write of the register has propagated
	 * into the low frequency domain before reading it. */



	// Must reset the LETIMER counter register since enabling the LETIMER to verify that
	// the clock tree has been correctly configured to the LETIMER may have resulted in
	// the counter counting down from 0 and underflowing which by default will load
	// the value of 0xffff.  To load the desired COMP0 value quickly into this
	// register after complete initialization, it must start at 0 so that the underflow
	// will happen quickly upon enabling the LETIMER loading the desired top count from
	// the COMP0 register.

	// Reset the Counter to a know value such as 0
	letimer->CNT = 0;	// What is the register enumeration to use to specify the LETIMER Counter Register?

	// Initialize letimer for PWM operation
	// XXX are values passed into the driver via app_letimer_struct
	// ZZZ are values that you must specify for this PWM specific driver
	letimer_pwm_values.bufTop = false;		// (ZZZ) Comp1 will not be used to load comp0, but used to create an on-time/duty cycle
	letimer_pwm_values.comp0Top = true;		// (ZZZ) load comp0 into cnt register when count register underflows enabling continuous looping
	letimer_pwm_values.debugRun = app_letimer_struct->debugRun;	// (XXX)
	letimer_pwm_values.enable = app_letimer_struct->enable;		// (XXX)
	letimer_pwm_values.out0Pol = 0;			// (ZZZ) While PWM is not active out, idle is DEASSERTED, 0
	letimer_pwm_values.out1Pol = 0;			//(ZZZ)While PWM is not active out, idle is DEASSERTED, 0
	letimer_pwm_values.repMode = LETIMER_CTRL_REPMODE_FREE;	//(ZZZ) Setup letimer for free running for continuous looping
	letimer_pwm_values.ufoa0 = letimerUFOAPwm ;		//(ZZZ) Using the HAL documentation, set to PWM mode
	letimer_pwm_values.ufoa1 = letimerUFOAPwm ;		// (ZZZ)Using the HAL documentation, set to PWM mode



	LETIMER_Init(letimer, &letimer_pwm_values);		// Initialize letimer



	while(letimer->SYNCBUSY){};
	/* Calculate the value of COMP0 and COMP1 and load these control registers
	 * with the calculated values
	 */
		period_cnt=app_letimer_struct->period*LETIMER_HZ;
		period_active_cnt = app_letimer_struct->active_period*LETIMER_HZ;
		letimer->COMP0 = period_cnt;
		letimer->COMP1 =period_active_cnt;
		letimer->REP0 |= true;
		letimer->REP1 |= true;

	/* Set the REP0 mode bits for PWM operation directly since this driver is PWM specific.
	 * Datasheets are very specific and must be read very carefully to implement correct functionality.
	 * Sometimes, the critical bit of information is a single sentence out of a 30-page datasheet
	 * chapter.  Look careful in the following section of the Pearl Gecko Reference Manual,
	 * 20.3.4 Underflow Output Action, to learn how to correctly set the REP0 and REP1 bits
	 *
	 * Use the values from app_letimer_struct input argument for ROUTELOC0 and ROUTEPEN enable
	 */
		letimer->ROUTELOC0 = app_letimer_struct->out_pin_route0;
		letimer->ROUTELOC0 |= app_letimer_struct->out_pin_route1;
		letimer->ROUTEPEN = (LETIMER_ROUTEPEN_OUT1PEN *app_letimer_struct->out_pin_1_en);
		letimer->ROUTEPEN |= (LETIMER_ROUTEPEN_OUT0PEN *app_letimer_struct->out_pin_0_en);

		/* setting callbacks for Interrupt Service Routine */
		scheduled_uf_cb = app_letimer_struct->uf_cb;
		scheduled_comp0_cb = app_letimer_struct->comp0_cb;
		scheduled_comp1_cb = app_letimer_struct->comp1_cb;
	/* We are not enabling any interrupts at this tie.  If you were, you would enable them now */
		//enabling required interrupts and NVIC

		uint32_t IEN_SET;
			IEN_SET = (app_letimer_struct->uf_irq_enable * LETIMER_IEN_UF );  // 1<<2=100
			IEN_SET |= (app_letimer_struct->comp0_irq_enable * LETIMER_IEN_COMP0); //10
			IEN_SET |= (app_letimer_struct->comp1_irq_enable * LETIMER_IEN_COMP1);  //1
			letimer->IEN = IEN_SET;
			NVIC_EnableIRQ(LETIMER0_IRQn);

	/* We will not enable or turn-on the LETIMER0 at this time */
			if(letimer->STATUS)
				sleep_block_mode(LETIMER_EM);
}

/***************************************************************************//**
 * @brief Interrupt handler
 *
 *
 * @details  Disables other interrupts, IF reg is cleared, each IF bit is handled
 *
 *
 * @note Each interrupt flag is asserted to check if interrupt was serviced
 *
 *
 ******************************************************************************/

void LETIMER0_IRQHandler(void){
	uint32_t int_flag;
	int_flag=LETIMER0->IF & LETIMER0->IEN;
	LETIMER0->IFC=int_flag;

	if (int_flag & LETIMER_IF_UF){
		add_scheduled_event(scheduled_uf_cb);
		EFM_ASSERT(!(LETIMER0->IF & LETIMER_IF_UF));
		}

	if (int_flag & LETIMER_IF_COMP0){
		add_scheduled_event(scheduled_comp0_cb);
		EFM_ASSERT(!(LETIMER0->IF & LETIMER_IF_COMP0));
	}

	if (int_flag & LETIMER_IF_COMP1){
		add_scheduled_event(scheduled_comp1_cb);
		EFM_ASSERT(!(LETIMER0->IF & LETIMER_IF_COMP1));
	}
}

/***************************************************************************//**
 * @brief Starts the letimer
 *
 *
 * @details Toggles sleep mode depending on if  letimer is running via a while loop
 *
 *
 * @note Sleep is turned off if letimer is on and turned on if letimer is off.
 *
 *
 * @param[in] letimer
 *
 *
 *
 *  @param[in] enable
 *  For comparison with letimer status reg
 *
 ******************************************************************************/
void letimer_start(LETIMER_TypeDef *letimer, bool enable){
	if(letimer->STATUS^enable){ //if status and enable are opposites
		LETIMER_Enable(letimer, enable);
		while(letimer->SYNCBUSY);
		if (enable)
			sleep_block_mode(LETIMER_EM); //status = 0, enable =1
		else
			sleep_unblock_mode(LETIMER_EM); //status = 1, enable = 0
	}
}
