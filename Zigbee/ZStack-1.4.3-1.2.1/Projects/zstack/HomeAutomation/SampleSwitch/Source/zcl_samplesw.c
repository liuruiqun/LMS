/**************************************************************************************************
  Filename:       zcl_samplesw.c
  Revised:        $Date: 2007-11-20 13:57:50 -0800 (Tue, 20 Nov 2007) $
  Revision:       $Revision: 15954 $

  Description:    Zigbee Cluster Library - sample device application.


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
  This device will be like an On/Off Switch device. This application
  is not intended to be a On/Off Switch device, but will use the device
  description to implement this sample code.
*********************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"

#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ha.h"
#include "zcl_samplesw.h"

#include "onboard.h"

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
byte zclSampleSw_TaskID;

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static afAddrType_t zclSampleSw_DstAddr;

#define ZCLSAMPLESW_BINDINGLIST       1
static cId_t bindingOutClusters[ZCLSAMPLESW_BINDINGLIST] =
{
  ZCL_HA_CLUSTER_ID_GEN_ON_OFF
};

// Test Endpoint to allow SYS_APP_MSGs
static endPointDesc_t sampleSw_TestEp =
{
  20,                                 // Test endpoint
  &zclSampleSw_TaskID,
  (SimpleDescriptionFormat_t *)NULL,  // No Simple description for this test endpoint
  (afNetworkLatencyReq_t)0            // No Network Latency req
};


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void zclSampleSw_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg );
static void zclSampleSw_HandleKeys( byte shift, byte keys );
static void zclSampleSw_BasicResetCB( void );
static void zclSampleSw_IdentifyCB( zclIdentify_t *pCmd );
static void zclSampleSw_IdentifyQueryRspCB(  zclIdentifyQueryRsp_t *pRsp );
static void zclSampleSw_ProcessIdentifyTimeChange( void );

/*********************************************************************
 * ZCL General Profile Callback table
 */
static zclGeneral_AppCallbacks_t zclSampleSw_CmdCallbacks =
{
  zclSampleSw_BasicResetCB,     // Basic Cluster Reset command
  zclSampleSw_IdentifyCB,       // Identify command  
  zclSampleSw_IdentifyQueryRspCB, // Identify Query Response command
  NULL,                         // On / Off cluster command - not needed.
  NULL,                         // Level Control Move to Level command
  NULL,                         // Level Control Move command
  NULL,                         // Level Control Step command
  NULL,                         // Group Response commands
  NULL,                         // Scene Store Request command
  NULL,                         // Scene Recall Request command
  NULL,                         // Scene Response commands
  NULL,                         // Alarm (Response) commands
  NULL,                         // RSSI Location commands
  NULL,                         // RSSI Location Response commands
};


/*********************************************************************
 * @fn          zclSampleSw_Init
 *
 * @brief       Initialization function for the zclGeneral layer.
 *
 * @param       none
 *
 * @return      none
 */
void zclSampleSw_Init( byte task_id )
{
  zclSampleSw_TaskID = task_id;

  // Set destination address to indirect
  zclSampleSw_DstAddr.addrMode = (afAddrMode_t)AddrNotPresent;
  zclSampleSw_DstAddr.endPoint = 0;
  zclSampleSw_DstAddr.addr.shortAddr = 0;

  // This app is part of the Home Automation Profile
  zclHA_Init( &zclSampleSw_SimpleDesc );

  // Register the ZCL General Cluster Library callback functions
  zclGeneral_RegisterCmdCallbacks( SAMPLESW_ENDPOINT, &zclSampleSw_CmdCallbacks );

  // Register the application's attribute list
  zcl_registerAttrList( SAMPLESW_ENDPOINT, SAMPLESW_MAX_ATTRIBUTES, zclSampleSw_Attrs );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( zclSampleSw_TaskID );

  // Register for a test endpoint
  afRegister( &sampleSw_TestEp );
  
  ZDO_RegisterForZDOMsg( zclSampleSw_TaskID, End_Device_Bind_rsp );
  ZDO_RegisterForZDOMsg( zclSampleSw_TaskID, Match_Desc_rsp );
}

/*********************************************************************
 * @fn          zclSample_event_loop
 *
 * @brief       Event Loop Processor for zclGeneral.
 *
 * @param       none
 *
 * @return      none
 */
