/**************************************************************************************************
  Filename:       LocDongle.c
  Revised:        $Date: 2007-10-27 17:16:54 -0700 (Sat, 27 Oct 2007) $
  Revision:       $Revision: 15793 $

  Description:    This application resides in a dongle and enables a PC GUI or
                  other application to send and recieve OTA location messages.


  Copyright 2007 Texas Instruments Incorporated. All rights reserved.

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
#include "OSAL.h"
#include "MTEL.h"
#include "AF.h"
#include "ZDApp.h"

#include "OnBoard.h"
#include "hal_key.h"
#if defined ( LCD_SUPPORTED )
#include "hal_lcd.h"
#endif

#include "LocationProfile.h"
#include "LocDongle.h"

#if !defined ( MT_TASK )
  #error      // No need for this module if MT_TASK isn't defined
#endif

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

//#define ZBIT  //ggg
 // #define DONGLE_TIMER_EVT  0x0001

#define LOCDONGLE_TX_OPTIONS          AF_MSG_ACK_REQUEST

#if defined ( ZBIT )
  #define DONGLE_TIMER_DLY  1000
#endif


static const cId_t LocDongle_InputClusterList[] =
{
  LOCATION_XY_RSSI_RESPONSE,
  LOCATION_BLINDNODE_FIND_RESPONSE,
  LOCATION_BLINDNODE_CONFIG,
  LOCATION_REFNODE_CONFIG
};

static const cId_t LocDongle_OutputClusterList[] =
{
  LOCATION_XY_RSSI_REQUEST,
  LOCATION_BLINDNODE_FIND_REQUEST,
  LOCATION_REFNODE_CONFIG,
  LOCATION_BLINDNODE_CONFIG,
  LOCATION_REFNODE_REQUEST_CONFIG,
  LOCATION_BLINDNODE_REQUEST_CONFIG
};

static const SimpleDescriptionFormat_t LocDongle_SimpleDesc =
{
  LOCATION_DONGLE_ENDPOINT,
  LOCATION_PROFID,

  LOCATION_DONGLE_DEVICEID,

  LOCATION_DEVICE_VERSION,
  LOCATION_FLAGS,

  sizeof(LocDongle_InputClusterList),
  (cId_t *)LocDongle_InputClusterList,

  sizeof(LocDongle_OutputClusterList),
  (cId_t *)LocDongle_OutputClusterList
};

static const endPointDesc_t epDesc =
{
  LOCATION_DONGLE_ENDPOINT,
  &LocDongle_TaskID,
  (SimpleDescriptionFormat_t *)&LocDongle_SimpleDesc,
  noLatencyReqs
};

/*********************************************************************
 * TYPEDEFS
 */

#if defined ( ZBIT )
  typedef struct
  {
    uint16 addr;
    uint8  ep;
  } Node_t;
#endif

/*********************************************************************
 * GLOBAL VARIABLES
 */

uint8 LocDongle_TaskID;

/*********************************************************************
 * LOCAL VARIABLES
 */

uint8 LocDongle_TransID;  // This is the unique message ID (counter)

#if defined ( ZBIT )
  static Node_t blindNodes[1];
#endif

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void LocDongle_ProcessMSGCmd( afIncomingMSGPacket_t *pckt );
static void LocDongle_MTMsg( uint8 len, uint8 *msg );

#if defined ( ZBIT )
  static void processTimerEvt( void );
#endif

