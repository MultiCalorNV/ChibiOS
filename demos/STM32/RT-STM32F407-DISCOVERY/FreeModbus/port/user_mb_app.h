#ifndef    USER_APP
#define USER_APP

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbutils.h"


/* -----------------------Master Defines -------------------------------------*/
#define M_DISCRETE_INPUT_START        0
#define M_DISCRETE_INPUT_NDISCRETES   500
#define M_COIL_START                  0
#define M_COIL_NCOILS                 500
#define M_REG_INPUT_START             0
#define M_REG_INPUT_NREGS             600
#define M_REG_HOLDING_START           0
#define M_REG_HOLDING_NREGS           600
/* master mode: holding register's all address */
#define          M_HD_RESERVE                     0
/* master mode: input register's all address */
#define          M_IN_RESERVE                     0
/* master mode: coil's all address */
#define          M_CO_RESERVE                     0
/* master mode: discrete's all address */
#define          M_DI_RESERVE                     0

void display_holding(void);

#endif
