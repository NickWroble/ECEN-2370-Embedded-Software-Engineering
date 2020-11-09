/**
 * @file sleep_routines.c
 * @date Sep 24, 2020
 * @author Nicholas Wroblewski
 * @brief Contains all the sleep routine functions
 *
***************************************************************************
 * ***************************************************************************
 * * @section License
 * * <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 * ****************************************************************************
 * * Permission is granted to anyone to use this software for any purpose,
 * * including commercial applications, and to alter it and redistribute it
 * * freely, subject to the following restrictions:*
 * * 1. The origin of this software must not be misrepresented; you must not
 * *    claim that you wrote the original software.
 * * 2. Altered source versions must be plainly marked as such, and must not be
 * *    misrepresented as beingthe original software.
 * * 3. This notice may not be removed or altered from any source distribution.
 * ** DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * * obligation to support this Software. Silicon Labs is providing the
 * * Software "AS IS", with no express or implied warranties of any kind,
 * * including, but not limited to, any implied warranties of merchantability
 * * or fitness for any particular purpose or warranties against infringement
 * * of any proprietary rights of a third party.
 * ** Silicon Labs will not be liable for any consequential, incidental, or
 * * special damages, or any other relief, or for any claim by any third party,
 * * arising from your use of this Software.
 ***************************************************************************/


//***********************************************************************************
// Include files
//***********************************************************************************
#include "sleep_routines.h"


//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Static / Private Variables
//***********************************************************************************
static int lowest_energy_modes[MAX_ENERGY_MODES];

//***********************************************************************************
// Private functions
//***********************************************************************************

//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *Initialize the sleep_routines static / private variable, lowest_energy_mode[]
 * @details
 * lowest energy mode is set 0
 * @note
 *No inputs or outputs
 *
 ******************************************************************************/
void sleep_open(void){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	for(int i = 0; i<MAX_ENERGY_MODES; i++) lowest_energy_modes[i] = 0;
	CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 *Utilized by a peripheral to prevent the Pearl Gecko going into that sleep mode while the peripheral is active.
 * @details
 *  Takes in a energy mode and will unblock it by decrementing it down to 0
 * @note
 * one input
 *@param[in] uint32_t EM
 * Energy mode you want unblocked
 *
 ******************************************************************************/
void sleep_block_mode(uint32_t EM){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	EFM_ASSERT (lowest_energy_modes[EM] < 5);
	lowest_energy_modes[EM]++;
	CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 *Utilized to release the processor from going into a sleep mode with a peripheral that is no longer active.
 * @details
 * takes in an energy mode, makes sure that mode is greater than or equal to zero. After which it unblocks the energy  mode by subtracting one.
 *
 * @note
 * one input
 *@param[in] uint32_t EM
 * Which energy mode is it
 *
 *
 ******************************************************************************/
void sleep_unblock_mode(uint32_t EM){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	EFM_ASSERT (lowest_energy_modes[EM] >= 0);
	lowest_energy_modes[EM]--;
	CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 * Function to enter sleep
 * @details
 *  reads what energy mode it's at and then enters sleep based off of that.
 * @note
 *if return is before core exit critical, then interrupts won't occur
 *
 ******************************************************************************/
void enter_sleep(void){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	if(lowest_energy_modes[EM0]>0) {
	}
	else if(lowest_energy_modes[EM1]>0){}
	else if(lowest_energy_modes[EM2]>0){
		EMU_EnterEM1();

	}
	else if(lowest_energy_modes[EM3]>0){
			EMU_EnterEM2(true);

		}
	else{
	EMU_EnterEM3(true);
	}
	CORE_EXIT_CRITICAL();
	return;
}

/***************************************************************************//**
 * @brief
 *Function that returns which energy mode that the current system cannot enter.
 * @details
 * if an energy mode is blocked, then its value will not be zero. So the while loop checks if any of the energy modes are blocked. If so it returns the energy not. If none are blocked it returns EM4
 * @note
 * default is EM4
 *
 ******************************************************************************/
uint32_t current_block_energy_mode(void){
	int i =0;
	while(i < MAX_ENERGY_MODES){
		if(lowest_energy_modes[i] != 0){
			return i;
		}
		i+=1;
	}
	return (MAX_ENERGY_MODES-1);
}
