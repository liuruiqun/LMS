/**************************************************************************************************
  Filename:       BlindNode.c
  Revised:        $Date: 2007-10-27 17:16:54 -0700 (Sat, 27 Oct 2007) $
  Revision:       $Revision: 15793 $

  Description:    Blind Node for the Z-Stack Location Profile.


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
  PROVIDED �AS IS� WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
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
#include "OSAL_NV.h"
#include "MTEL.h"
#include "AF.h"
#include "ZDApp.h"

#include "OnBoard.h"
#include "hal_key.h"
#if defined ( LCD_SUPPORTED )
#include "hal_lcd.h"
#endif

#include "LocationProfile.h"
#include "BlindNode.h"
#include "LocationEngine.h"
#include "SampleApp.h"

#if !defined ( CC2431 )
  #error    // Must be defined
#endif

/*********************************************************************
 * CONSTANTS
 */

#define BLINDNODE_MAX_REF_NODES    20
#define BLINDNODE_MIN_REF_NODES     3

#define BN_TIME_INCR              100   // Milliseconds

#define BLINDNODE_FIND_DELAY       20   // BN_TIME_INCR
#define BLINDNODE_WAIT_TIMEOUT      2   // BN_TIME_INCR

#define BLINDNODE_BLAST_DELAY      20   // Milliseconds
#define BLINDNODE_BLAST_COUNT       8

/* The Min/Max coming from the MAC may change, so try to accomodate this by
 * defining the "BlindNode" min/max based on the relative values of the MAC and
 * the Location Engine min/max.
 * The file mac_radio_defs.h was not meant to be included by any files
 * outside of low-level MAC files, per the MAC team - so re-define thier
 * #defines here.
 */
#define _ED_RF_POWER_MIN_DBM  -81  // TBD - why not directly access MAC defs?
#define _ED_RF_POWER_MAX_DBM   10  // TBD - why not directly access MAC defs?
#define _ED_RF_POWER_DELTA   (_ED_RF_POWER_MAX_DBM - _ED_RF_POWER_MIN_DBM)
#define _MAC_SPEC_ED_MAX      255  // TBD - why not directly access MAC defs?

/* Location Engine requires RSSI as dBm limited to -95 dBm to -40 dBm.
 * Weaker than -95 dBm must not be used.
 * Stronger than -40 dBm must be truncated and then passed as -40 dBm.
 * Values must be passed as absolute value: 40 strong to 95 weak.
 */
#define LOC_ENGINE_MIN_DBM  -95
#define LOC_ENGINE_MAX_DBM  -40

#if ( LOC_ENGINE_MIN_DBM < _ED_RF_POWER_MIN_DBM )
  #define BLINDNODE_MIN_DBM  _ED_RF_POWER_MIN_DBM
#else
  #define BLINDNODE_MIN_DBM  LOC_ENGINE_MIN_DBM
#endif

#define BLINDNODE_MIN_RSSI  (_MAC_SPEC_ED_MAX * (BLINDNODE_MIN_DBM - _ED_RF_POWER_MIN_DBM)) / \
                                                                     _ED_RF_POWER_DELTA

#if ( LOC_ENGINE_MAX_DBM < _ED_RF_POWER_MAX_DBM )
  #define BLINDNODE_MAX_DBM  LOC_ENGINE_MAX_DBM
#else
  #define BLINDNODE_MAX_DBM  _ED_RF_POWER_MAX_DBM
#endif

#define BLINDNODE_MAX_RSSI  (_MAC_SPEC_ED_MAX * (BLINDNODE_MAX_DBM - _ED_RF_POWER_MIN_DBM)) / \
                                                                     _ED_RF_POWER_DELTA

#define BLINDNODE_CONV_RSSI( lqi ) \
 st ( \
  if ( lqi <= BLINDNODE_MIN_RSSI ) \
    lqi = 0; \
  else if ( lqi >= BLINDNODE_MAX_RSSI ) \
    lqi = -LOC_ENGINE_MAX_DBM; \
  else \
    lqi = -BLINDNODE_MIN_DBM - ((uint8)(((uint16)(lqi) * _ED_RF_POWER_DELTA) / _MAC_SPEC_ED_MAX)); \
 )

// [ Loc = (Loc * (N-1) + New-Calc) / N ]  Set to 1 for no filtering.
#define BLINDNODE_FILTER      2
// If newly calculated pos differs by more than M meters, flush filter and 
// restart. 10m * 4 = 40
#define BLINDNODE_FLUSH      40

/*********************************************************************
 * TYPEDEFS
 */

