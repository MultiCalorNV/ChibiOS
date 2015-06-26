/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: porttimer_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbport.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Variables ----------------------------------------*/
static USHORT usT35TimeOut50us;
static virtual_timer_t vt35;
static virtual_timer_t vtout;
static virtual_timer_t vtdelay;

extern ITMStream itm_port;

/* ----------------------- static functions ---------------------------------*/
static void timer_timeout_ind(void* parameter);
static void prvvTIMERExpiredISR(void);

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortTimersInit(USHORT usTimeOut50us)
{
    /* backup T35 ticks */
    usT35TimeOut50us = usTimeOut50us;
	palClearPad(GPIOC, GPIOC_PIN9);

    return TRUE;
}

void vMBMasterPortTimersT35Enable()
{
	//chprintf((BaseSequentialStream *)&itm_port, "%s\n", "T35 Enable");
    /* Set current timer mode, don't change it.*/
    vMBMasterSetCurTimerMode(MB_TMODE_T35);
	palSetPad(GPIOC, GPIOC_PIN9);

	chSysLockFromISR();
	chVTResetI(&vt35);
	chVTSetI(&vt35, US2ST((uint32_t)2000), timer_timeout_ind, NULL);
	chSysUnlockFromISR();
}

void vMBMasterPortTimersConvertDelayEnable()
{
    /* Set current timer mode, don't change it.*/
    vMBMasterSetCurTimerMode(MB_TMODE_CONVERT_DELAY);

    chSysLockFromISR();
	chVTResetI(&vtdelay);
	chVTSetI(&vtdelay, MS2ST((uint32_t)MB_MASTER_DELAY_MS_CONVERT), timer_timeout_ind, NULL);
	chSysUnlockFromISR();
}

void vMBMasterPortTimersRespondTimeoutEnable()
{
	//chprintf((BaseSequentialStream *)&itm_port, "%s\n", "TimeOut Enable");
	//palSetPad(GPIOC, GPIOC_PIN10);
    /* Set current timer mode, don't change it.*/
    chSysLockFromISR();
	chVTResetI(&vtout);
	chVTSetI(&vtout, MS2ST((uint32_t)MB_MASTER_TIMEOUT_MS_RESPOND), timer_timeout_ind, NULL);
	chSysUnlockFromISR();
}

void vMBMasterPortTimersDisable()
{
	palClearPad(GPIOC, GPIOC_PIN9);
	//palClearPad(GPIOC, GPIOC_PIN10);
	chSysLockFromISR();
    chVTResetI(&vt35);
	chVTResetI(&vtdelay);
	chVTResetI(&vtout);
	chSysUnlockFromISR();
}

void prvvTIMERExpiredISR(void)
{
    (void) pxMBMasterPortCBTimerExpired();
}

static void timer_timeout_ind(void* parameter)
{
    prvvTIMERExpiredISR();
}

#endif
