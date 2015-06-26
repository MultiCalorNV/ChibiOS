/*
 * FreeModbus Libary: user callback functions and buffer define in master mode
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
 * File: $Id: user_mb_app_m.c,v 1.60 2013/11/23 11:49:05 Armink $
 */
#include "user_mb_app.h"

extern ITMStream itm_port;


/*-----------------------Master mode use these variables----------------------*/
#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
//Master mode:DiscreteInputs variables
USHORT   usMDiscInStart                             = M_DISCRETE_INPUT_START;
#if      M_DISCRETE_INPUT_NDISCRETES%8
UCHAR    ucMDiscInBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_DISCRETE_INPUT_NDISCRETES/8+1];
#else
UCHAR    ucMDiscInBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_DISCRETE_INPUT_NDISCRETES/8];
#endif
//Master mode:Coils variables
static uint16_t	xtCoilNRegs;
static uint16_t	xtCoilRegIndex;
static USHORT   usMCoilStart                               = M_COIL_START;
#if      M_COIL_NCOILS%8
static UCHAR    ucMCoilBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_COIL_NCOILS/8+1];
#else
static UCHAR    ucMCoilBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_COIL_NCOILS/8];
#endif
//Master mode:InputRegister variables
static uint16_t	xtinputNRegs;
static uint16_t	xtinputRegIndex;
static USHORT   usMRegInStart                              = M_REG_INPUT_START;
static USHORT   usMRegInBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_INPUT_NREGS];
//Master mode:HoldingRegister variables
static uint16_t	xtholdingNRegs;
static uint16_t	xtholdingRegIndex;
static USHORT   usMRegHoldStart                            = M_REG_HOLDING_START;
static USHORT   usMRegHoldBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_HOLDING_NREGS];

void display_holding(void){
	uint16_t			i;
	
	for(i = 0; i <= xtholdingNRegs; i++){
		chprintf((BaseSequentialStream *)&itm_port, "usMRegHoldBuf[%d]: %d\n", xtholdingRegIndex + i, usMRegHoldBuf[ucMBMasterGetDestAddress() - 1][xtholdingRegIndex + i]);
	}
	for(i = 0; i <= xtinputNRegs; i++){
		chprintf((BaseSequentialStream *)&itm_port, "usMRegInBuf[%d]: %d\n", xtinputRegIndex + i, usMRegInBuf[ucMBMasterGetDestAddress() - 1][xtinputRegIndex + i]);
	}
	for(i = 0; i <= xtCoilNRegs; i++){
		chprintf((BaseSequentialStream *)&itm_port, "ucMCoilBuf[%d]: %d\n", xtCoilRegIndex + i, ucMCoilBuf[ucMBMasterGetDestAddress() - 1][xtCoilRegIndex + i]);
	}
}

/**
 * Modbus master input register callback function.
 *
 * @param pucRegBuffer input register buffer
 * @param usAddress input register address
 * @param usNRegs input register number
 *
 * @return result
 */
