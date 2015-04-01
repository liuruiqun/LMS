/**************************************************************************************************
  Filename:       SampleApp.c
  Revised:        $Date: 2007-10-27 17:16:54 -0700 (Sat, 27 Oct 2007) $
  Revision:       $Revision: 15793 $

  Description:    Sample Application for incorporating the Z-Stack Location
                  Profile into another Application.


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

/*********************************************************************
 * INCLUDES
 */

#include "ZComDef.h"
#include "SampleApp.h"
#include "LocationProfile.h"

#include "AF.h"
#if !defined( NONWK )
  #include "APS.h"
  #include "nwk.h"
  #include "ZDApp.h"
#endif
#include "OSAL.h"
#include "OSAL_Custom.h"
#include "OSAL_Nv.h"
#if defined( POWER_SAVING )
#include "OSAL_PwrMgr.h"
#endif
#include "OSAL_Tasks.h"

#include "hal_drivers.h"
#include "hal_key.h"
#if defined ( LCD_SUPPORTED )
  #include "hal_lcd.h"
#endif
#include "hal_led.h"
#include "OnBoard.h"

#if defined ( MT_TASK )
  #include "MTEL.h"
#endif

/*********************************************************************
 * CONSTANTS
 */

#define SAMPLE_APP_ENDPOINT                 10
#define SAMPLE_APP_PROFID                   0x2001
#define SAMPLE_APP_DEVICEID                 13
#define SAMPLE_APP_VERSION                  31
#define SAMPLE_APP_FLAGS                    0

#define SAMPLE_APP_GENERIC_ID               0x0001
#define SAMPLE_APP_ON_OFF_ID                0x0010

#define SAMPLE_APP_ON                       0xFF
#define SAMPLE_APP_OFF                      0x00
#define SAMPLE_APP_TOGGLE                   0xF0

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

uint8 SampleApp_TaskID;

// The order in this table must be identical to the task initialization calls below in osalInitTask.
const pTaskEventHandlerFn tasksArr[] = {
#if defined( ZMAC_F8W )
  macEventLoop,
#endif
#if !defined( NONWK )
  nwk_event_loop,
#endif
  Hal_ProcessEvent,
#if defined( MT_TASK )
  MT_ProcessEvent,
#endif
#if !defined( NONWK )
  APS_event_loop,
  ZDApp_event_loop,
#endif
  SampleApp_ProcessEvent,
#if defined ( LOCATION_REFNODE )
  RefNode_ProcessEvent
#endif
#if defined ( LOCATION_BLINDNODE )
  BlindNode_ProcessEvent
#endif
#if defined ( LOCATION_DONGLE )
  LocDongle_ProcessEvent
#endif
};

const uint8 tasksCnt = sizeof( tasksArr ) / sizeof( tasksArr[0] );
uint16 *tasksEvents;

/*********************************************************************
 * LOCAL VARIABLES
 */

#if defined ( LOCATION_REFNODE )
static const cId_t SampleApp_InputClusterList[] =
{
  SAMPLE_APP_GENERIC_ID,
  SAMPLE_APP_ON_OFF_ID
};

static const cId_t SampleApp_OutputClusterList[] =
{
  SAMPLE_APP_GENERIC_ID
};
#endif

#if defined ( LOCATION_BLINDNODE )
static const cId_t SampleApp_InputClusterList[] =
{
  SAMPLE_APP_GENERIC_ID
};

static const cId_t SampleApp_OutputClusterList[] =
{
  SAMPLE_APP_GENERIC_ID,
  SAMPLE_APP_ON_OFF_ID
};
#endif

#if defined ( LOCATION_DONGLE )
static const cId_t SampleApp_InputClusterList[] =
{
  SAMPLE_APP_GENERIC_ID,
  SAMPLE_APP_ON_OFF_ID
};

static const cId_t SampleApp_OutputClusterList[] =
{
  SAMPLE_APP_GENERIC_ID
};
#endif

static const SimpleDescriptionFormat_t SampleApp_SimpleDesc =
{
  SAMPLE_APP_ENDPOINT,
  SAMPLE_APP_PROFID,

  SAMPLE_APP_DEVICEID,

  SAMPLE_APP_VERSION,
  SAMPLE_APP_FLAGS,

  sizeof( SampleApp_InputClusterList ),
  (cId_t*)SampleApp_InputClusterList,

  sizeof( SampleApp_OutputClusterList ),
  (cId_t*)SampleApp_OutputClusterList
};