/*********************************************************************
 * @fn      LocDongle_Init
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
void LocDongle_Init( uint8 task_id )
{
  LocDongle_TaskID = task_id;
  LocDongle_TransID = 0;

  // Register the endpoint/interface description with the AF
  afRegister( (endPointDesc_t *)&epDesc );

  // Register for all key events - This app will handle all key events
  // RegisterForKeys( LocDongle_TaskID );
  
  // Update the display
#if defined ( LCD_SUPPORTED )
  HalLcdWriteString( "Location-Dongle", HAL_LCD_LINE_2 );
#endif
}

/*********************************************************************
 * @fn      LocDongle_ProcessEvent
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
UINT16 LocDongle_ProcessEvent( uint8 task_id, UINT16 events )
{
  afIncomingMSGPacket_t *MSGpkt;

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( LocDongle_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
      case MT_SYS_APP_MSG:  // Z-Architect Messages
          LocDongle_MTMsg( ((mtSysAppMsg_t *)MSGpkt)->appDataLen, ((mtSysAppMsg_t *)MSGpkt)->appData );
          break;

        case KEY_CHANGE:
          //keyChange( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        case AF_INCOMING_MSG_CMD:
          LocDongle_ProcessMSGCmd( MSGpkt );
          break;

        default:
          break;
      }

      osal_msg_deallocate( (uint8 *)MSGpkt );
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( LocDongle_TaskID );
    }

    // Return unprocessed events
    return ( events ^ SYS_EVENT_MSG );
  }

#if defined ( ZBIT )
  if ( events & DONGLE_TIMER_EVT )
  {
    processTimerEvt();

    // Return unprocessed events.
    return ( events ^ DONGLE_TIMER_EVT );
  }
#endif

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * Event Generation Functions
 */
/*********************************************************************
 * @fn      keyChange
 *
 * @brief   Handles all key events for this device.
 *
 * @param   keys - bit field for key events. Valid entries:
 *                 EVAL_SW4
 *                 EVAL_SW3
 *                 EVAL_SW2
 *                 EVAL_SW1
 *
 * @return  none
 */
