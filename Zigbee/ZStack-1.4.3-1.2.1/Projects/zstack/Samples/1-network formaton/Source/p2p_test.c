/**************************************************************************************************
  Filename:       p2p_test.c
  Revised:        $Date: 2007-10-27 17:16:54 -0700 (Sat, 27 Oct 2007) $
  Revision:       $Revision: 15793 $

  Description:    Generic Application (no Profile).


  Copyright 2004-2007 Texas Instruments Incorporated. All rights reserved.

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
  PROVIDED “AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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
  This application isn't intended to do anything useful, it is
  intended to be a simple example of an application's structure.

  This application sends "Hello World" to another "Generic"
  application every 15 seconds.  The application will also
  receive "Hello World" packets.

  The "Hello World" messages are sent/received as MSG type message.

  This applications doesn't have a profile, so it handles everything
  directly - itself.

  Key control:
    SW1:
    SW2:  initiates end device binding
    SW3:
    SW4:  initiates a match description request
*********************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"

#include "p2p_test.h"
#include "DebugTrace.h"

#if !defined( WIN32 )
  #include "OnBoard.h"
#endif

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"
//#include "Hal_Sensor.h"  
#include "Hal_defs.h"
#include "hal_adc.h"
/*********************************************************************
 * MACROS
 */
 #define DATA P1_1

#define SCK P1_0

/*********************************************************************
 * CONSTANTS
 */
#define CLKSPD_1  (CLKCON & 0x01)


// This list should be filled with Application specific Cluster IDs.
/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// This list should be filled with Application specific Cluster IDs.
const cId_t p2p_test_ClusterList[p2p_test_MAX_CLUSTERS] =
{
  p2p_test_CLUSTERID,
  //fire_extinguisher_clusterId
};

const SimpleDescriptionFormat_t p2p_test_SimpleDesc =
{
  p2p_test_ENDPOINT,              //  int Endpoint;
  p2p_test_PROFID,                //  uint16 AppProfId[2];
  p2p_test_DEVICEID,              //  uint16 AppDeviceId[2];
  p2p_test_DEVICE_VERSION,        //  int   AppDevVer:4;
  p2p_test_FLAGS,                 //  int   AppFlags:4;
  p2p_test_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)p2p_test_ClusterList,  //  byte *pAppInClusterList;
  p2p_test_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)p2p_test_ClusterList   //  byte *pAppInClusterList;
};

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in p2p_test_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t p2p_test_epDesc;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
byte p2p_test_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // p2p_test_Init() is called.
devStates_t p2p_test_NwkState;


byte p2p_test_TransID;  // This is the unique message ID (counter)


byte p2p_test_TransID;  // This is the unique message ID (counter)

afAddrType_t p2p_test_DstAddr;
  uint8 p2p_msg[6];//the collect message
 uint16 shortaddr;
 uint16 fatheraddr;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
void p2p_test_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg );
void p2p_test_HandleKeys( byte shift, byte keys );
void p2p_test_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void p2p_test_SendTheMessage( void );

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      p2p_test_Init
 *
 * @brief   Initialization function for the Generic App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void p2p_test_Init( byte task_id )
{
  p2p_test_TaskID = task_id;
  p2p_test_NwkState = DEV_INIT;
  p2p_test_TransID = 0;

  // Device hardware initialization can be added here or in main() (Zmain.c).
  // If the hardware is application specific - add it here.
  // If the hardware is other parts of the device add it in main().

  p2p_test_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
  p2p_test_DstAddr.endPoint = p2p_test_ENDPOINT;
  p2p_test_DstAddr.addr.shortAddr = 0x0000;

  // Fill out the endpoint description.
  p2p_test_epDesc.endPoint = p2p_test_ENDPOINT;
  p2p_test_epDesc.task_id = &p2p_test_TaskID;
  p2p_test_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&p2p_test_SimpleDesc;
  p2p_test_epDesc.latencyReq = noLatencyReqs;

  // Register the endpoint description with the AF
  afRegister( &p2p_test_epDesc );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( p2p_test_TaskID );



  // Update the display
#if defined ( LCD_SUPPORTED )
    HalLcdWriteString( "p2p_test", HAL_LCD_LINE_1 );
#endif

  ZDO_RegisterForZDOMsg( p2p_test_TaskID, End_Device_Bind_rsp );
  ZDO_RegisterForZDOMsg( p2p_test_TaskID, Match_Desc_rsp );
}

