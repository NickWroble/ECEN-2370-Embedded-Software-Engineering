#ifndef	APP_HG
#define	APP_HG

//***********************************************************************************
// Include files
//***********************************************************************************

/* Silicon Labs include statements */
#include "em_cmu.h"
#include "em_assert.h"
#include "em_core.h"

/* The developer's include statements */
#include "cmu.h"
#include "gpio.h"
#include "letimer.h"
#include "brd_config.h"
#include "scheduler.h"
#include "i2c.h"
#include "Si7021.h"
#include "ble.h"

#include <string.h>
#include <stdio.h>


//***********************************************************************************
// defined files
//***********************************************************************************

#define		PWM_PER				2.7		// PWM period in seconds
#define		PWM_ACT_PER			0.15	// PWM active period in seconds

//Application Scheduled Events
#define LETIMER0_COMP0_CB			0x00000001 //0b00001
#define	LETIMER0_COMP1_CB			0x00000002 //0b00010
#define LETIMER0_UF_CB				0x00000004 //0b00100

#define SI7021_READ_CB              0x00000008 //0b01000

#define BOOT_UP_CB					0x00000010 //0b10000
#define SYSTEM_BLOCK_EM 			EM3
#define TXEVENT						0
#define RXEVENT						0
#define BLE_CALLBACK				0x00000020

//***********************************************************************************
// function prototypes
//***********************************************************************************

void app_peripheral_setup(void);
void scheduled_letimer0_uf_cb(void);
void scheduled_letimer0_comp0_cb(void);
void scheduled_letimer0_comp1_cb(void);
void scheduled_si7021_done(void);
void scheduled_boot_up_cb(void);
void schdeduled_ble_done(void);

#endif