typedef struct
{
  uint16 timeout;       // In 100 msec increments - time to collect ref node responses
  uint16 cycle;         // In 100 msec increments - For auto mode
  uint16 dstAddr;       // For auto mode
  uint8  dstEp;         // For auto mode
  uint8  mode;
  uint8  minRefNodes;
  LocDevCfg_t loc;
} BN_Cfg_t;

typedef enum
{
  eBnIdle,
  eBnBlastOut,
  eBnBlastIn,
  eBnBlastOff
} eBN_States_t;

typedef struct
{
  uint16 x;
  uint16 y;
  uint16 addr;
  uint8  ep;
  uint8  rssi;
} RefNode_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

uint8 BlindNode_TaskID;

/*********************************************************************
 * LOCAL VARIABLES
 */

static const cId_t BlindNode_InputClusterList[] =
{
  LOCATION_XY_RSSI_REQUEST,
  LOCATION_XY_RSSI_RESPONSE,
  LOCATION_BLINDNODE_FIND_REQUEST,
  LOCATION_BLINDNODE_CONFIG,
  LOCATION_BLINDNODE_REQUEST_CONFIG
};

static const cId_t BlindNode_OutputClusterList[] =
{
  LOCATION_XY_RSSI_REQUEST,
  LOCATION_XY_RSSI_RESPONSE,
  LOCATION_BLINDNODE_FIND_RESPONSE,
  LOCATION_BLINDNODE_CONFIG,

  LOCATION_RSSI_BLAST
};

static const SimpleDescriptionFormat_t BlindNode_SimpleDesc =
{
  LOCATION_BLINDNODE_ENDPOINT,
  LOCATION_PROFID,
  LOCATION_BLINDNODE_DEVICEID,
  LOCATION_DEVICE_VERSION,
  LOCATION_FLAGS,
  sizeof(BlindNode_InputClusterList),
  (cId_t *)BlindNode_InputClusterList,
  sizeof(BlindNode_OutputClusterList),
  (cId_t *)BlindNode_OutputClusterList
};

static const endPointDesc_t epDesc =
{
  LOCATION_BLINDNODE_ENDPOINT,
  &BlindNode_TaskID,
  (SimpleDescriptionFormat_t *)&BlindNode_SimpleDesc,
  noLatencyReqs
};

static BN_Cfg_t config;
static eBN_States_t state;
static RefNode_t refNodes[BLINDNODE_MAX_REF_NODES];
static uint8 transId;

static afAddrType_t defAddr;

#if defined( BN_DISPLAY_TEST ) && defined( MT_TASK )
  #define PRINTBUFSIZE 80
  uint8 printBuf[PRINTBUFSIZE];
#endif

static uint8 blastCnt;
static uint8 rspCnt;   // Count of unique XY_RSSI_RESPONSE messages after blast.

static uint8 rspMsg[LOCATION_XY_RSSI_LEN];

static uint32 xOld, yOld;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void processMSGCmd( afIncomingMSGPacket_t *pckt );
static void parseConfig( uint8 *msg );
static afStatus_t sendRsp( void );
static afStatus_t sendConfig( afAddrType_t *dstAddr );
static void finishCollection( void );
static void calcOffsets( RefNode_t *ref, uint16 *xOff, uint16 *yOff );
static RefNode_t *findBestRSSI( RefNode_t *ref );
static uint8 sortNodes( RefNode_t *ref );
static void setLogicals( LocRefNode_t *loc, RefNode_t *ref,
                                               uint16 xOff, uint16 yOff );

static void startBlast( void );

/*********************************************************************
 * @fn      BlindNode_Init
 *
 * @brief   Initialization function for the Generic App Task.
 *
 * @param   task_id - the ID assigned by OSAL.
 *
 * @return  none
 */