/*********************************************************************
 * @fn      p2p_test_ProcessEvent
 *
 * @brief   Generic Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
UINT16 p2p_test_ProcessEvent( byte task_id, UINT16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  afDataConfirm_t *afDataConfirm;

  // Data Confirmation message fields
  byte sentEP;
  ZStatus_t sentStatus;
  byte sentTransID;       // This should match the value sent

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( p2p_test_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        case ZDO_CB_MSG:
          p2p_test_ProcessZDOMsgs( (zdoIncomingMsg_t *)MSGpkt );
          break;

        case KEY_CHANGE:
          // p2p_test_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          
          break;
      case INTER_CHANGE:
          p2p_test_SendRainfall();
          break;

        case AF_DATA_CONFIRM_CMD:
          // This message is received as a confirmation of a data packet sent.
          // The status is of ZStatus_t type [defined in ZComDef.h]
          // The message fields are defined in AF.h
          afDataConfirm = (afDataConfirm_t *)MSGpkt;
          sentEP = afDataConfirm->endpoint;
          sentStatus = afDataConfirm->hdr.status;
          sentTransID = afDataConfirm->transID;
          (void)sentEP;
          (void)sentTransID;

          // Action taken when confirmation is received.
          if ( sentStatus != ZSuccess )
          {
            // The data wasn't delivered -- Do something
          }
          break;

        case AF_INCOMING_MSG_CMD:
          p2p_test_MessageMSGCB( MSGpkt );
          
 //         HalLedSet( HAL_LED_1, HAL_LED_MODE_ON );
  //        MicroWait( 62500 );
  //        MicroWait( 62500 );
   //       HalLedSet( HAL_LED_1, HAL_LED_MODE_OFF );
          break;

        case ZDO_STATE_CHANGE:
          p2p_test_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if (  (p2p_test_NwkState == DEV_ROUTER)
              || (p2p_test_NwkState == DEV_END_DEVICE) )
          {
              //osal_start_timerEx( p2p_test_TaskID,
              //                  p2p_test_SEND_MSG_EVT,
              //                 10 );
          }
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( p2p_test_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // Send a message out - This event is generated by a timer
  //  (setup in p2p_test_Init()).
  if ( events & p2p_test_SEND_MSG_EVT )
  {
    p2p_msg[0] = 0xFF;
    //next is for ieeaddr
    shortaddr = NLME_GetShortAddr();
    p2p_msg[1] = LO_UINT16(shortaddr);
    p2p_msg[2] = HI_UINT16(shortaddr);
    fatheraddr = NLME_GetCoordShortAddr();
    p2p_msg[3] = LO_UINT16(fatheraddr);
    p2p_msg[4] = HI_UINT16(fatheraddr);
    uint8 *ieeeAddr;
    ieeeAddr = NLME_GetExtAddr();
    p2p_msg[5]=  *(uint8 *)(ieeeAddr+7);//p2p_DeviceID; 
    // Send "the" message
    p2p_test_SendTheMessage();
    osal_start_timerEx( p2p_test_TaskID,
                        p2p_test_SEND_MSG_EVT,
                        p2p_test_SEND_MSG_TIMEOUT );

    // return unprocessed events
    return (events ^ p2p_test_SEND_MSG_EVT);
  }

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * Event Generation Functions
 */
