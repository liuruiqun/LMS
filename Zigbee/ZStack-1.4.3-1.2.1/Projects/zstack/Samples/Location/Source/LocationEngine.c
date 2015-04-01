/**************************************************************************************************
  Filename:       LocationEngine.c
  Revised:        $Date: 2007-10-27 17:16:54 -0700 (Sat, 27 Oct 2007) $
  Revision:       $Revision: 15793 $

  Description:    Location Engine Functions.


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

#if defined ( CC2431 )

/*********************************************************************
 * INCLUDES
 */

#include "ZComDef.h"
#include "LocationEngine.h"
#include "hal_mcu.h"

/*********************************************************************
 * MACROS
 */

#define XREG(addr)       ((unsigned char volatile __xdata *) 0)[addr]

#define  REFCOORD    XREG( 0xDF55 )  /*  Location Engine             */
#define  MEASPARM    XREG( 0xDF56 )  /*  Location Engine             */
#define  LOCENG      XREG( 0xDF57 )  /*  Location Engine             */
#define  LOCX        XREG( 0xDF58 )  /*  Location Engine             */
#define  LOCY        XREG( 0xDF59 )  /*  Location Engine             */
#define  LOCMIN      XREG( 0xDF5A )  /*  Location Engine             */

//Location engine enable
#define LOC_ENABLE()  do { LOCENG |= 0x10;  } while(0)
#define LOC_DISABLE() do { LOCENG &= ~0x10; } while(0)

//Location engine load parameters
#define LOC_PARAMETER_LOAD( on )\
   do {                         \
      if(on) LOCENG |= 0x04;    \
      else LOCENG &= ~0x04;     \
   } while(0)

//Location engine load reference coordinates
#define LOC_REFERENCE_LOAD( on )\
   do {                         \
      if(on) LOCENG |= 0x02;    \
      else LOCENG &= ~0x02;     \
} while(0)

//Location engine run
#define LOC_RUN() do { LOCENG |= 0x01; } while(0)

//Location engine done
#define LOC_DONE()  (LOCENG & 0x08)

/*********************************************************************
 * @fn      locationCalculatePosition
 *
 * @brief   Calculates the position of this device.  This function
 *          only works in a 2431.
 *
 * @param   ref - input - Reference Nodes
 * @param   node - input/output - This node's information
 *
 * @return  none
 */
void locationCalculatePosition( LocRefNode_t *ref, LocDevCfg_t *node )
{
  LocRefNode_t *pRef = ref;
  // Rev-B Chip have LocEng Ver 1.0 w/ cap=8, Rev-C have LocEng Ver 2.0 w/ 16.
  const byte stop = ( ( CHVER == 0x01 ) ? LOC_ENGINE_NODE_CAPACITY_REVB :
                                          LOC_ENGINE_NODE_CAPACITY_REVC);
  byte idx;

  LOC_DISABLE();	
  LOC_ENABLE();	

  // Load the reference coordinates.
  LOC_REFERENCE_LOAD( TRUE );

  for ( idx = 0; idx < stop; idx++ )
  {
    REFCOORD = pRef->x;		
    REFCOORD = pRef->y;
    pRef++;
  }			
  LOC_REFERENCE_LOAD( FALSE );

  // Load the measured parameters.
  LOC_PARAMETER_LOAD( TRUE );	

  MEASPARM = node->param_a;  	
  MEASPARM = node->param_n;

  // CC2431 rev. C->
  if ( CHVER != 0x01 )
  {
    MEASPARM = LOC_ENGINE_X_MIN;
    MEASPARM = LOC_ENGINE_X_MAX;

    MEASPARM = LOC_ENGINE_Y_MIN;
    MEASPARM = LOC_ENGINE_Y_MAX;
  }

  // Load the measured RSSI values shifted for not used fractional bit.
  for ( idx = 0; idx < stop; idx++ )
  {
    MEASPARM = ref->rssi * 2;
    ref++;
  }

  LOC_PARAMETER_LOAD( FALSE );	

  LOC_RUN();

  while( !(LOCENG & 0x08) );

  if ( CHVER == 0x01 )
  {
    // Convert output format (LSB = .5m) to input format (2 LSB's = .25m).
    node->x = (LOCX << 1);
    node->y = (LOCY << 1);
    node->min = LOCMIN;
  }
  else
  {
    node->x = LOCX + 2;
    node->y = LOCY;
    node->min = 0;
  }

  LOC_DISABLE();	
}

/*********************************************************************
*********************************************************************/
#endif // CC2431
