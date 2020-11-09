//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef	APP_HG
#define	APP_HG

/* System include statements */
#include <string.h>
#include <stdio.h>
#include <math.h>
/* Silicon Labs include statements */
#include "em_cmu.h"
#include "em_assert.h"

/* The developer's include statements */
#include "cmu.h"
#include "gpio.h"
#include "letimer.h"
#include "brd_config.h"
#include "sleep_routines.h"
#include "SI7021.h"
#include "i2c.h"
#include "ble.h"
#include "HW_Delay.h"
#include "leuart.h"



//***********************************************************************************
// defined files
//***********************************************************************************
#define		PWM_PER				2.7		// PWM period in seconds
#define		PWM_ACT_PER			0.15	// PWM active period in seconds

#define	LETIMER0_COMP0_CB		0x00000001
#define	LETIMER0_COMP1_CB		0x00000002
#define LETIMER0_UF_CB 			0x00000004

#define SI7021_READ_CB			0x00000008

#define BOOT_UP_CB 				0x00000010
#define TX_DONE_EVENT 			0x00000020
#define RX_DONE_EVENT				0x00000040

#define SYSTEM_BLOCK_EM 		EM3

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void app_peripheral_setup(void);
void scheduled_letimer0_uf_cb(void);
void si7021_temp_done(void);
void scheduled_letimer0_comp0_cb(void);
void scheduled_letimer0_comp1_cb(void);
void scheduled_boot_up_cb(void);
void scheduled_tx_done_event(void);


#endif