void BlindNode_Init( uint8 task_id )
{
  BlindNode_TaskID = task_id;

  state = eBnIdle;

  config.loc.param_a = LOC_DEFAULT_A;
  config.loc.param_n = LOC_DEFAULT_N;
  config.mode = LOC_DEFAULT_MODE;
  config.timeout = BLINDNODE_WAIT_TIMEOUT * BN_TIME_INCR;
  config.cycle = (uint16)((uint16)BLINDNODE_FIND_DELAY * BN_TIME_INCR);
  config.dstAddr = 0;
  config.dstEp = LOCATION_DONGLE_ENDPOINT;
  config.minRefNodes = BLINDNODE_MIN_REF_NODES;

  if ( ZSUCCESS == osal_nv_item_init( LOC_NV_BLINDNODE_CONFIG,
                                      sizeof(BN_Cfg_t), &config ) )
  {
    osal_nv_read( LOC_NV_BLINDNODE_CONFIG, 0, sizeof( BN_Cfg_t ), &config );
  }
  else
  {
    osal_nv_write( LOC_NV_BLINDNODE_CONFIG, 0, sizeof( BN_Cfg_t ), &config );
  }

  afRegister( (endPointDesc_t *)&epDesc );
  //RegisterForKeys( BlindNode_TaskID );

#if defined ( LCD_SUPPORTED )
  HalLcdWriteString( "Location-Blind", HAL_LCD_LINE_1 );
#endif

  defAddr.addrMode = afAddrBroadcast;
  defAddr.addr.shortAddr = NWK_BROADCAST_SHORTADDR_DEVALL;
  defAddr.endPoint = LOCATION_DONGLE_ENDPOINT;
  
  if ( (config.mode == NODE_MODE_AUTO) && (config.cycle > 0) )
  {
    osal_start_timerEx( BlindNode_TaskID, BLINDNODE_FIND_EVT, config.cycle );
  }
}

/*********************************************************************
 * @fn      BlindNode_ProcessEvent
 *
 * @brief   Generic Application Task event processor.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.
 *
 * @return  none
 */
uint16 BlindNode_ProcessEvent( uint8 task_id, uint16 events )
{
  if ( events & SYS_EVENT_MSG )
  {
    afIncomingMSGPacket_t *MSGpkt;
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( BlindNode_TaskID );

    while( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
      case KEY_CHANGE:
        //handleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
        break;

      case AF_DATA_CONFIRM_CMD:
#if !defined( RTR_NWK )
        {
        // This message is received as a confirmation of a data packet sent.
        // The status is of ZStatus_t type [defined in ZComDef.h]
        afDataConfirm_t *afDataConfirm = (afDataConfirm_t *)MSGpkt;
        
        if ( afDataConfirm->hdr.status != ZSuccess )
        {
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
          else  // Some other error -- Do something.
          {
          }
        }
        }
#endif
        break;

      case AF_INCOMING_MSG_CMD:
        processMSGCmd( MSGpkt );
        break;

      case ZDO_STATE_CHANGE:
#if defined( RTR_NWK )
        NLME_PermitJoiningRequest( 0 );
#endif
        /* Broadcast the X,Y location for any passive listeners in order to
         * register this node.
         */
        osal_start_timerEx( BlindNode_TaskID, BLINDNODE_FIND_EVT,
                                              BLINDNODE_FIND_DELAY );
        break;

      default:
        break;
      }

      osal_msg_deallocate( (uint8 *)MSGpkt );
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( BlindNode_TaskID );
    }

    return ( events ^ SYS_EVENT_MSG );  // Return unprocessed events.
  }

  if ( events & BLINDNODE_BLAST_EVT )
  {
    if ( blastCnt == 0 )
    {
      state = eBnBlastOff;
      finishCollection();
    }
    else
    {
      afAddrType_t dstAddr;
      uint8 stat, delay;

      dstAddr.addrMode = afAddrBroadcast;
      dstAddr.addr.shortAddr = NWK_BROADCAST_SHORTADDR_DEVALL;
      dstAddr.endPoint = LOCATION_REFNODE_ENDPOINT;

      if ( --blastCnt == 0 )
      {
        stat = AF_DataRequest( &dstAddr, (endPointDesc_t *)&epDesc,
                                LOCATION_XY_RSSI_REQUEST, 0, NULL,
                               &transId, AF_SKIP_ROUTING, 1 );

        state = eBnBlastIn;
        delay = config.timeout;
      }
      else
      {
        stat = AF_DataRequest( &dstAddr, (endPointDesc_t *)&epDesc,
                                LOCATION_RSSI_BLAST, 0, NULL,
                               &transId, AF_SKIP_ROUTING, 1 );
        delay = BLINDNODE_BLAST_DELAY;
      }
      if ( stat != afStatus_SUCCESS )
      {
        blastCnt++;
      }
      osal_start_timerEx( BlindNode_TaskID, BLINDNODE_BLAST_EVT, delay );
    }

    return ( events ^ BLINDNODE_BLAST_EVT );
  }

  if ( events & BLINDNODE_FIND_EVT )
  {
    BlindNode_FindRequest();

    return ( events ^ BLINDNODE_FIND_EVT );
  }

  if ( events & BLINDNODE_WAIT_EVT )
  {
    SampleApp_Sleep( TRUE );

    return ( events ^ BLINDNODE_WAIT_EVT );
  }

  return 0;  // Discard unknown events.
}

/*********************************************************************
 * @fn      LocationHandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   keys - bit field for key events. Valid entries:
 *
 * @return  none
 */
