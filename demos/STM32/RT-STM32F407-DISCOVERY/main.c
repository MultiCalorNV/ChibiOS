/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
#include <time.h>

/*	Includes ----------------------------------------------------------*/
#include "ch.h"
#include "hal.h"
#include "test.h"
#include "ArrayList.h"
#include "ITM_trace.h"
#include "chprintf.h"
#include "chsem.h"
#include "lcfg_static.h"

/*	FreeModbus include ******/
#include "mb_m.h"

/*	LwIP include  ******/
#include "lwipthread.h"

#include "web/web.h"

/*	Static variables -------------------------------------------------*/
static int Debug_ITMDebug = 0;
ITMStream itm_port;

DeviceList list_RTC;
DeviceList list_mbDevices;
static semaphore_t mbPoll;

unsigned char 	*value;
size_t			size;

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

/*
 * LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg){
	
  (void)arg;
  chRegSetThreadName("blinker");
  while (TRUE)
  {
	chprintf((BaseSequentialStream *)&itm_port, "%s", "Thread blinker.\n");
    palClearPad(GPIOG, GPIOG_PIN6);
    chThdSleepMilliseconds(500);
    palSetPad(GPIOG, GPIOG_PIN6);
    chThdSleepMilliseconds(500);
  }
}

static THD_WORKING_AREA(wa_freemodbus_thread, 512);
static THD_FUNCTION(modbus_thread, arg) {
  eMBErrorCode			eStatus;
  eMBMasterReqErrCode	errorCode = MB_MRE_NO_ERR;

  (void)arg;
  chRegSetThreadName("modbus_poll");
  
  /*	Init modbus	Master -----------------------------------------------*/
  eStatus = eMBMasterInit(MB_RTU, 3, 19200, MB_PAR_NONE);
  chprintf((BaseSequentialStream *)&itm_port, "eStatus: %s\n", eStatus ? "error": "no'error");
  /************************************************************************/
	
  /*	Enable the Modbus Protocol Stack ---------------------------------*/
  eStatus = eMBMasterEnable();
  chprintf((BaseSequentialStream *)&itm_port, "eStatus: %s\n", eStatus ? "error": "no'error");
  /************************************************************************/
  
  while (TRUE) {
	eMBMasterPoll();
	
	chSemSignal(&mbPoll);
	chThdSleepMilliseconds(100);
  }
}

static THD_WORKING_AREA(wa_modbusreq_thread, 512);
static THD_FUNCTION(modbusreq_thread, arg) {
  eMBMasterReqErrCode	errorCode = MB_MRE_NO_ERR;
  //uint16_t			i;
  uint16_t 			counter = 1;

  (void)arg;
  chRegSetThreadName("modbus_request");

  while (TRUE) {
    chThdSleepMilliseconds(720);
    chSemWait(&mbPoll);
	counter++;
	switch(counter % 2){
		case 0:
			//errorCode = eMBMasterReqReadInputRegister(10, 497, 6, -1);
			errorCode = eMBMasterReqReadHoldingRegister(10, 147, 20, -1);
				
			break;
		case 1:
			//errorCode = eMBMasterReqReadHoldingRegister(10, 147, 6, -1);
			//errorCode = eMBMasterReqReadInputRegister(10, 497, 15, -1);
			errorCode = eMBMasterReqReadCoils(11, 79, 25, -1);
				
			break;
		default:
			break;
	}
	
	display_holding();
	
	chThdSleepMilliseconds(520);
  }
}

enum lcfg_status example_visitor(const char *key, void *data, size_t len, void *user_data){
	int		i;
	char 	c;
	
	chprintf((BaseSequentialStream *)&itm_port, "key: %s ", key);
	for(i = 0; i < len; i++){
		c = *((const char *)(data + i));
		chprintf((BaseSequentialStream *)&itm_port, "value: %c\n", isprint(c) ? c : '.');
	}
	
	return lcfg_status_ok;
}

static void setup_Config(void){
	struct lcfg *c = lcfg_new("modbus.cfg");
  
    if(lcfg_parse(c) != lcfg_status_ok){
		chprintf((BaseSequentialStream *)&itm_port, "lcfg error: %s\n", lcfg_error_get(c));
    }else{
		lcfg_accept(c, example_visitor, 0);
    }
	
    lcfg_delete(c);
}

/*
 * Application entry point.
 */
int main(void)
{
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
  
  //setup_Config();
  
  /*
   * List of modbus slave devices - Web
   *
   */
  init(&list_RTC);
  
  /*
   * List of requests to modbus devices on bus
   *
   */
  init(&list_mbDevices);
  
  /*
   * Synchronize modbus
   */
  chSemObjectInit(&mbPoll, 1);

  /*
   * Activates the serial driver 6 using the driver default configuration.
   */
  sdStart(&SD6, NULL);

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  
  /*
   * Creates the modbus poll thread.
   */
  chThdCreateStatic(wa_freemodbus_thread, sizeof(wa_freemodbus_thread), NORMALPRIO,
					modbus_thread, NULL);
					
  /*
   * Creates the modbus request thread.
   */
  chThdCreateStatic(wa_modbusreq_thread, sizeof(wa_modbusreq_thread), NORMALPRIO,
					modbusreq_thread, NULL);

  /*
   * Creates the LWIP threads (it changes priority internally).
   */
  chThdCreateStatic(wa_lwip_thread, LWIP_THREAD_STACK_SIZE, HIGHPRIO + 1,
					lwip_thread, NULL);
	
  /*
   * Creates the HTTP thread (it changes priority internally).
   */
  chThdCreateStatic(wa_http_server, sizeof(wa_http_server), NORMALPRIO + 1,
					http_server, NULL);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (TRUE)
  {
	
    if (palReadPad(GPIOA, GPIOA_BUTTON) == 1)
	TestThread(&SD6);
    chThdSleepMilliseconds(1000);
  }
}
