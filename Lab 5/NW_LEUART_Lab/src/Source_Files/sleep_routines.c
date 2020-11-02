/**
 * @file sleep_routines.c
 *
 * @author Nicholas Wroblewski
 *
 * @date September 23rd, 2020
 *
 * @brief Creates functions for proper sleep routine functionality
 *
 */
/**************************************************************************
* @file sleep_routines.c
***************************************************************************
* @section License
* <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
***************************************************************************
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
* DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
* obligation to support this Software. Silicon Labs is providing the
* Software "AS IS", with no express or implied warranties of any kind,
* including, but not limited to, any implied warranties of merchantability
* or fitness for any particular purpose or warranties against infringement
* of any proprietary rights of a third party.
*
* Silicon Labs will not be liable for any consequential, incidental, or
* special damages, or any other relief, or for any claim by any third party,
* arising from your use of this Software.
*
**************************************************************************/

//***********************************************************************************
// Include files
//***********************************************************************************
#include "sleep_routines.h"

//***********************************************************************************
// Private Variables
//***********************************************************************************
static int lowest_energy_mode[MAX_ENERGY_MODES];

//***********************************************************************************
// Private functions
//***********************************************************************************

/***************************************************************************//**
 * @brief Function to enter sleep
 *
 *
 * @details
 * CORE_DECLARE_IRQ_STATE - saves the current state of the global interrupt enable bit
 * CORE_ENTER_CRITICAL()- Disables all global interrupts
 * CORE_EXIT_CRITICAL()- If the saved value is zero, this will enable global interrupts
 * but if the saved value is 1, global interrupts will remain disabled
 *
 * @note This lowest energy mode the system can enter is EM3
 *
 *
 ******************************************************************************/
void enter_sleep(){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	if (lowest_energy_mode[EM0]>0){
	}
	else if (lowest_energy_mode[EM1]>0){
	}
	else if (lowest_energy_mode[EM2]>0){
		EMU_EnterEM1();
	}
	else if (lowest_energy_mode[EM3]>0){
		EMU_EnterEM2(true);
	}
	else {
		EMU_EnterEM3(true);
	}
	CORE_EXIT_CRITICAL();
	return;
}

/***************************************************************************//**
 * @brief Blocks the system from surpassing EM3
 *
 *
 * @details Uses while loop to make sure the array of "lowest_energy_mode[]" doesn't
 * surpass the 4th spot (EM3)
 *
 *
 * @note Function breaks if EM4 is entered. This function must also handle interrupts with
 *  CORE_DECLARE_IRQ_STATE,CORE_ENTER_CRITICAL(),and CORE_EXIT_CRITICAL() which are described earlier
 *
 *
 * @return k
 * returns the value of MAX_ENERGY_MODES-1, the energy mode the system cannot enter
 *
 ******************************************************************************/
uint32_t  current_block_energy_mode(void){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	int i = 0;
	int k = MAX_ENERGY_MODES-1;
	while(i<MAX_ENERGY_MODES){
		if (lowest_energy_mode[i]){
			k=i;
			break;
		}
		else {
			i=i+1;
		}
	}
	CORE_EXIT_CRITICAL();
	return k;
}

/***************************************************************************//**
 * @brief Blocks the pearl gecko from entering sleep mode while peripheral is active
 *
 *
 * @details Function is utilized by the active peripheral
 *
 *
 * @note Uses assertion to ensure the program does not exceed the lowest_energy_mode array
 * limits. This function must also handle interrupts with CORE_DECLARE_IRQ_STATE,CORE_ENTER_CRITICAL(), and
 * CORE_EXIT_CRITICAL() which are described earlier
 *
 *
 * @param[in] EM
 * Representation of current energy mode
 *
 ******************************************************************************/
void sleep_block_mode(uint32_t EM){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	lowest_energy_mode[EM]++;
	EFM_ASSERT (lowest_energy_mode[EM] < 5);
	CORE_EXIT_CRITICAL();
	return;
}

/***************************************************************************//**
 * @brief Allows for sleep mode to be entered when the peripheral is no longer active
 *
 *
 * @details Releases the processor once peripheral is no longer active
 *
 *
 * @note Uses assertion to make sure proper energy modes are not blocked. This function
 * must also handle interrupts with CORE_DECLARE_IRQ_STATE,CORE_ENTER_CRITICAL(),and CORE_EXIT_CRITICAL()
 * which are described earlier
 *
 *
 * @param[in] EM
 * Representation of current energy mode
 *
 ******************************************************************************/

void sleep_unblock_mode(uint32_t EM){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	lowest_energy_mode[EM]--;
	EFM_ASSERT (lowest_energy_mode[EM] >= 0);
	CORE_EXIT_CRITICAL();
	return;
}

/***************************************************************************//**
 * @brief Initializes the sleep routines static variable "lowest_energy_mode[MAX_ENERGY_MODES]"
 *
 *
 * @details Sets all energy modes to zero with the use of a for loop
 *
 *
 * @note Must handle interrupts with CORE_DECLARE_IRQ_STATE,CORE_ENTER_CRITICAL(), and
 * CORE_EXIT_CRITICAL() which are described earlier
 *
 *
 ******************************************************************************/
void sleep_open(void){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	int i;
	for(i=0; i<MAX_ENERGY_MODES; i++)
		lowest_energy_mode[i]=0;
	CORE_EXIT_CRITICAL();
	return;
}