void LocationHandleKeys( uint8 keys )
{
  if ( keys & HAL_KEY_SW_1 )
  {
#if defined ( ZBIT )
    osal_start_timerEx(LocDongle_TaskID, DONGLE_TIMER_EVT, DONGLE_TIMER_DLY);
#endif
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

/*********************************************************************
 * @fn      LocDongle_ProcessMSGCmd
 *
 * @brief   All incoming messages are sent out the serial port
 *          as an MT SYS_APP_MSG.
 *
 * @param   Raw incoming MSG packet structure pointer.
 *
 * @return  none
 */
static void LocDongle_ProcessMSGCmd( afIncomingMSGPacket_t *pkt )
{
#if defined ( ZBIT )
  switch ( pkt->clusterId )
  {
    case LOCATION_BLINDNODE_RESPONSE:
    case LOCATION_BLINDNODE_CONFIG:
      blindNodes[0].addr = pkt->srcAddr.addr.shortAddr;
      blindNodes[0].ep = pkt->srcAddr.endPoint;
      break;

    default:
      break;
  }
#else
  #include "SPIMgr.h"
  // All incoming messages are sent out the serial port as a SYS_APP_MSG.
  uint16 cmd = SPI_RESPONSE_BIT | SPI_CMD_SYS_APP_MSG;
  // DstEP + SrcAddr + SrcEP + cId + len = 1+2+1+2+1
  uint8 buf[SPI_0DATA_MSG_LEN + 7];
  uint8 *pBuf = buf;
  uint8 succ = FALSE;

  *pBuf++ = SOP_VALUE;
  *pBuf++ = (uint8)(HI_UINT16( cmd ));
  *pBuf++ = (uint8)(LO_UINT16( cmd ));
  *pBuf++ = pkt->cmd.DataLength + 7;

  *pBuf++ = LOCATION_DONGLE_ENDPOINT;
  *pBuf++ = LO_UINT16( pkt->srcAddr.addr.shortAddr );
  *pBuf++ = HI_UINT16( pkt->srcAddr.addr.shortAddr );
  *pBuf++ = pkt->srcAddr.endPoint;
  *pBuf++ = LO_UINT16( pkt->clusterId );
  *pBuf++ = HI_UINT16( pkt->clusterId );
  *pBuf++ = pkt->cmd.DataLength;

  //(void)HalUARTWrite( SPI_MGR_DEFAULT_PORT, buf, 11 );
  if ( HalUARTWrite( SPI_MGR_DEFAULT_PORT, buf, 11 ) )
  {
    //(void)HalUARTWrite( SPI_MGR_DEFAULT_PORT, pkt->cmd.Data, pkt->cmd.DataLength);
    if ( HalUARTWrite( SPI_MGR_DEFAULT_PORT, pkt->cmd.Data, pkt->cmd.DataLength) )
    {
      cmd = SPIMgr_CalcFCS( buf+1, 10 ) ^
            SPIMgr_CalcFCS( pkt->cmd.Data, pkt->cmd.DataLength );
      //(void)HalUARTWrite( SPI_MGR_DEFAULT_PORT, (uint8 *)(&cmd), 1 );
      if ( HalUARTWrite( SPI_MGR_DEFAULT_PORT, (uint8 *)(&cmd), 1 ) )
      {
        succ = TRUE;
      }
    }
  }
#endif
}

/*********************************************************************
 * @fn      LocDongle_MTMsg
 *
 * @brief   Process Monitor and Test messages
 *
 * @param   len - number of bytes
 * @param   msg - pointer to message
 *          0 - Lo byte destination address
 *          1 - Hi byte destination address
 *          2 - endpoint
 *          3 - lo byte cluster ID
 *          4 - hi byte cluster ID
 *          5 - data length
 *          6 - first byte of data
 *
 * @return  none
 */
static void LocDongle_MTMsg( uint8 len, uint8 *msg )
{
  afAddrType_t dstAddr;
  cId_t clusterID;
  uint8 dataLen;

  dstAddr.addrMode = afAddr16Bit;
  dstAddr.addr.shortAddr = BUILD_UINT16( msg[0], msg[1] );
  msg += 2;
  dstAddr.endPoint = *msg++;
  clusterID = BUILD_UINT16( msg[0], msg[1] );
  msg += 2;
  dataLen = *msg++;

  if ( (clusterID == LOCATION_XY_RSSI_REQUEST) ||
       (clusterID == LOCATION_REFNODE_CONFIG)  ||
       (clusterID == LOCATION_REFNODE_REQUEST_CONFIG) )
  {
    dstAddr.endPoint = LOCATION_REFNODE_ENDPOINT;
  }
  else
  {
    dstAddr.endPoint = LOCATION_BLINDNODE_ENDPOINT;
  }

  (void)AF_DataRequest( &dstAddr, (endPointDesc_t*)&epDesc, clusterID,
                         dataLen, msg,
                        &LocDongle_TransID, 0, AF_DEFAULT_RADIUS );
}

#if defined ( ZBIT )
/*********************************************************************
 * @fn      processTimerEvt
 *
 * @brief   Process the timer event to simulate OTA traffic normally driven by
 *          the Location PC App.
 *
 * @param   none
 *
 * @return  none
 */
static void processTimerEvt( void )
{
  static uint8 state = 0;

  const uint16 delay1 = 1000;  // Delay after broadcast request for node info.
  const uint16 delay2 = 3000;  // Delay after unicast request for position.

  uint16 delay = delay1;
  afAddrType_t dstAddr;
  cId_t cId;

  dstAddr.endPoint = LOCATION_ENDPOINT;
  dstAddr.addrMode = afAddrBroadcast;
  dstAddr.addr.shortAddr = NWK_BROADCAST_SHORTADDR;

  switch ( state )
  {
  case 0:
    cId = LOCATION_BLINDNODE_REQUEST_CONFIG;
    break;

  case 1:
    cId = LOCATION_REFNODE_REQUEST_CONFIG;
    break;

  case 2:
    cId = LOCATION_BLINDNODE_REQUEST;
    delay = delay2;
    dstAddr.addrMode = afAddr16Bit;
    dstAddr.addr.shortAddr = blindNodes[0].addr;
    dstAddr.endPoint = blindNodes[0].ep;
    break;

  default:
    break;
  }

  if ( afStatus_SUCCESS == AF_DataRequest( &dstAddr, (endPointDesc_t *)&epDesc,
                                            cId, 0, NULL, &LocDongle_TransID,
                                            AF_TX_OPTIONS_NONE,
                                            AF_DEFAULT_RADIUS ) )
  {
    if ( ++state >= 3 )
    {
      state = 0;
    }
  }

  osal_start_timerEx( LocDongle_TaskID, DONGLE_TIMER_EVT, delay );
}
#endif

//#undef ZBIT  //ggg
/*********************************************************************
*********************************************************************/
