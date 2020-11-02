/**
 * @file scheduler.c
 *
 * @author Nicholas Wroblewski
 *
 * @date September 17th, 2020
 *
 * @brief Creates functions in order to run the scheduler
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************

#include "scheduler.h"
#include "em_assert.h"
#include "em_emu.h"
#include "em_core.h"


//***********************************************************************************
// Private Variables
//***********************************************************************************

static signed int event_scheduled;

//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief Enables the scheduler to be open
 *
 *
 * @details Sets created static variable "event_scheduled" to zero
 *
 *
 * @note This function is opening the schedulers functionality by resetting the
 * created static variable
 *
 ******************************************************************************/

void scheduler_open(void){
	event_scheduled=0;
}

/***************************************************************************//**
 * @brief Inputs the value of a new event into "event_scheduled"
 *
 *
 * @details Uses OR operation with input argument into the existing state of the static variable
 *
 *
 * @note An OR operation is required here because event_scheduled is set to zero at the start of the
 * program and the input argument has a value, therefore the OR operation will set event_scheduled to its
 * new value
 *
 *
 * @param[in] event
 * first scheduled event
 *
 ******************************************************************************/

void add_scheduled_event(uint32_t event){
	event_scheduled |= event;
}

/***************************************************************************//**
 * @brief Reverses the operation of add_scheduled_event
 *
 *
 * @details Uses NAND operation in order to remove the input argument from the existing
 * state of the private variable
 *
 *
 * @note A NAND operation is required because at this point "event_scheduled" has a value
 * therefore a NAND operation with a signed input value, "event" will revert "event_scheduled"
 * back to zero
 *
 *
 * @param[in] event
 * scheduled event
 *
 ******************************************************************************/

void remove_scheduled_event(uint32_t event){
	event_scheduled &= ~event;
}

/***************************************************************************//**
 * @brief Returns the state of the static variable "event scheduled"
 *
 *
 * @note Used for EFM_ASSERT checks
 *
 *
 * @return event_scheduled
 * created static/private variable
 *
 ******************************************************************************/

uint32_t get_scheduled_events(void){
	return event_scheduled;
}
