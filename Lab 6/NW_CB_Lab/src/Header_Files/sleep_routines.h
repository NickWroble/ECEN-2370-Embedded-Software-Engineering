/*
 * sleep_routines.h
 *
 *  Created on: Sep 24, 2020
 *      Author: Nicholas Wroblewski
 */

#ifndef SRC_HEADER_FILES_SLEEP_ROUTINES_H_
#define SRC_HEADER_FILES_SLEEP_ROUTINES_H_

/* System include statements */


/* Silicon Labs include statements */
#include "em_emu.h"
#include "em_int.h"
#include "em_core.h"
#include "em_assert.h"

/* The developer's include statements */

//***********************************************************************************
// defined files
//***********************************************************************************
#define		EM0 	0
#define 	EM1 	1
#define 	EM2 	2
#define 	EM3 	3
#define 	EM4 	4
#define 	MAX_ENERGY_MODES 	5
//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void sleep_open(void);
// Initialize the sleep_routines static / private variable, lowest_energy_mode[]

void sleep_block_mode(uint32_t EM);
//Utilized by a peripheral to prevent the Pearl Gecko going into that sleep mode while the peripheral is active.

void sleep_unblock_mode(uint32_t EM);
// Utilized to release the processor from going into a sleep mode with a peripheral that is no longer active.

void enter_sleep(void);
//Function to enter sleep

uint32_t current_block_energy_mode(void);
//Function that returns which energy mode that the current system cannot enter.
#endif /* SRC_HEADER_FILES_SLEEP_ROUTINES_H_ */