void LocationHandleKeys( uint8 keys )
{
  if ( keys & HAL_KEY_SW_1 )
  {
  }
  if ( keys & HAL_KEY_SW_2 )
  {
  }
  if ( keys & HAL_KEY_SW_3 )
  {
    osal_set_event( BlindNode_TaskID, BLINDNODE_FIND_EVT );
  }
  if ( keys & HAL_KEY_SW_4 )
  {
    if ( state == eBnIdle )
    {
      startBlast();
    }
  }
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
    case LOCATION_XY_RSSI_REQUEST:
      rspMsg[LOCATION_XY_RSSI_RSSI_IDX] = pkt->LinkQuality;
      pkt->srcAddr.addrMode = afAddr16Bit;
      (void)AF_DataRequest( &pkt->srcAddr, (endPointDesc_t *)&epDesc,
                             LOCATION_XY_RSSI_RESPONSE, LOCATION_XY_RSSI_LEN,
                             rspMsg, &transId, 0, AF_DEFAULT_RADIUS );
      break;

    case LOCATION_BLINDNODE_FIND_REQUEST:
      if ( state == eBnIdle )
      {
        defAddr.addrMode = afAddr16Bit;
        defAddr.addr.shortAddr = pkt->srcAddr.addr.shortAddr;
        defAddr.endPoint = pkt->srcAddr.endPoint;
        startBlast();
      }
      break;

    case LOCATION_BLINDNODE_CONFIG:
      if ( state == eBnIdle )
      {
        parseConfig( pkt->cmd.Data );
      }
      break;

    case LOCATION_BLINDNODE_REQUEST_CONFIG:
      sendConfig( &(pkt->srcAddr) );
      break;

    case LOCATION_XY_RSSI_RESPONSE:
      if ( state == eBnBlastIn )
      {
        uint8 idx;

        for ( idx = 0; idx < BLINDNODE_MAX_REF_NODES; idx++ )
        {
          if ( (refNodes[idx].addr == INVALID_NODE_ADDR) ||
               (refNodes[idx].addr == pkt->srcAddr.addr.shortAddr) )
          {
            refNodes[idx].addr = pkt->srcAddr.addr.shortAddr;
            refNodes[idx].ep = pkt->srcAddr.endPoint;
            refNodes[idx].x = BUILD_UINT16(
                                  pkt->cmd.Data[LOCATION_XY_RSSI_X_LO_IDX],
                                  pkt->cmd.Data[LOCATION_XY_RSSI_X_HI_IDX] );
            refNodes[idx].y = BUILD_UINT16(
                                  pkt->cmd.Data[LOCATION_XY_RSSI_Y_LO_IDX],
                                  pkt->cmd.Data[LOCATION_XY_RSSI_Y_HI_IDX] );
            refNodes[idx].rssi =  pkt->cmd.Data[LOCATION_XY_RSSI_RSSI_IDX];

            if ( rspCnt <= idx )
            {
              rspCnt = idx+1;
            }
            break;
          }
        }
      }
      break;

    default:
      break;
  }
}

/*********************************************************************
 * @fn      parseConfig
 *
 * @brief   Parse message and populate the configuration struct
 *
 * @param   none
 *
 * @return  none
 */
static void parseConfig( uint8 *msg )
{
  config.loc.param_a = msg[BLINDNODE_CONFIG_A_IDX];
  config.loc.param_n = msg[BLINDNODE_CONFIG_N_IDX];
  if ( (msg[BLINDNODE_CONFIG_MODE_IDX] == NODE_MODE_POLLED) ||
       (msg[BLINDNODE_CONFIG_MODE_IDX] == NODE_MODE_AUTO) )
  {
    config.mode = msg[BLINDNODE_CONFIG_MODE_IDX];
  }

  config.timeout = BN_TIME_INCR * BUILD_UINT16(
                                  msg[BLINDNODE_CONFIG_COLLECT_TIME_LO_IDX],
                                  msg[BLINDNODE_CONFIG_COLLECT_TIME_HI_IDX] );

  config.cycle = BN_TIME_INCR * BUILD_UINT16(
       msg[BLINDNODE_CONFIG_CYCLE_LO_IDX], msg[BLINDNODE_CONFIG_CYCLE_HI_IDX] );
  config.dstAddr = BUILD_UINT16( msg[BLINDNODE_CONFIG_REPORT_SADDR_LO_IDX],
                                 msg[BLINDNODE_CONFIG_REPORT_SADDR_HI_IDX] );
  config.dstEp = msg[BLINDNODE_CONFIG_REPORT_EP_IDX];
  config.minRefNodes = msg[BLINDNODE_CONFIG_MIN_REFNODES_IDX];

  osal_nv_write( LOC_NV_BLINDNODE_CONFIG, 0, sizeof( BN_Cfg_t ), &config );

  if ( config.mode == NODE_MODE_AUTO )
  {
    osal_set_event( BlindNode_TaskID, BLINDNODE_FIND_EVT );
  }
}

