/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/*****************************************
*	Newlib libc
*
******************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*	Includes ----------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
#include "test.h"
#include "ITM_trace.h"
#include "chprintf.h"

/*	FreeModbus include ******/
#include "mb_m.h"
#include "user_mb_app.h"

/*	Static variables -------------------------------------------------*/
static int Debug_ITMDebug = 0;
ITMStream itm_port;

eMBErrorCode		eStatus;

//****************************************************************************

void Debug_ITMDebugEnable(void){
	volatile unsigned int *ITM_TER      = (volatile unsigned int *)0xE0000E00;
	volatile unsigned int *SCB_DHCSR 		= (volatile unsigned int *)0xE000EDF0;
	volatile unsigned int *DBGMCU_CR 		= (volatile unsigned int *)0xE0042004;

	*DBGMCU_CR |= 0x27; // DBGMCU_CR

if ((*SCB_DHCSR & 1) && (*ITM_TER & 1)) // Enabled?
    Debug_ITMDebug = 1;
}

//****************************************************************************

void Debug_ITMDebugOutputChar(char ch){
	static volatile unsigned int *ITM_STIM0 = (volatile unsigned int *)0xE0000000; // ITM Port 0
	static volatile unsigned int *SCB_DEMCR = (volatile unsigned int *)0xE000EDFC;

	if (Debug_ITMDebug && (*SCB_DEMCR & 0x01000000))
	{
		while(*ITM_STIM0 == 0);
  	*((volatile char *)ITM_STIM0) = ch;
	}
}

//****************************************************************************

void Debug_ITMDebugOutputString(char *Buffer){
	if (Debug_ITMDebug)
		while(*Buffer)
			Debug_ITMDebugOutputChar(*Buffer++);
}

//******************************************************************************

/*static void ledGreenoff(void *p) {
	
  (void)p;
  palClearPad(GPIOA, GPIOA_LED_GREEN);
}*/

/*
 * Red LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("sender");
  while (TRUE) {
    palClearPad(GPIOA, GPIOA_LED_GREEN);
	chThdSleepMilliseconds(300);
    palSetPad(GPIOA, GPIOA_LED_GREEN);
	chThdSleepMilliseconds(300);
  }
}

static THD_WORKING_AREA(wa_freemodbus_thread, 128);
static THD_FUNCTION(modbus_thread, arg) {
  eMBErrorCode			eStatus;
  eMBMasterReqErrCode	errorCode = MB_MRE_NO_ERR;

  (void)arg;
  chRegSetThreadName("modbus_poll");
  
  /*	Init modbus	Master -----------------------------------------------*/
  eStatus = eMBMasterInit(MB_RTU, 3, 19200, MB_PAR_NONE);
  chprintf((BaseSequentialStream *)&itm_port, "eStatus: %s\n", eStatus ? "error": "no'error");
  /************************************************************************/
	
  /*	Enable the Modbus Protocol Stack --------------------------------*/
  eStatus = eMBMasterEnable();
  chprintf((BaseSequentialStream *)&itm_port, "eStatus: %s\n", eStatus ? "error": "no'error");
  /************************************************************************/
  
  while (TRUE) {
    eMBMasterPoll();
	
	chThdSleepMilliseconds(100);
  }
}

static THD_WORKING_AREA(wa_modbusreq_thread, 128);
static THD_FUNCTION(modbusreq_thread, arg) {
  eMBMasterReqErrCode	errorCode = MB_MRE_NO_ERR;
  uint16_t			i;

  (void)arg;
  chRegSetThreadName("modbus_request");

  while (TRUE) {
	errorCode = eMBMasterReqReadHoldingRegister(10, 147, 20, -1);
	
	display_holding();
	
	chThdSleepMilliseconds(850);
  }
}


/*
 * Application entry point.
 */
int main(void) {

  /* Enable TRACE debug -----------------------------------------------*/
  Debug_ITMDebugEnable();
  Debug_ITMDebugOutputString("SWV Enabled\n");
  
  itmObjectInit(&itm_port);

  chprintf((BaseSequentialStream *)&itm_port, "%s", "ChibiOS V3.0\n");

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

	
  /*
   * Creates the blinker thread.
   */
  thread_t *tp = chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  
  /*
   * Creates the modbus poll thread.
   */
  chThdCreateStatic(wa_freemodbus_thread, sizeof(wa_freemodbus_thread), NORMALPRIO + 5,
					modbus_thread, NULL);
					
  /*
   * Creates the modbus request thread.
   */
  chThdCreateStatic(wa_modbusreq_thread, sizeof(wa_modbusreq_thread), NORMALPRIO,
					modbusreq_thread, NULL);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (TRUE) {
    /*if (!palReadPad(GPIOC, GPIOC_BUTTON))
	TestThread(&SD2);*/
    chThdSleepMilliseconds(500);
  }
}