/*********************************************************************
 * @fn      p2p_test_ProcessZDOMsgs()
 *
 * @brief   Process response messages
 *
 * @param   none
 *
 * @return  none
 */
void p2p_test_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg )
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
            p2p_test_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
            p2p_test_DstAddr.addr.shortAddr = pRsp->nwkAddr;
            // Take the first endpoint, Can be changed to search through endpoints
            p2p_test_DstAddr.endPoint = pRsp->epList[0];

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
 * @fn      p2p_test_HandleKeys
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
void p2p_test_SendRainfall(void)
{
  if (  (p2p_test_NwkState == DEV_ROUTER)
              || (p2p_test_NwkState == DEV_END_DEVICE) )
  {
    unsigned char p2p_msg[3];
    p2p_msg[0] = HI_UINT16(shortaddr);
    p2p_msg[1] = LO_UINT16(shortaddr);
    p2p_msg[2] = 1;
    
    AF_DataRequest( &p2p_test_DstAddr, &p2p_test_epDesc,
                       p2p_test_CLUSTERID,
                       3,
                       (byte *)&p2p_msg,
                       &p2p_test_TransID,
                       AF_DISCV_ROUTE, AF_DEFAULT_RADIUS );
  }
}
void p2p_test_HandleKeys( byte shift, byte keys )
{
  zAddrType_t dstAddr;

  // Shift is used to make each button/switch dual purpose.
  if ( shift )
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
  else
  {
    if ( keys & HAL_KEY_SW_1 )
    {
    }

    if ( keys & HAL_KEY_SW_2 )
    {
      HalLedSet ( HAL_LED_4, HAL_LED_MODE_OFF );

      // Initiate an End Device Bind Request for the mandatory endpoint
      dstAddr.addrMode = Addr16Bit;
      dstAddr.addr.shortAddr = 0x0000; // Coordinator
      ZDP_EndDeviceBindReq( &dstAddr, NLME_GetShortAddr(),
                            p2p_test_epDesc.endPoint,
                            p2p_test_PROFID,
                            p2p_test_MAX_CLUSTERS, (cId_t *)p2p_test_ClusterList,
                            p2p_test_MAX_CLUSTERS, (cId_t *)p2p_test_ClusterList,
                            FALSE );
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
                        p2p_test_PROFID,
                        p2p_test_MAX_CLUSTERS, (cId_t *)p2p_test_ClusterList,
                        p2p_test_MAX_CLUSTERS, (cId_t *)p2p_test_ClusterList,
                        FALSE );
    }
  }
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      p2p_test_MessageMSGCB
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   none
 *
 * @return  none
 */
void p2p_test_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  uint8 msg_head[2];
  uint8 check;
  switch ( pkt->clusterId )
  {
    case p2p_test_CLUSTERID:
      // "the" message
      //the head
      msg_head[0] = 0xAA;
      msg_head[1] = 0x55;
      check = 0x01; //test, not true
      HalUARTWrite( 0, (uint8*)msg_head, 2 );
      HalUARTWrite( 0, (uint8*)pkt->cmd.Data, pkt->cmd.DataLength );
      HalUARTWrite( 0, &check, 1 );
      break;
   
  }
}


/*********************************************************************
 * @fn      p2p_test_SendTheMessage
 *
 * @brief   Send "the" message.
 *
 * @param   none
 *
 * @return  none
 */
void p2p_test_SendTheMessage( void )
{
   //char p2p_msg[] = "Hello World ";
   if ( AF_DataRequest( &p2p_test_DstAddr, &p2p_test_epDesc,
                       p2p_test_CLUSTERID,
                       6,
                       (byte *)&p2p_msg,
                       &p2p_test_TransID,
                       AF_DISCV_ROUTE, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )

  {
    // Successfully requested to be sent.  
//    HalUARTWrite( 0,  (uint8 *)p2p_msg, (byte)osal_strlen( p2p_msg ) + 1);

  } 
  
  else
  {
    // Error occurred in request to send.
  }
}