/*********************************************************************
 * @fn      sendRsp
 *
 * @brief   Build and send the response message.
 *
 * @param   None.
 *
 * @return  status from call to AF_DataRequest().
 */
static afStatus_t sendRsp( void )
{
  uint8 msg[BLINDNODE_RESPONSE_LEN];
  LocRefNode_t locNodes[BLINDNODE_MAX_REF_NODES];
  uint16 xOff, yOff;
  uint8 idx, cnt = 0;

  for ( idx = 0; idx < rspCnt; idx++ )
  {
    BLINDNODE_CONV_RSSI( refNodes[idx].rssi );
    if ( refNodes[idx].rssi != 0 )
    {
      cnt++;
    }
  }

  if ( cnt >= config.minRefNodes )
  {
    msg[BLINDNODE_RESPONSE_STATUS_IDX] = BLINDNODE_RSP_STATUS_SUCCESS;

    // Sort the ref nodes by RSSI in order to pass the best 16 to loc engine.
    rspCnt = sortNodes( refNodes );

    calcOffsets( refNodes, &xOff, &yOff );

    // Convert to logical coordinates.
    setLogicals( locNodes, refNodes, xOff, yOff );

    // Run the location calculation
    locationCalculatePosition( locNodes, &(config.loc) );

    // Convert results to real coordinates and average over several samples.
    xOff += config.loc.x;
    yOff += config.loc.y;

    if ( ((xOff > xOld) && ((xOff - xOld) > BLINDNODE_FLUSH))  ||
         ((xOff < xOld) && ((xOld - xOff) > BLINDNODE_FLUSH))  ||
         ((yOff > yOld) && ((yOff - yOld) > BLINDNODE_FLUSH))  ||
         ((yOff < yOld) && ((yOld - yOff) > BLINDNODE_FLUSH)) )
    {
      xOld = xOff;
      yOld = yOff;
    }
    else
    {
      xOld = ((xOld * (BLINDNODE_FILTER-1)) + xOff) / BLINDNODE_FILTER;
      yOld = ((yOld * (BLINDNODE_FILTER-1)) + yOff) / BLINDNODE_FILTER;
    }
    
    xOff = (uint16)xOld;
    yOff = (uint16)yOld;
  }
  else
  {
    msg[BLINDNODE_RESPONSE_STATUS_IDX] = BLINDNODE_RSP_STATUS_NOT_ENOUGH_REFNODES;

    xOff = (uint16)xOld;
    yOff = (uint16)yOld;
  }
  rspMsg[REFNODE_CONFIG_X_LO_IDX] =
  msg[BLINDNODE_RESPONSE_X_LO_IDX] = LO_UINT16( xOff );
  rspMsg[REFNODE_CONFIG_X_HI_IDX] =
  msg[BLINDNODE_RESPONSE_X_HI_IDX] = HI_UINT16( xOff );
  rspMsg[REFNODE_CONFIG_Y_LO_IDX] =
  msg[BLINDNODE_RESPONSE_Y_LO_IDX] = LO_UINT16( yOff );
  rspMsg[REFNODE_CONFIG_Y_HI_IDX] =
  msg[BLINDNODE_RESPONSE_Y_HI_IDX] = HI_UINT16( yOff );

  msg[BLINDNODE_RESPONSE_NUMREFNODES_IDX] = cnt;

  if ( rspCnt != 0 )
  {
    msg[BLINDNODE_RESPONSE_REF_SHORTADDR_LO_IDX] = LO_UINT16( refNodes->addr );
    msg[BLINDNODE_RESPONSE_REF_SHORTADDR_HI_IDX] = HI_UINT16( refNodes->addr );
    msg[BLINDNODE_RESPONSE_REF_X_LO_IDX] = LO_UINT16( refNodes->x );
    msg[BLINDNODE_RESPONSE_REF_X_HI_IDX] = HI_UINT16( refNodes->x );
    msg[BLINDNODE_RESPONSE_REF_Y_LO_IDX] = LO_UINT16( refNodes->y );
    msg[BLINDNODE_RESPONSE_REF_Y_HI_IDX] = HI_UINT16( refNodes->y );
    msg[BLINDNODE_RESPONSE_REF_RSSI] = refNodes->rssi;
  }
  else
  {
    msg[BLINDNODE_RESPONSE_REF_SHORTADDR_LO_IDX] = LO_UINT16( INVALID_NODE_ADDR );
    msg[BLINDNODE_RESPONSE_REF_SHORTADDR_HI_IDX] = HI_UINT16( INVALID_NODE_ADDR );
    msg[BLINDNODE_RESPONSE_REF_X_LO_IDX] = LO_UINT16( LOC_DEFAULT_X_Y );
    msg[BLINDNODE_RESPONSE_REF_X_HI_IDX] = HI_UINT16( LOC_DEFAULT_X_Y );
    msg[BLINDNODE_RESPONSE_REF_Y_LO_IDX] = LO_UINT16( LOC_DEFAULT_X_Y );
    msg[BLINDNODE_RESPONSE_REF_Y_HI_IDX] = HI_UINT16( LOC_DEFAULT_X_Y );
    msg[BLINDNODE_RESPONSE_REF_RSSI] = 0xFF;
  }

#if defined ( LCD_SUPPORTED )
  {
    uint8 hex[] = "0123456789ABCDEF";
    //extern uint8 *_ltoa( uint32 l, uint8 * buf, uint8 radix );
    uint8 buf[32];
    uint16 tmp;

    osal_memset( buf, ' ', 16 );
    tmp = xOff/4;
    if ( tmp > 99 )
      tmp = 99;
    if ( tmp < 10 )
      _ltoa( tmp, buf+1, 10 );
    else
      _ltoa( tmp, buf, 10 );
    buf[2] = ',';
    tmp = yOff/4;
    if ( tmp > 99 )
      tmp = 99;
    if ( tmp < 10 )
      _ltoa( tmp, buf+4, 10 );
    else
      _ltoa( tmp, buf+3, 10 );
    buf[5] = ' ';
    if ( cnt > 16 )
      buf[6] = 'X';
    else
      buf[6] = hex[cnt];

#if defined ( LCD_SUPPORTED )
    HalLcdWriteString( (char*)buf, HAL_LCD_LINE_1 );
#endif

    osal_memset( buf, ' ', 16 );
    if ( rspCnt != 0 )
    {
      tmp = refNodes->x/4;
      if ( tmp > 99 )
        tmp = 99;
      if ( tmp < 10 )
        _ltoa( tmp, buf+1, 10 );
      else
        _ltoa( tmp, buf, 10 );
      buf[2] = ',';
      tmp = refNodes->y/4;
      if ( tmp > 99 )
        tmp = 99;
      if ( tmp < 10 )
        _ltoa( tmp, buf+4, 10 );
      else
        _ltoa( tmp, buf+3, 10 );
      buf[5] = ' ';
      tmp = refNodes->addr;
      buf[9] = hex[tmp & 0xf];
      tmp /= 16;
      buf[8] = hex[tmp & 0xf];
      tmp /= 16;
      buf[7] = hex[tmp & 0xf];
      tmp /= 16;
      buf[6] = hex[tmp & 0xf];
    }

#if defined ( LCD_SUPPORTED )
    HalLcdWriteString( (char*)buf, HAL_LCD_LINE_2 );
#endif
  }
#endif

  osal_start_timerEx( BlindNode_TaskID, BLINDNODE_WAIT_EVT, 1000 );

  return ( AF_DataRequest( &defAddr, (endPointDesc_t *)&epDesc,
                           LOCATION_BLINDNODE_FIND_RESPONSE,
                           BLINDNODE_RESPONSE_LEN, msg,
                           &transId, 0, AF_DEFAULT_RADIUS ) );
}