eMBErrorCode eMBMasterRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          iRegIndex;
    USHORT *        pusRegInputBuf;
    USHORT          REG_INPUT_START;
    USHORT          REG_INPUT_NREGS;
    USHORT          usRegInStart;

    xtinputNRegs = usNRegs;

    pusRegInputBuf = usMRegInBuf[ucMBMasterGetDestAddress() - 1];
    REG_INPUT_START = M_REG_INPUT_START;
    REG_INPUT_NREGS = M_REG_INPUT_NREGS;
    usRegInStart = usMRegInStart;

    /* it already plus one in modbus function method. */
    usAddress--;

    if ((usAddress >= REG_INPUT_START)
            && (usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS))
    {
        iRegIndex = usAddress - usRegInStart;
		xtinputRegIndex = iRegIndex;
        while (usNRegs > 0)
        {
            pusRegInputBuf[iRegIndex] = *pucRegBuffer++ << 8;
            pusRegInputBuf[iRegIndex] |= *pucRegBuffer++;
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

/**
 * Modbus master holding register callback function.
 *
 * @param pucRegBuffer holding register buffer
 * @param usAddress holding register address
 * @param usNRegs holding register number
 * @param eMode read or write
 *
 * @return result
 */
eMBErrorCode
eMBMasterRegHoldingCB(uint8_t* pucRegBuffer, uint16_t usAddress, uint16_t usNRegs, eMBRegisterMode eMode)
{
    eMBErrorCode    eStatus = MB_ENOERR;
    uint16_t          iRegIndex;
    uint16_t *        pusRegHoldingBuf;
    uint16_t          REG_HOLDING_START;
    uint16_t          REG_HOLDING_NREGS;
    uint16_t          usRegHoldStart;
	
	xtholdingNRegs = usNRegs;

    pusRegHoldingBuf = usMRegHoldBuf[ucMBMasterGetDestAddress() - 1];
    REG_HOLDING_START = M_REG_HOLDING_START;
    REG_HOLDING_NREGS = M_REG_HOLDING_NREGS;
    usRegHoldStart = usMRegHoldStart;
    /* if mode is read, the master will write the received date to buffer. */
    eMode = MB_REG_WRITE;

    /* it already plus one in modbus function method. */
    usAddress--;
	//chprintf((BaseSequentialStream *)&itm_port,"usAddress: %d\n", usAddress);

    if ((usAddress >= REG_HOLDING_START)
            && (usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS))
    {
        iRegIndex = usAddress - usRegHoldStart;
		xtholdingRegIndex = iRegIndex;
        switch (eMode)
        {
        /* read current register values from the protocol stack. */
        case MB_REG_READ:
            while (usNRegs > 0)
            {
                *pucRegBuffer++ = (uint8_t) (pusRegHoldingBuf[iRegIndex] >> 8);
                *pucRegBuffer++ = (uint8_t) (pusRegHoldingBuf[iRegIndex] & 0xFF);
                iRegIndex++;
                usNRegs--;
            }
            break;
        /* write current register values with new values from the protocol stack. */
        case MB_REG_WRITE:
            while (usNRegs > 0)
            {
                pusRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                pusRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
            break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

/**
 * Modbus master coils callback function.
 *
 * @param pucRegBuffer coils buffer
 * @param usAddress coils address
 * @param usNCoils coils number
 * @param eMode read or write
 *
 * @return result
 */
eMBErrorCode eMBMasterRegCoilsCB(UCHAR * pucRegBuffer, USHORT usAddress,
        USHORT usNCoils, eMBRegisterMode eMode)
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          iRegIndex , iRegBitIndex , iNReg;
    UCHAR *         pucCoilBuf;
    USHORT          COIL_START;
    USHORT          COIL_NCOILS;
    USHORT          usCoilStart;
    iNReg =  		usNCoils / 8 + 1;

    xtCoilNRegs = iNReg;
	
	chprintf( (BaseSequentialStream *)&itm_port, "xtCoilNRegs[%d]: %d\n", xtCoilNRegs );

    pucCoilBuf = ucMCoilBuf[ucMBMasterGetDestAddress() - 1];
    COIL_START = M_COIL_START;
    COIL_NCOILS = M_COIL_NCOILS;
    usCoilStart = usMCoilStart;

    /* if mode is read,the master will write the received date to buffer. */
    eMode = MB_REG_WRITE;

    /* it already plus one in modbus function method. */
    usAddress--;

    if ((usAddress >= COIL_START)
            && (usAddress + usNCoils <= COIL_START + COIL_NCOILS))
    {
        iRegIndex = (uint16_t) (usAddress - usCoilStart) / 8;
		xtCoilRegIndex = iRegIndex;
        iRegBitIndex = (uint16_t) (usAddress - usCoilStart) % 8;
        switch (eMode)
        {
         /* read current coil values from the protocol stack. */
        case MB_REG_READ:
            while (iNReg > 0)
            {
                *pucRegBuffer++ = xMBUtilGetBits(&pucCoilBuf[iRegIndex++],
                        iRegBitIndex, 8);
                iNReg--;
            }
            pucRegBuffer--;
            /* last coils */
            usNCoils = usNCoils % 8;
            /* filling zero to high bit */
            *pucRegBuffer = *pucRegBuffer << (8 - usNCoils);
            *pucRegBuffer = *pucRegBuffer >> (8 - usNCoils);
            break;

        /* write current coil values with new values from the protocol stack. */
        case MB_REG_WRITE:
            while (iNReg > 1)
            {
                xMBUtilSetBits(&pucCoilBuf[iRegIndex++], iRegBitIndex, 8,
                        *pucRegBuffer++);
                iNReg--;
            }
            /* last coils */
            usNCoils = usNCoils % 8;
            /* xMBUtilSetBits has bug when ucNBits is zero */
            if (usNCoils != 0)
            {
                xMBUtilSetBits(&pucCoilBuf[iRegIndex++], iRegBitIndex, usNCoils,
                        *pucRegBuffer++);
            }
            break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

/**
 * Modbus master discrete callback function.
 *
 * @param pucRegBuffer discrete buffer
 * @param usAddress discrete address
 * @param usNDiscrete discrete number
 *
 * @return result
 */
eMBErrorCode eMBMasterRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          iRegIndex , iRegBitIndex , iNReg;
    UCHAR *         pucDiscreteInputBuf;
    USHORT          DISCRETE_INPUT_START;
    USHORT          DISCRETE_INPUT_NDISCRETES;
    USHORT          usDiscreteInputStart;
    iNReg =  usNDiscrete / 8 + 1;

    pucDiscreteInputBuf = ucMDiscInBuf[ucMBMasterGetDestAddress() - 1];
    DISCRETE_INPUT_START = M_DISCRETE_INPUT_START;
    DISCRETE_INPUT_NDISCRETES = M_DISCRETE_INPUT_NDISCRETES;
    usDiscreteInputStart = usMDiscInStart;

    /* it already plus one in modbus function method. */
    usAddress--;

    if ((usAddress >= DISCRETE_INPUT_START)
            && (usAddress + usNDiscrete    <= DISCRETE_INPUT_START + DISCRETE_INPUT_NDISCRETES))
    {
        iRegIndex = (USHORT) (usAddress - usDiscreteInputStart) / 8;
        iRegBitIndex = (USHORT) (usAddress - usDiscreteInputStart) % 8;

        /* write current discrete values with new values from the protocol stack. */
        while (iNReg > 1)
        {
            xMBUtilSetBits(&pucDiscreteInputBuf[iRegIndex++], iRegBitIndex, 8,
                    *pucRegBuffer++);
            iNReg--;
        }
        /* last discrete */
        usNDiscrete = usNDiscrete % 8;
        /* xMBUtilSetBits has bug when ucNBits is zero */
        if (usNDiscrete != 0)
        {
            xMBUtilSetBits(&pucDiscreteInputBuf[iRegIndex++], iRegBitIndex,
                    usNDiscrete, *pucRegBuffer++);
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}
#endif
