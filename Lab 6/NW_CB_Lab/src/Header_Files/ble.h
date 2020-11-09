/*
 * ble.h
 *
 *  Created on:
 *      Author:
 */
//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef	BLE_GUARD_H
#define	BLE_GUARD_H

//** Standard Libraries
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
// Driver functions
#include "leuart.h"
#include "gpio.h"

#define CIRC_TEST true
#define CIRC_OPER false
//***********************************************************************************
// defined files
//***********************************************************************************

#define HM10_LEUART0		LEUART0
#define HM10_BAUDRATE		9600
#define	HM10_DATABITS		leuartDatabits8
#define HM10_ENABLE			leuartEnableTx
#define HM10_PARITY			leuartNoParity
#define HM10_REFFREQ		0					// use reference clock
#define HM10_STOPBITS		leuartStopbits1

#define LEUART0_TX_ROUTE	LEUART_ROUTELOC0_TXLOC_LOC18 	// Route to ...
#define LEUART0_RX_ROUTE	LEUART_ROUTELOC0_RXLOC_LOC18  	// Route to ...


//***********************************************************************************
// global variables
//***********************************************************************************
//***********************************************************************************
// function prototypes
//***********************************************************************************
void ble_open(uint32_t tx_event, uint32_t rx_event);
void ble_write(char *string);

bool ble_test(char *mod_name);
void circular_buff_test(void);
bool ble_circ_pop(bool test);
#endif