/*********************************************************************
 * @fn      sendConfig
 *
 * @brief   Build and send the response message.
 *
 * @param   dstAddr - where to send
 *
 * @return  status from call to AF_DataRequest().
 */
static afStatus_t sendConfig( afAddrType_t *dstAddr )
{
  uint8 msg[BLINDNODE_CONFIG_LEN];

  msg[BLINDNODE_CONFIG_A_IDX] = config.loc.param_a;
  msg[BLINDNODE_CONFIG_N_IDX] = config.loc.param_n;
  msg[BLINDNODE_CONFIG_MODE_IDX] = config.mode;
  msg[BLINDNODE_CONFIG_COLLECT_TIME_LO_IDX] = LO_UINT16(
                                       config.timeout / BN_TIME_INCR );
  msg[BLINDNODE_CONFIG_COLLECT_TIME_HI_IDX] = HI_UINT16(
                                       config.timeout / BN_TIME_INCR );
  msg[BLINDNODE_CONFIG_CYCLE_LO_IDX] = LO_UINT16( config.cycle / BN_TIME_INCR );
  msg[BLINDNODE_CONFIG_CYCLE_HI_IDX] = HI_UINT16( config.cycle / BN_TIME_INCR );
  msg[BLINDNODE_CONFIG_REPORT_SADDR_LO_IDX] = LO_UINT16( config.dstAddr );
  msg[BLINDNODE_CONFIG_REPORT_SADDR_HI_IDX] = HI_UINT16( config.dstAddr );
  msg[BLINDNODE_CONFIG_REPORT_EP_IDX] = config.dstEp;
  msg[BLINDNODE_CONFIG_MIN_REFNODES_IDX] = config.minRefNodes;

  return ( AF_DataRequest( dstAddr, (endPointDesc_t *)&epDesc,
                           LOCATION_BLINDNODE_CONFIG,
                           BLINDNODE_CONFIG_LEN, msg,
                           &transId, 0, AF_DEFAULT_RADIUS ) );
}

