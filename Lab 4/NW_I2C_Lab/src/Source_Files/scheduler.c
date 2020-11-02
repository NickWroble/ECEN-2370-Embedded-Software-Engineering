/**
 * @file scheduler.c
 *
 * @author Nicholas Wroblewski
 *
 * @date September 27th, 2020
 *
 * @brief Scheduler Functions
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
// Private functions
//***********************************************************************************

/***************************************************************************//**
 * @brief Opens scheduler
 *
 *
 * @details Sets "event_scheduled" to zero
 *
 *
 * @note
 *
 ******************************************************************************/

void scheduler_open(void){
	event_scheduled=0;
}

/***************************************************************************//**
 * @brief Inserts value of new event into "event_scheduled"
 *
 *
 * @details Uses bitwise OR op with input parameter into the static var
 *
 *
 * @note
 *
 *
 * @param[in] event
 * next scheduled event
 *
 ******************************************************************************/

void add_scheduled_event(uint32_t event){
	event_scheduled |= event;
}

/***************************************************************************//**
 * @brief Removes a scheduled event
 *
 *
 * @details Uses bitwise NAND op to remove the input parameter from the private variable
 *
 *
 * @note
 *
 *
 * @param[in] event
 * scheduled event to be removed
 *
 ******************************************************************************/

void remove_scheduled_event(uint32_t event){
	event_scheduled &= ~event;
}

/***************************************************************************//**
 * @brief Returns the state of "event_scheduled"
 *
 *
 * @note Used for EFM_ASSERT
 *
 *
 * @return event_scheduled
 *
 *
 ******************************************************************************/

uint32_t get_scheduled_events(void){
	return event_scheduled;
}
