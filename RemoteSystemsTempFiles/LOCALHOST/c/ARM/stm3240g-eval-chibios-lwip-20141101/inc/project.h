/**************************************************************************
*  Copyright (c) 2013-2014 by Michael Fischer.
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*  
*  1. Redistributions of source code must retain the above copyright 
*     notice, this list of conditions and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the 
*     documentation and/or other materials provided with the distribution.
*  3. Neither the name of the author nor the names of its contributors may 
*     be used to endorse or promote products derived from this software 
*     without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
*  THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
*  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
*  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
*  SUCH DAMAGE.
*
***************************************************************************
*  History:
*
*  19.05.2013  mifi  First Version
*  19.08.2013  mifi  Some rework and cleanup
*  18.04.2014  mifi  Support STM32F2x7 and STM32F4x7
**************************************************************************/
#if !defined(__PROJECT_H__)
#define __PROJECT_H__

/**************************************************************************
*  Includes
**************************************************************************/
#include <stdint.h>
#include "ch.h"

#ifdef STM32F40_41xxx
#include "stm32f4xx.h"
#include "stm32f4x7_eth.h"
#endif

#ifdef STM32F2XX
#include "stm32f2xx.h"
#include "stm32f2x7_eth.h"
#endif

/**************************************************************************
*  Global Definitions
**************************************************************************/

/*-----------------------------------------------------------------------*/
/*    Ticks per second                                                   */
/*-----------------------------------------------------------------------*/

#define TICKS_PER_SECOND  1000

/*-----------------------------------------------------------------------*/
/*    Task priorities and delays                                         */
/*-----------------------------------------------------------------------*/

/*
 * Define priority and stack size of the tasks.
 * In case of a ChibiOS/RT project, valid priorities are in the range 
 * from LOWPRIO (2) ... HIGHPRIO (127).
 */

#define TASK_START_PRIORITY      110
#define TASK_START_PRIORITY_IDLE 2 
#define TASK_START_STK_SIZE      512

/*
 * The priority of the IP_RX task must be the highest one of
 * all IP tasks. Than comes the IP task and all IP application.
 */
#define TASK_IP_RX_PRIORITY      100 
#define TASK_IP_RX_STK_SIZE      512

#define TASK_IP_PRIORITY         99
#define TASK_IP_STK_SIZE         768

/**************************************************************/

#define TASK_IP_IPERF_PRIORITY   90
#define TASK_IP_IPERF_STK_SIZE   512

#define TASK_IP_HTTP_PRIORITY    88
#define TASK_IP_HTTP_STK_SIZE    768

#define TASK_TERM_PRIORITY       80
#define TASK_TERM_STK_SIZE       384

#define TASK_LED_PRIORITY        67
#define TASK_LED_STK_SIZE        256


/*
 * Define delay times of the tasks
 */
#define TASK_LED_DELAY_MS        500


/*-----------------------------------------------------------------------*/
/*    Other IP stuff                                                     */
/*-----------------------------------------------------------------------*/
#define IP_DEFAULT_ADDRESS    "192.168.1.200"
#define IP_DEFAULT_NETMASK    "255.255.255.0"
#define IP_DEFAULT_GATEWAY    "0.0.0.0"

#define IP_MAC_ADDRESS_0      0x00
#define IP_MAC_ADDRESS_1      0x11
#define IP_MAC_ADDRESS_2      0x22
#define IP_MAC_ADDRESS_3      0x33
#define IP_MAC_ADDRESS_4      0x44
#define IP_MAC_ADDRESS_5      0x55


/**************************************************************************
*  Macro Definitions
**************************************************************************/

#define MS_2_TICKS(_x) (((uint32_t)(_x) * TICKS_PER_SECOND) / 1000)
#define TICKS_2_MS(_x) ((1000 / TICKS_PER_SECOND) * (_x))

/**************************************************************************
*  Funtions Definitions
**************************************************************************/
void TimeDly (uint32_t Ticks);
 
#endif /* !__PROJECT_H__ */
/*** EOF ***/