/*********************************************************************
 * @fn      finishCollection
 *
 * @brief   Sends the next Bind Node Response message
 *
 * @param   none
 *
 * @return  none
 */
static void finishCollection( void )
{
  if ((ZDO_Config_Node_Descriptor.CapabilityFlags & CAPINFO_RCVR_ON_IDLE) == 0)
  {
    uint8 x;

    // Turn the receiver back off while idle
    x = false;
    ZMacSetReq( ZMacRxOnIdle, &x );
  }

  // Send the Blind node response
  sendRsp();

  if ( config.mode == NODE_MODE_AUTO )
  {
    // set up next auto response
    osal_start_timerEx( BlindNode_TaskID, BLINDNODE_FIND_EVT, config.cycle );
    defAddr.addrMode = afAddr16Bit;
    defAddr.addr.shortAddr = config.dstAddr;
    defAddr.endPoint = config.dstEp;
  }

  state = eBnIdle;
}

/*********************************************************************
 * @fn      calcOffsets
 *
 * @brief   Calculates the XY offsets.
 *
 * INPUTS:
 * @param   ref - Array of reference nodes, pre-sorted on RSSI, best to worst.
 *
 * OUTPUTS:
 * @param   xOff - pointer to X offset
 * param    yOff - pointer to Y offset
 *
 * @return  none
 */
static void calcOffsets( RefNode_t *ref, uint16 *xOff, uint16 *yOff )
{
  RefNode_t *rnP = ref;
  uint16 xMax = 0;
  uint16 yMax = 0;
  uint8 idx;

  for ( idx = 0; idx < rspCnt; idx++ )
  {
    if ( xMax < rnP->x )
    {
      xMax = rnP->x;
    }
    if ( yMax < rnP->y )
    {
      yMax = rnP->y;
    }

    rnP++;
  }

  // No need for conversion.
  if ( (xMax < 256) && (yMax < 256) )
  {
    *xOff = *yOff = 0;
  }
  else
  {
    // Force reference node with the best RSSI to sit at logical (32,32).
    *xOff = (ref->x & 0xFFFC) - 128;
    *yOff = (ref->y & 0xFFFC) - 128;
  }
}

/*********************************************************************
 * @fn      findBestRSSI
 *
 * @brief   Returns the node with the best rssi.
 *
 * INPUTS:  ref - Array of reference nodes w/ RSSI values converted for
 *          location engine from 40 (strong) to 95 (weak) & zero = too week.
 *
 * @return  Ref node entry with best RSSI.
 */
static RefNode_t *findBestRSSI( RefNode_t *ref )
{
  RefNode_t *bestRef = NULL;
  uint8 idx;

  for ( idx = 0; idx < rspCnt; idx++ )
  {
    if ( (ref->addr != INVALID_NODE_ADDR) && (ref->rssi != 0) )
    {
      if ( (bestRef == NULL) || (bestRef->rssi > ref->rssi) )
      {
        bestRef = ref;
      }
    }
    ref++;
  }

  return bestRef;
}

/*********************************************************************
 * @fn      sortNodes
 *
 * @brief   Sorts the nodes into a list with the best to least RSSI
 *
 * INPUTS:  ref - Array of reference nodes w/ RSSI values converted for
 *          location engine from 40 (strong) to 95 (weak) & zero = too week.
 *
 * OUTPUTS: none
 *
 * @return  Count of non-zero RSSI entries.
 */
