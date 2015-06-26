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
/*
* **** This file incorporates work covered by the following copyright and ****
* **** permission notice:                                                 ****
*
*  Copyright (c) 2009 by Michael Fischer. All rights reserved.
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
****************************************************************************
*  History:
*
*  28.03.09  mifi       First Version, based on the original syscall.c from
*                       newlib version 1.17.0
*  17.08.09  gdisirio   Modified the file for use under ChibiOS/RT
*  15.11.09  gdisirio   Added read and write handling
****************************************************************************/
#include "halconf.h"

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/unistd.h>

#include "ch.h"
#if HAL_USE_RTC || defined(STDOUT_SD) || defined(STDIN_SD)
#include "hal.h"
#endif
#include "Verventa-master.h"

#include "rtc.h"

#include "ITM_trace.h"
#include "chprintf.h"

extern ITMStream itm_port;

#define EOF	'\0'

/***************************************************************************/

int _open_r(void *reent, const char *file, int flags, int mode)
{
	//chprintf((BaseSequentialStream *)&itm_port, "file: %s\n", file);
	
	char	i;
	int 	fd = -1;
	
	for(i = 0; i < NUMBER_OF_FILES; i++)
	{
		if(strcmp(lcfg_table[i].name, file) == 0) return fd = i;
	}
	
	(void)reent; (void)flags; (void)mode;
	return fd;
}

/***************************************************************************/

int _read_r(struct _reent *r, int file, char *ptr, int len)
{
  char		i = 0;
  uint8_t	buffersize = len;
  //char	ascii;
	
  (void)r;
#if defined(STDIN_SD)
  if (!len || (file != 0)) {
    __errno_r(r) = EINVAL;
    return -1;
  }
  len = sdRead(&STDIN_SD, (uint8_t *)ptr, (size_t)len);
  return len;
#else
  //chprintf((BaseSequentialStream *)&itm_port, "file descript: %i\n", file);
  
  if (!len || (file == -1)) {
    __errno_r(r) = EBADF;
    return -1;
  }else {
		do{
			if(lcfg_table[file].data[i] != EOF){
				*ptr = lcfg_table[file].data[i++];
				//ascii = *ptr;
				//chprintf((BaseSequentialStream *)&itm_port, "character: %c in file: %i with len: %i\n", ascii, file, len);
			
				ptr++;
			}

			len--;
		}while((len > 0) && (lcfg_table[file].data[i] != EOF));
		
		return buffersize - len;
	}

  __errno_r(r) = EINVAL;
  return -1;
#endif
}

/***************************************************************************/

int _lseek_r(struct _reent *r, int file, int ptr, int dir)
{
  (void)r;
  (void)file;
  (void)ptr;
  (void)dir;

  return 0;
}

/***************************************************************************/

int _write_r(struct _reent *r, int file, char * ptr, int len)
{
  int n;
  (void)r;
  //(void)file;
  (void)ptr;
#if defined(STDOUT_SD)
  if (file != 1) {
    __errno_r(r) = EINVAL;
    return -1;
  }
  sdWrite(&STDOUT_SD, (uint8_t *)ptr, (size_t)len);
#endif
  switch (file) {
      case 1: /*stdout*/
      case 2: /* stderr */
          for (n = 0; n < len; n++) {
        	  chprintf((BaseSequentialStream *)&itm_port, "%c", *ptr++);
          }
          break;
      default:
          errno = EBADF;
          return -1;
  }
  return len;
}

/***************************************************************************/

int _close_r(struct _reent *r, int file)
{
  (void)r;
  (void)file;

  return 0;
}

/***************************************************************************/

caddr_t _sbrk_r(struct _reent *r, int incr)
{
#if CH_CFG_USE_MEMCORE
  void *p;

  chDbgCheck(incr > 0);

  p = chCoreAlloc((size_t)incr);
  if (p == NULL) {
    __errno_r(r) = ENOMEM;
    return (caddr_t)-1;
  }
  return (caddr_t)p;
#else
  (void)incr;
  __errno_r(r) = ENOMEM;
  return (caddr_t)-1;
#endif
}

/***************************************************************************/

int _fstat_r(struct _reent *r, int file, struct stat * st)
{
  (void)r;
  (void)file;

  memset(st, 0, sizeof(*st));
  st->st_mode = S_IFCHR;
  return 0;
}

/***************************************************************************/

int _isatty_r(struct _reent *r, int fd)
{
  (void)r;
  (void)fd;

  return 1;
}

/***************************************************************************/

int _gettimeofday(struct timeval *tv)
{
	RTCDateTime timespec;

	rtcGetTime(&RTCD1, &timespec);
    uint64_t t = (uint64_t)timespec.millisecond * 1000000;/*__your_system_time_function_here__();*/  // get uptime in nanoseconds
    tv->tv_sec = t / 1000000000;  // convert to seconds
    tv->tv_usec = ( t % 1000000000 ) / 1000;  // get remaining microseconds
    chprintf((BaseSequentialStream *)&itm_port, "timespec: time %d usec. %d sec. %d\n", t, tv->tv_usec, tv->tv_sec);
    return 0;  // return non-zero for error
} // end _gettimeofday()

/*** EOF ***/
