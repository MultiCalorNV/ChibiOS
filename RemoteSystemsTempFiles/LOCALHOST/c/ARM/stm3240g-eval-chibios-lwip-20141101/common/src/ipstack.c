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
*  19.05.2013  mifi  First Version.
*  19.08.2013  mifi  Some rework and cleanup.
*  18.04.2014  mifi  Some rework, HW init of ETH will be done by
*                    ethernetif_init.
**************************************************************************/
#define __IPSTACK_C__

/*=======================================================================*/
/*  Include                                                              */
/*=======================================================================*/
#include <string.h>
#include "project.h"
#include "ethernetif.h"

#include "lwip\api.h"
#include "lwip\tcpip.h"
#include "lwip\inet.h"
#include "lwip\netif.h"

/*=======================================================================*/
/*  Extern                                                               */
/*=======================================================================*/
extern IP_PHY_DRIVER *BoardETHConfig (void);

/*=======================================================================*/
/*  All Structures and Common Constants                                  */
/*=======================================================================*/

/*=======================================================================*/
/*  Definition of all local Data                                         */
/*=======================================================================*/

static struct netif     NetIf;
static struct ip_addr   IPAddr;
static struct ip_addr   NETMask;
static struct ip_addr   GW;

/*=======================================================================*/
/*  Definition of all local Procedures                                   */
/*=======================================================================*/

/*=======================================================================*/
/*  All code exported                                                    */
/*=======================================================================*/

/*************************************************************************/
/*  IPInit                                                               */
/*                                                                       */
/*  In    : none                                                         */
/*  Out   : none                                                         */
/*  Return: none                                                         */
/*************************************************************************/
void IPInit (void)
{
   uint8_t MACAddress[6];
   
   /* Setup MAC address */
   MACAddress[0] = IP_MAC_ADDRESS_0;
   MACAddress[1] = IP_MAC_ADDRESS_1;
   MACAddress[2] = IP_MAC_ADDRESS_2;
   MACAddress[3] = IP_MAC_ADDRESS_3;
   MACAddress[4] = IP_MAC_ADDRESS_4;
   MACAddress[5] = IP_MAC_ADDRESS_5;
   
   /* IP address setting */
   IPAddr.addr  = inet_addr(IP_DEFAULT_ADDRESS);
   NETMask.addr = inet_addr(IP_DEFAULT_NETMASK);
   GW.addr      = inet_addr(IP_DEFAULT_GATEWAY); 
   
   /* Initialize lwIP */
   tcpip_init(NULL, NULL);

   /* Add the network interface to the list of lwIP netifs. */
   netif_add(&NetIf, &IPAddr, &NETMask, &GW, &MACAddress, ethernetif_init, tcpip_input);

   /* Set the network interface as the default network interface. */
   netif_set_default(&NetIf);

   /* Bring the interface up. */
   netif_set_up(&NetIf); 
        
} /* IPInit */

/*************************************************************************/
/*  IPIsNetIfUp                                                          */
/*                                                                       */
/*  Check if the interface and link is Up.                               */
/*                                                                       */
/*  In    : none                                                         */
/*  Out   : none                                                         */
/*  Return: 0 / 1                                                        */
/*************************************************************************/
int IPIsNetIfUp (void)
{
   int NetIfUp = 0;
   
   if ((netif_is_up(&NetIf)) && (netif_is_link_up(&NetIf)))
   {
      NetIfUp = 1;
   }
   
   return(NetIfUp);
} /* IPIsNetIfUp */

/*** EOF ***/