static const endPointDesc_t SampleApp_epDesc =
{
  SAMPLE_APP_ENDPOINT,
  &SampleApp_TaskID,
  (SimpleDescriptionFormat_t *)&SampleApp_SimpleDesc,
  noLatencyReqs
};

#if defined( POWER_SAVING )
/* The ZDO_STATE_CHANGE event will be received twice after initiating the
 * rejoin process, only act on one of them.
 */
static uint8 rejoinPending;
#endif

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void SampleApp_Init( uint8 task_id );
static void handleKeys( uint8 shift, uint8 keys );
static void processMSGCmd( afIncomingMSGPacket_t *pkt );

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/

/*********************************************************************
 * @fn      osalInitTasks
 *
 * @brief   This function invokes the initialization function for each task.
 *
 * @param   void
 *
 * @return  none
 */
void osalInitTasks( void )
{
  uint8 taskID = 0;

  tasksEvents = (uint16 *)osal_mem_alloc( sizeof( uint16 ) * tasksCnt);
  osal_memset( tasksEvents, 0, (sizeof( uint16 ) * tasksCnt));

#if defined( ZMAC_F8W )
  macTaskInit( taskID++ );
#endif
#if !defined( NONWK )
  nwk_init( taskID++ );
#endif
  Hal_Init( taskID++ );
#if defined( MT_TASK )
  MT_TaskInit( taskID++ );
#endif
#if !defined( NONWK )
  APS_Init( taskID++ );
  ZDApp_Init( taskID++ );
#endif
  SampleApp_Init( taskID++ );
#if defined ( LOCATION_REFNODE )
  RefNode_Init( taskID );
#endif
#if defined ( LOCATION_BLINDNODE )
  BlindNode_Init( taskID );
#endif
#if defined ( LOCATION_DONGLE )
  LocDongle_Init( taskID );
#endif
}

/*********************************************************************
 * @fn      SampleApp_Init
 *
 * @brief   Initialization function for the SampleApp OSAL task.
 *
 * @param   task_id - the ID assigned by OSAL.
 *
 * @return  none
 */
static void SampleApp_Init( uint8 task_id )
{
  SampleApp_TaskID = task_id;

  /* Sample NV Restore.
  if ( ZSUCCESS == osal_nv_item_init( SAMPLE_APP_NV_ITEM,
                            sizeof( sampleAppNV_Item_t ), &sampleAppNV_Item ) )
  {
    osal_nv_read( SAMPLE_APP_NV_ITEM, 0,
                            sizeof( sampleAppNV_Item_t ), &sampleAppNV_Item ) )
  }
  else
  {
    osal_nv_read( SAMPLE_APP_NV_ITEM, 0,
                            sizeof( sampleAppNV_Item_t ), &sampleAppNV_Item ) )
  }
   */

  // Register the endpoint/interface description with the AF.
  afRegister( (endPointDesc_t *)&SampleApp_epDesc );

  // Register for all key events - This app will handle all key events.
  RegisterForKeys( SampleApp_TaskID );

#if defined ( LCD_SUPPORTED )
  HalLcdWriteString( "<--- SampleApp --->", HAL_LCD_LINE_1 );
#endif
}

/*********************************************************************
 * @fn      SampleApp_ProcessEvent
 *
 * @brief   Generic Application Task event processor.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - Bit map of events to process.
 *
 * @return  none
 */
uint16 SampleApp_ProcessEvent( uint8 task_id, uint16 events )
{
  if ( events & SYS_EVENT_MSG )
  {
    afIncomingMSGPacket_t *MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(
                                                             SampleApp_TaskID );

    while ( MSGpkt != NULL )
    {
      switch ( MSGpkt->hdr.event )
      {
      case KEY_CHANGE:
        handleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
        break;

      case AF_DATA_CONFIRM_CMD:
#if !defined( RTR_NWK )
        {
        // This message is received as a confirmation of a data packet sent.
        // The status is of ZStatus_t type [defined in ZComDef.h]
        afDataConfirm_t *afDataConfirm = (afDataConfirm_t *)MSGpkt;

        /* No ACK from the MAC layer implies that mobile device is out of
         * range of most recent parent. Therefore, begin an orphan scan
         * to try to find a former parent.
         * NOTE: To get the fastest action in the process of finding a new
         * parent, set the MAX_JOIN_ATTEMPTS in ZDApp.c to 1.
         */
        if ( afDataConfirm->hdr.status == ZMacNoACK )
        {
          SampleApp_NoACK();
        }
        // Some other error -- Do something.
        else
        {
        }
        }
#endif
        break;

      case AF_INCOMING_MSG_CMD:
        processMSGCmd( MSGpkt );
        break;

      case ZDO_STATE_CHANGE:
#if defined( POWER_SAVING )
        if ( rejoinPending )
        {
          rejoinPending = FALSE;

          // Ok to resume power saving ops.
          SampleApp_Sleep( TRUE );
        }
#endif
        break;

      default:
        break;
      }

      osal_msg_deallocate( (uint8 *)MSGpkt );
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    }

    // Return unprocessed events.
    return ( events ^ SYS_EVENT_MSG );
  }

  return 0;  // Discard unknown events
}

