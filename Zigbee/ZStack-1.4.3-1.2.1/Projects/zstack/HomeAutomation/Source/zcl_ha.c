/**************************************************************************************************
  Filename:       zcl_ha.c
  Revised:        $Date: 2007-11-16 10:50:03 -0800 (Fri, 16 Nov 2007) $
  Revision:       $Revision: 15940 $

  Description:    Describe the purpose and contents of the file.


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
#include "OSAL.h"

#include "zcl.h"
#include "zcl_general.h"
#include "zcl_closures.h"
#include "zcl_HVAC.h"
#include "zcl_ss.h"
#include "zcl_ms.h"
#include "zcl_lighting.h"
#include "zcl_ha.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * Cluster Conversion List - table used to convert between real and
 * logical cluster IDs.
 */
CONST zclConvertClusterRec_t ha_ClusterConvertTable[] =
{
    // ZCL: General Cluster Library
  { ZCL_HA_CLUSTER_ID_GEN_BASIC,                             ZCL_GEN_LOGICAL_CLUSTER_ID_BASIC },
  { ZCL_HA_CLUSTER_ID_GEN_POWER_CFG,                         ZCL_GEN_LOGICAL_CLUSTER_ID_POWER_CFG },
  { ZCL_HA_CLUSTER_ID_GEN_DEVICE_TEMP_CONFIG,                ZCL_GEN_LOGICAL_CLUSTER_ID_DEV_TEMP_CFG },
  { ZCL_HA_CLUSTER_ID_GEN_IDENTIFY,                          ZCL_GEN_LOGICAL_CLUSTER_ID_IDENTIFY },
  { ZCL_HA_CLUSTER_ID_GEN_GROUPS,                            ZCL_GEN_LOGICAL_CLUSTER_ID_GROUPS },
  { ZCL_HA_CLUSTER_ID_GEN_SCENES,                            ZCL_GEN_LOGICAL_CLUSTER_ID_SCENES },
  { ZCL_HA_CLUSTER_ID_GEN_ON_OFF,                            ZCL_GEN_LOGICAL_CLUSTER_ID_ON_OFF },
  { ZCL_HA_CLUSTER_ID_GEN_ON_OFF_SWITCH_CONFIG,              ZCL_GEN_LOGICAL_CLUSTER_ID_ON_OFF_SWITCH_CFG },
  { ZCL_HA_CLUSTER_ID_GEN_LEVEL_CONTROL,                     ZCL_GEN_LOGICAL_CLUSTER_ID_LEVEL_CTRL },
  { ZCL_HA_CLUSTER_ID_GEN_ALARMS,                            ZCL_GEN_LOGICAL_CLUSTER_ID_ALARMS },
  { ZCL_HA_CLUSTER_ID_GEN_TIME,                              ZCL_GEN_LOGICAL_CLUSTER_ID_TIME },
  { ZCL_HA_CLUSTER_ID_GEN_LOCATION,                          ZCL_GEN_LOGICAL_CLUSTER_ID_LOCATION },
    // ZCL Closures Cluster Library
  { ZCL_HA_CLUSTER_ID_CLOSURES_SHADE_CONFIG,	             ZCL_CLOSURES_LOGICAL_CLUSTER_ID_SHADE_CONFIG },
    // ZCL HVAC Cluster Library
  { ZCL_HA_CLUSTER_ID_HVAC_PUMP_CONFIG_CONTROL,	             ZCL_HVAC_LOGICAL_CLUSTER_ID_PUMP_CONFIG_CONTROL },
  { ZCL_HA_CLUSTER_ID_HAVC_THERMOSTAT,                       ZCL_HVAC_LOGICAL_CLUSTER_ID_THERMOSTAT },
  { ZCL_HA_CLUSTER_ID_HAVC_FAN_CONTROL,                      ZCL_HVAC_LOGICAL_CLUSTER_ID_FAN_CONTROL },
  { ZCL_HA_CLUSTER_ID_HAVC_DIHUMIDIFICATION_CONTROL,         ZCL_HVAC_LOGICAL_CLUSTER_ID_DIHUMIDIFICATION_CONTROL },
  { ZCL_HA_CLUSTER_ID_HAVC_USER_INTERFACE_CONFIG,            ZCL_HVAC_LOGICAL_CLUSTER_ID_USER_INTERFACE_CONFIG },
    // ZCL Lighting Cluster Library
  { ZCL_HA_CLUSTER_ID_LIGHTING_COLOR_CONTROL,	             ZCL_LIGHTING_LOGICAL_CLUSTER_ID_COLOR_CONTROL },
  { ZCL_HA_CLUSTER_ID_LIGHTING_BALLAST_CONFIG,	             ZCL_LIGHTING_LOGICAL_CLUSTER_ID_BALLAST_CONFIG },
    // ZCL MS Cluster Library
  { ZCL_HA_CLUSTER_ID_MS_ILLUMINANCE_MEASUREMENT,            ZCL_MS_LOGICAL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT },
  { ZCL_HA_CLUSTER_ID_MS_ILLUMINANCE_LEVEL_SENSING_CONFIG,   ZCL_MS_LOGICAL_CLUSTER_ID_ILLUMINANCE_LEVEL_SENSING_CFG },
  { ZCL_HA_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT,            ZCL_MS_LOGICAL_CLUSTER_ID_TEMPERATURE_MEASUREMENT },
  { ZCL_HA_CLUSTER_ID_MS_PRESSURE_MEASUREMENT,               ZCL_MS_LOGICAL_CLUSTER_ID_PRESSURE_MEASUREMENT },
  { ZCL_HA_CLUSTER_ID_MS_FLOW_MEASUREMENT,                   ZCL_MS_LOGICAL_CLUSTER_ID_FLOW_MEASUREMENT },
  { ZCL_HA_CLUSTER_ID_MS_RELATIVE_HUMIDITY,                  ZCL_MS_LOGICAL_CLUSTER_ID_RELATIVE_HUMIDITY },
  { ZCL_HA_CLUSTER_ID_MS_OCCUPANCY_SENSING,                  ZCL_MS_LOGICAL_CLUSTER_ID_OCCUPANCY_SENSING },
    // ZCL SS Cluster Library
  { ZCL_HA_CLUSTER_ID_SS_IAS_ZONE,                           ZCL_SS_LOGICAL_CLUSTER_ID_IAS_ZONE },	
  { ZCL_HA_CLUSTER_ID_SS_IAS_ACE,                            ZCL_SS_LOGICAL_CLUSTER_ID_IAS_ACE },
  { ZCL_HA_CLUSTER_ID_SS_IAS_WD,                             ZCL_SS_LOGICAL_CLUSTER_ID_IAS_WD },
    /*** Add clusters as the libraries are developed. ***/
};

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static uint8 zclHA_RegisteredClusterList = FALSE;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      zclHA_Init
 *
 * @brief   Register the Simple descriptor with the HA profile.
 *          This function also registers the profile's cluster
 *          conversion table.
 *
 * @param   simpleDesc
 *
 * @return  none
 */
void zclHA_Init( SimpleDescriptionFormat_t *simpleDesc )
{
  endPointDesc_t *epDesc;

  // Set up the Real Cluster ID to Logical Cluster ID conversion
  if ( !zclHA_RegisteredClusterList )
  {
    zcl_registerClusterConvertTable( ZCL_HA_PROFILE_ID,
       (sizeof( ha_ClusterConvertTable ) / sizeof( zclConvertClusterRec_t )) ,
           (GENERIC zclConvertClusterRec_t *)ha_ClusterConvertTable );
    zclHA_RegisteredClusterList = TRUE;
  }

  // Register the application's endpoint descriptor
  //  - This memory is allocated and never freed.
  epDesc = osal_mem_alloc( sizeof ( endPointDesc_t ) );
  if ( epDesc )
  {
    // Fill out the endpoint description.
    epDesc->endPoint = simpleDesc->EndPoint;
    epDesc->task_id = &zcl_TaskID;   // all messages get sent to ZCL first
    epDesc->simpleDesc = simpleDesc;
    epDesc->latencyReq = noLatencyReqs;

    // Register the endpoint description with the AF
    afRegister( epDesc );
  }
}

/*********************************************************************
*********************************************************************/
