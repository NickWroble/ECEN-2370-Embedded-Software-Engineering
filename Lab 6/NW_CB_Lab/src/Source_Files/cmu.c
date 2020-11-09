/**
 * @file cmu.c
 * @author Nicholas Wroblewski
 * @date 9/12/2020
 * @brief Sets up the proper clock tree.
 *
 */
//***********************************************************************************
// Include files
//***********************************************************************************
#include "cmu.h"

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
 *  Initializes clock to the LETIMER clock tree.
 *
 * @details
 * First is enabling the clock, and then disabling
 * the LFRCO and LFXO. Next, is to route the clock to the LETIMER0
 * clock tree and makes sure the clock is global.
 *
 * @note
 * No inputs or outputs.
 *
 *
 ******************************************************************************/

void cmu_open(void){

		CMU_ClockEnable(cmuClock_HFPER, true);

		// By default, Low Frequency Resistor Capacitor Oscillator, LFRCO, is enabled,
		// Disable the LFRCO oscillator
		CMU_OscillatorEnable(cmuOsc_LFRCO  , false, false);	 // What is the enumeration required for LFRCO?

		// Disable the Low Frequency Crystal Oscillator, LFXO
		CMU_OscillatorEnable(cmuOsc_LFXO, true, true);	// What is the enumeration required for LFXO?

		// No requirement to enable the ULFRCO oscillator.  It is always enabled in EM0-4H
		CMU_ClockSelectSet(cmuClock_LFB,cmuSelect_LFXO);
		// Route LF clock to LETIMER0 clock tree
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);	// What clock tree does the LETIMER0 reside on?

		// Now, you must ensure that the global Low Frequency is enabled
		CMU_ClockEnable(cmuClock_HFLE, true);	//This enumeration is found in the Lab 2 assignment
}