uint16 zclSampleSw_event_loop( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;

  if ( events & SYS_EVENT_MSG )
  {
    while ( (MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( zclSampleSw_TaskID )) )
    {
      switch ( MSGpkt->hdr.event )
      {
        case ZDO_CB_MSG:
          zclSampleSw_ProcessZDOMsgs( (zdoIncomingMsg_t *)MSGpkt );
          break;
          
        case KEY_CHANGE:
          zclSampleSw_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  if ( events & SAMPLESW_IDENTIFY_TIMEOUT_EVT )
  {
    zclSampleSw_IdentifyTime = 10;
    zclSampleSw_ProcessIdentifyTimeChange();

    return ( events ^ SAMPLESW_IDENTIFY_TIMEOUT_EVT );
  }

  // Discard unknown events
  return 0;
}


/*********************************************************************
 * @fn      zclSampleSw_ProcessZDOMsgs()
 *
 * @brief   Process response messages
 *
 * @param   none
 *
 * @return  none
 */
void zclSampleSw_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg )
{
  switch ( inMsg->clusterID )
  {
    case End_Device_Bind_rsp:
      if ( ZDO_ParseBindRsp( inMsg ) == ZSuccess )
      {
        // Light LED
        HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
      }
#if defined(BLINK_LEDS)
      else
      {
        // Flash LED to show failure
        HalLedSet ( HAL_LED_4, HAL_LED_MODE_FLASH );
      }
#endif
      break;
      
    case Match_Desc_rsp:
      {
        ZDO_ActiveEndpointRsp_t *pRsp = ZDO_ParseEPListRsp( inMsg );
        if ( pRsp )
        {
          if ( pRsp->status == ZSuccess && pRsp->cnt )
          {
            zclSampleSw_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
            zclSampleSw_DstAddr.addr.shortAddr = pRsp->nwkAddr;
            // Take the first endpoint, Can be changed to search through endpoints
            zclSampleSw_DstAddr.endPoint = pRsp->epList[0];
            
            // Light LED
            HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
          }
          osal_mem_free( pRsp );
        }
      }
      break;
  }
}

/*********************************************************************
 * @fn      zclSampleSw_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_4
 *                 HAL_KEY_SW_3
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
static void zclSampleSw_HandleKeys( byte shift, byte keys )
{
  zAddrType_t dstAddr;

  if ( keys & HAL_KEY_SW_1 )
  {
    // Using this as the "Light Switch"
    zclGeneral_SendOnOff_CmdToggle( SAMPLESW_ENDPOINT, &zclSampleSw_DstAddr, false, 0 );
  }

  if ( keys & HAL_KEY_SW_2 )
  {
    HalLedSet ( HAL_LED_4, HAL_LED_MODE_OFF );
    
    // Initiate an End Device Bind Request, this bind request will
    // only use a cluster list that is important to binding.
    dstAddr.addrMode = afAddr16Bit;
    dstAddr.addr.shortAddr = 0;   // Coordinator makes the match
    ZDP_EndDeviceBindReq( &dstAddr, NLME_GetShortAddr(),
                           SAMPLESW_ENDPOINT,
                           ZCL_HA_PROFILE_ID,
                           0, NULL,   // No incoming clusters to bind
                           ZCLSAMPLESW_BINDINGLIST, bindingOutClusters,
                           TRUE );
  }

  if ( keys & HAL_KEY_SW_3 )
  {
  }
  
  if ( keys & HAL_KEY_SW_4 )
  {
    HalLedSet ( HAL_LED_4, HAL_LED_MODE_OFF );
    
    // Initiate a Match Description Request (Service Discovery)
    dstAddr.addrMode = AddrBroadcast;
    dstAddr.addr.shortAddr = NWK_BROADCAST_SHORTADDR;
    ZDP_MatchDescReq( &dstAddr, NWK_BROADCAST_SHORTADDR,
                       ZCL_HA_PROFILE_ID,
                       ZCLSAMPLESW_BINDINGLIST, bindingOutClusters,
                       0, NULL,   // No incoming clusters to bind
                       FALSE );
  }
}

/*********************************************************************
 * @fn      zclSampleSw_ProcessIdentifyTimeChange
 *
 * @brief   Called to process any change to the IdentifyTime attribute.
 *
 * @param   none
 *
 * @return  none
 */
static void zclSampleSw_ProcessIdentifyTimeChange( void )
{
  if ( zclSampleSw_IdentifyTime > 0 )
  {
    osal_start_timerEx( zclSampleSw_TaskID, SAMPLESW_IDENTIFY_TIMEOUT_EVT, 1000 );
    HalLedBlink ( HAL_LED_4, 0xFF, HAL_LED_DEFAULT_DUTY_CYCLE, HAL_LED_DEFAULT_FLASH_TIME );
  }
  else
  {
    if ( zclSampleSw_OnOff )
      HalLedSet ( HAL_LED_4, HAL_LED_MODE_ON );
    else
      HalLedSet ( HAL_LED_4, HAL_LED_MODE_OFF );
    osal_stop_timerEx( zclSampleSw_TaskID, SAMPLESW_IDENTIFY_TIMEOUT_EVT );
  }
}

/*********************************************************************
 * @fn      zclSampleSw_BasicResetCB
 *
 * @brief   Callback from the ZCL General Cluster Library
 *          to set all the Basic Cluster attributes to  default values.
 *
 * @param   none
 *
 * @return  none
 */
static void zclSampleSw_BasicResetCB( void )
{
}

/*********************************************************************
 * @fn      zclSampleSw_IdentifyCB
 *
 * @brief   Callback from the ZCL General Cluster Library when
 *          it received an Identity Command for this application.
 *
 * @param   srcAddr - source address and endpoint of the response message
 * @param   identifyTime - the number of seconds to identify yourself
 *
 * @return  none
 */
static void zclSampleSw_IdentifyCB( zclIdentify_t *pCmd )
{
  zclSampleSw_IdentifyTime = pCmd->identifyTime;
  zclSampleSw_ProcessIdentifyTimeChange();
}

/*********************************************************************
 * @fn      zclSampleSw_IdentifyQueryRspCB
 *
 * @brief   Callback from the ZCL General Cluster Library when
 *          it received an Identity Query Response Command for this application.
 *
 * @param   srcAddr - source address
 * @param   timeout - number of seconds to identify yourself (valid for query response)
 *
 * @return  none
 */
static void zclSampleSw_IdentifyQueryRspCB(  zclIdentifyQueryRsp_t *pRsp )
{
  // Query Response (with timeout value)
}


/****************************************************************************
****************************************************************************/


