/**
 * @file scheduler.c
 * @author Nicholas Wroblewski
 * @date 9/12/2020
 * @brief Sets up all of the scheduler functions
 *
 */
//***********************************************************************************
// Include files
//***********************************************************************************
#include "scheduler.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************
static unsigned int event_scheduled;

//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************
/***************************************************************************//**
 * @brief
 *   Opens the scheduler functionality by resetting the static / private variable event_scheduled to 0
 *
 * @details
 *  Pretty much explained in Brief
 * @note
 *
 *
 *
 ******************************************************************************/
void scheduler_open(void){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	event_scheduled = 0;
	CORE_EXIT_CRITICAL();
}
/***************************************************************************//**
 * @brief
 *   ORs a new event, the input argument,into the existing state of the private (static) variable event_scheduled.
 *
 * @details
 * takes in event and ors it with the event scheduled
  * @note
 * One input
 * @param[in] uint32_t event
 * event to be scheduled
 *
 *
 ******************************************************************************/
void add_scheduled_event(uint32_t event){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	event_scheduled |= event;
	CORE_EXIT_CRITICAL();
}
/***************************************************************************//**
 * @brief
 *   Removes the event, the input argument, from the existing state of the private(static)variable event_scheduled.
 *
 * @details
 * Removes event by ANDing with the not of itself
 * @note
 * One input
 * @param[in] uint32_t event
 * event to be removed
 *
 *
 ******************************************************************************/
void remove_scheduled_event(uint32_t event){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	event_scheduled &=  ~event;
	CORE_EXIT_CRITICAL();
}
/***************************************************************************//**
 * @brief
 *  Returns the current state of the private (static) variable event_scheduled.
 *
 * @details
 * Explained in brief
 * @note
 * Output is event_Scheduled. Returns a value.
 *
 *
 ******************************************************************************/
uint32_t get_scheduled_events(void){
	return event_scheduled;
}