/*********************************************************************
 * @fn      SampleApp_NoACK
 *
 * @brief   Sample Application recovery from getting a ZMacNoAck.
 *
 * @return  none
 */
void SampleApp_NoACK( void )
{
#if defined( POWER_SAVING )
  rejoinPending = TRUE;
  ZDApp_SendEventMsg( ZDO_NWK_JOIN_REQ, 0, NULL );

  SampleApp_Sleep( FALSE );
#endif
}

/*********************************************************************
 * @fn      SampleApp_Sleep
 *
 * @brief   Sample Application set allow/disallow sleep mode.
 *
 * @return  none
 */
void SampleApp_Sleep( uint8 allow )
{
#if defined( POWER_SAVING )
  if ( allow )
  {
    osal_pwrmgr_task_state( NWK_TaskID, PWRMGR_CONSERVE );
    NLME_SetPollRate( 0 );
  }
  else
  {
    osal_pwrmgr_task_state( NWK_TaskID, PWRMGR_HOLD );
    NLME_SetPollRate( 1000 );
  }
#endif
}

/*********************************************************************
 * @fn      handleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *
 * @return  none
 */
static void handleKeys( uint8 shift, uint8 keys )
{
/* POWER_SAVING key press interrupt uses shift key to awaken from deep sleep,
 * so it is not available.
 */
#if !defined( POWER_SAVING )
  /* Give the LocationProfile access to 4 keys by sending any Shif-Key
   * sequence to the profile as a normal Key.
   */
  if ( shift )
  {
    LocationHandleKeys( keys );
  }
  else
#endif
  {
    if ( keys & HAL_KEY_SW_1 )
    {
    }

    if ( keys & HAL_KEY_SW_2 )
    {
    }

    if ( keys & HAL_KEY_SW_3 )
    {
    }

    if ( keys & HAL_KEY_SW_4 )
    {
    }
  }

#if defined ( LOCATION_BLINDNODE )
  {
    static uint8 transId;

    uint8 actionId = SAMPLE_APP_TOGGLE;
    afAddrType_t dstAddr;

    dstAddr.addrMode = afAddrBroadcast;
    dstAddr.addr.shortAddr = NWK_BROADCAST_SHORTADDR_DEVALL;
    dstAddr.endPoint = SAMPLE_APP_ENDPOINT;

    // Control all lights within 1-hop radio range.
    (void)AF_DataRequest( &dstAddr, (endPointDesc_t *)&SampleApp_epDesc,
                           SAMPLE_APP_ON_OFF_ID, 1, &actionId,
                          &transId, AF_SKIP_ROUTING, 1 );

    BlindNode_FindRequest();
  }
#endif
}

/*********************************************************************
 * @fn      processMSGCmd
 *
 * @brief   Data message processor callback.
 *
 * @param   none
 *
 * @return  none
 */
static void processMSGCmd( afIncomingMSGPacket_t *pkt )
{
  switch ( pkt->clusterId )
  {
  case SAMPLE_APP_ON_OFF_ID:
    switch ( pkt->cmd.Data[0] )
    {
    case SAMPLE_APP_OFF:
      HalLedSet( HAL_LED_3, HAL_LED_MODE_OFF );
      break;

    case SAMPLE_APP_TOGGLE:
      HalLedSet( HAL_LED_3, HAL_LED_MODE_TOGGLE );
      break;

    case SAMPLE_APP_ON:
      HalLedSet( HAL_LED_3, HAL_LED_MODE_ON );
      break;
    }
    break;

  default:
    break;
  }
}

/*********************************************************************
*********************************************************************/
