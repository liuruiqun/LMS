/**************************************************************************************************
  Filename:       LocationEngine.h
  Revised:        $Date: 2007-10-27 17:16:54 -0700 (Sat, 27 Oct 2007) $
  Revision:       $Revision: 15793 $

  Description:    This file contains the Location Engine definitions.


  Copyright 2006-2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE, 
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com. 
**************************************************************************************************/

#ifndef LOCATIONENGINE_H
#define LOCATIONENGINE_H

#ifdef __cplusplus
extern "C"
{
#endif

#if defined ( ZBIT )
  #define CC2431
#endif

#if defined ( CC2431 )

/*********************************************************************
 * INCLUDES
 */

#include "ZComDef.h"

/*********************************************************************
 * CONSTANTS
 */

#define LOC_ENGINE_NODE_CAPACITY_REVB   8
#define LOC_ENGINE_NODE_CAPACITY_REVC  16

// TBD - how to define/use min/max x/y?
#define LOC_ENGINE_X_MIN  0x00
#define LOC_ENGINE_X_MAX  0xff

#define LOC_ENGINE_Y_MIN  0x00
#define LOC_ENGINE_Y_MAX  0xff

/*********************************************************************
 * STRUCTURES
 */

typedef struct
{
  byte x;
  byte y;
  byte rssi;
} LocRefNode_t;

typedef struct
{
  byte  param_a;
  byte  param_n;

  byte  x;
  byte  y;
  byte  min;
} LocDevCfg_t;

/*
 * Calculate Location
 */
extern void locationCalculatePosition( LocRefNode_t *ref, LocDevCfg_t *node );

/*********************************************************************
*********************************************************************/
#endif // CC2431

#ifdef __cplusplus
}
#endif

#endif /* LOCATIONENGINE_H */
