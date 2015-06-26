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
 * File: $Id: portserial_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions $
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Static variables ---------------------------------*/
/* modbus Master serial device */
static UARTDriver *serial;

extern ITMStream itm_port;

/* ----------------------- Defines ------------------------------------------*/
/* serial transmit event */
#define EVENT_SERIAL_TRANS_START    (1<<0)

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR(void);
static void prvvUARTRxISR(void);

static void txend1(UARTDriver *uartp);
static void rxerr(UARTDriver *uartp, uartflags_t e);
static void rxchar(UARTDriver *uartp, uint16_t c);

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
        eMBParity eParity)
{
	BOOL            bStatus = FALSE;
    /**
     * set 485 mode receive and transmit control IO
     */
	
	/* set serial configure parameter */
	static UARTConfig uartCfg = {
		txend1,
		NULL,
		NULL,
		rxchar,
		rxerr,
		9600,
		0,
		0,
		0
	};
	
    uartCfg.speed = ulBaudRate;
	serial = &UARTD3;

	/* set serial configure */
	uartStart(serial, &uartCfg);

    return TRUE;
}

void vMBMasterPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
	USART_TypeDef *ureg = serial->usart;
	
    if (xRxEnable)
    {
		/* switch 485 to receive mode */
		
        /* enable RX interrupt */
		ureg->CR1 |= USART_CR1_RXNEIE;
		chprintf((BaseSequentialStream *)&itm_port, "%s\n", "Receive Enable");
    }
    else
    {
        /* disable RX interrupt */
		ureg->CR1 &= ~USART_CR1_RXNEIE;
		chprintf((BaseSequentialStream *)&itm_port, "%s\n", "Receive Disable");
    }
    if (xTxEnable)
    {
		/* switch 485 to transmit mode */
		palSetPad(GPIOG, GPIOG_PIN8);
		
		/* start serial transmit */
		ureg->CR1 |= USART_CR1_TXEIE;
		chprintf((BaseSequentialStream *)&itm_port, "%s\n", "Transmit Enable");
    }
    else
    {
        /* stop serial transmit */
		ureg->CR1 &= ~USART_CR1_TXEIE;
		ureg->CR1 |= USART_CR1_TCIE;
		chprintf((BaseSequentialStream *)&itm_port, "%s\n", "Transmit Disable");
    }
}

void vMBMasterPortClose(void)
{
    uartStop(serial);
}

BOOL xMBMasterPortSerialPutByte(CHAR ucByte)
{
	USART_TypeDef *ureg = serial->usart;
	
	//chprintf((BaseSequentialStream *)&itm_port, "ucByte: %d\n", ucByte);
	
	(ureg->DR) = (uint8_t)ucByte;
	
    return TRUE;
}

BOOL xMBMasterPortSerialGetByte(CHAR * pucByte)
{	
	USART_TypeDef *ureg = serial->usart;
	
    *pucByte = (uint8_t)(ureg->DR);
	
	//chprintf((BaseSequentialStream *)&itm_port, "*pucByte: %d\n", *pucByte);
	
    return TRUE;
}

/*
	* This callback is invoked when a transmission buffer has been completely
 * read by the driver.
 */
static void txend1(UARTDriver *uartp){
	(void)uartp;
	
	//chprintf((BaseSequentialStream *)&itm_port, "%s\n", "TxEmpty...");
	//chSysLockFromISR();
	pxMBMasterFrameCBTransmitterEmpty();
	//chSysUnlockFromISR();

}

/*
 * This callback is invoked when a character is received but the application
 * was not ready to receive it, the character is passed as parameter.
 */
static void rxchar(UARTDriver *uartp, uint16_t c){
	(void)uartp;
	(void)c;
	
	//chprintf((BaseSequentialStream *)&itm_port, "%s\n", "Rx Not Empty...");
	//chSysLockFromISR();
	pxMBMasterFrameCBByteReceived();
	//chSysUnlockFromISR();
	
}

/*
 * This callback is invoked on a receive error, the errors mask is passed
 * as parameter.
 */
static void rxerr(UARTDriver *uartp, uartflags_t e){
	(void)uartp;
	(void)e;
	
	//chSysLockFromISR(); 
	if (e & USART_SR_PE) {
	  chprintf((BaseSequentialStream *)&itm_port, "%s\n", "parity err");
	} else if (e & USART_SR_FE) {
	  chprintf((BaseSequentialStream *)&itm_port, "%s\n", "framing err");
	} if (e & USART_SR_NE) {
	  chprintf((BaseSequentialStream *)&itm_port, "%s\n", "noise err");
	} if (e & USART_SR_ORE) {
	  chprintf((BaseSequentialStream *)&itm_port, "%s\n", "overrun err");
	} if (e & USART_SR_IDLE) {
	  chprintf((BaseSequentialStream *)&itm_port, "%s\n", "idle line err");
	} else {
	  chprintf((BaseSequentialStream *)&itm_port, "%s\n", "uart rx err");
	}
	//chSysUnlockFromISR();

}

/* 
 * Create an interrupt handler for the transmit rxbuffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
/*void prvvUARTTxReadyISR(void)
{
    pxMBMasterFrameCBTransmitterEmpty();
}*/

/* 
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
/*void prvvUARTRxISR(void)
{
    pxMBMasterFrameCBByteReceived();
}*/


#endif