static uint8 sortNodes( RefNode_t *ref )
{
  RefNode_t *workNodes;
  uint8 idx;

  workNodes = osal_mem_alloc( sizeof( RefNode_t ) * rspCnt );

  if ( workNodes == NULL )
  {
    return 0;
  }

  osal_memcpy( workNodes, ref, sizeof( RefNode_t ) * rspCnt );

  for ( idx = 0; idx < rspCnt; idx++ )
  {
    RefNode_t *node = findBestRSSI( workNodes );

    if ( node == NULL )
    {
      break;
    }
    else
    {
      osal_memcpy( ref, node, sizeof( RefNode_t ) );
      node->addr = INVALID_NODE_ADDR;
    }

    ref++;
  }

  osal_mem_free( workNodes );

  return idx;
}

/*********************************************************************
 * @fn      setLogicals
 *
 * @brief   Sets the reference node's logical coordinates & RSSI for the
 *          required number of inputs to the location engine.
 *
 * INPUTS:
 * @param   ref - array of reference nodes
 * @param   offsetX - X offset used to make logical numbers
 * param    offsetY - Y offset used to make logical numbers
 *
 * @return  none
 */
static void setLogicals( LocRefNode_t *loc, RefNode_t *ref,
                                               uint16 xOff, uint16 yOff )
{
  // Rev-B Chip have LocEng Ver 1.0 w/ cap=8, Rev-C have LocEng Ver 2.0 w/ 16.
  const uint8 stop = ( ( CHVER == 0x01 ) ? LOC_ENGINE_NODE_CAPACITY_REVB :
                                          LOC_ENGINE_NODE_CAPACITY_REVC);
  uint16 xTmp, yTmp;
  uint8 idx;

  // Set the logical coordinates
  for ( idx = 0; idx < rspCnt; idx++ )
  {
    xTmp = ref->x - xOff;
    yTmp = ref->y - yOff;

    if ( (xTmp < 256) && (yTmp < 256) )
    {
      loc->x = (uint8)xTmp;
      loc->y = (uint8)yTmp;
      loc->rssi = ref->rssi;
    }
    else
    {
      // Out of bounds, so feed zero to location engine.
      loc->x = loc->y = loc->rssi = 0;
    }

    loc++;
    ref++;
  }
  
  for ( ; idx < stop; idx++ )
  {
    // Feed zero to location engine to meet the required number of inputs.
    loc->x = loc->y = 0;
    loc->rssi = 0;
    loc++;
  }
}

#if defined ( ZBIT )
/*********************************************************************
 * @fn      locationCalculatePosition
 *
 * @brief   Simulate the position calculation of this device.
 *
 * @param   ref - input - Reference Nodes
 * @param   node - input/output - This node's information
 *
 * @return  none
 */
void locationCalculatePosition( LocRefNode_t *ref, LocDevCfg_t *node )
{

  return;
}
#endif

/*********************************************************************
 * @fn      startBlast
 *
 * @brief   Start a sequence of blasts and calculate position.
 *
 * @param   none
 *
 * @return  none
 */
static void startBlast( void )
{
  uint8 idx;
  afAddrType_t dstAddr;
  dstAddr.addrMode = afAddrBroadcast;
  dstAddr.addr.shortAddr = NWK_BROADCAST_SHORTADDR_DEVALL;
  dstAddr.endPoint = LOCATION_REFNODE_ENDPOINT;

  if ((ZDO_Config_Node_Descriptor.CapabilityFlags & CAPINFO_RCVR_ON_IDLE) == 0)
  {
    // Turn the receiver on while idle - temporarily.
    idx = true;
    ZMacSetReq( ZMacRxOnIdle, &idx );
  }
  SampleApp_Sleep( FALSE );

  for ( idx = 0; idx < BLINDNODE_MAX_REF_NODES; idx++ )
  {
    refNodes[idx].addr = INVALID_NODE_ADDR;
  }

  (void)AF_DataRequest( &dstAddr, (endPointDesc_t *)&epDesc,
                         LOCATION_RSSI_BLAST, 0,
                         NULL, &transId,
                         AF_SKIP_ROUTING, 1 );
  rspCnt = 0;
  blastCnt = BLINDNODE_BLAST_COUNT;
  state = eBnBlastOut;
  osal_start_timerEx( BlindNode_TaskID, BLINDNODE_BLAST_EVT, BLINDNODE_BLAST_DELAY );
}

/*********************************************************************
 * @fn      BlindNode_FindRequest
 *
 * @brief   Start a sequence of blasts and calculate position.
 *
 * @param   none
 *
 * @return  none
 */
void BlindNode_FindRequest( void )
{
  if ( state == eBnIdle )
  {
    startBlast();
  }
}

/*********************************************************************
*********************************************************************/
