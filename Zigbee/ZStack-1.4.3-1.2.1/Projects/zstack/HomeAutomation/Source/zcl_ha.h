/**************************************************************************************************
  Filename:       zcl_ha.h
  Revised:        $Date: 2007-11-16 10:50:03 -0800 (Fri, 16 Nov 2007) $
  Revision:       $Revision: 15940 $

  Description:    This file contains the Zigbee Cluster Library: Home
                  Automation Profile definitions.


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

#ifndef ZCL_HA_H
#define ZCL_HA_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */
// Zigbee Home Automation Profile Identification
#define ZCL_HA_PROFILE_ID                                       0x0104

// General Clusters
#define ZCL_HA_CLUSTER_ID_GEN_BASIC                             0x0000
#define ZCL_HA_CLUSTER_ID_GEN_POWER_CFG                         0x0001
#define ZCL_HA_CLUSTER_ID_GEN_DEVICE_TEMP_CONFIG                0x0002
#define ZCL_HA_CLUSTER_ID_GEN_IDENTIFY                          0x0003
#define ZCL_HA_CLUSTER_ID_GEN_GROUPS                            0x0004
#define ZCL_HA_CLUSTER_ID_GEN_SCENES                            0x0005
#define ZCL_HA_CLUSTER_ID_GEN_ON_OFF                            0x0006
#define ZCL_HA_CLUSTER_ID_GEN_ON_OFF_SWITCH_CONFIG              0x0007
#define ZCL_HA_CLUSTER_ID_GEN_LEVEL_CONTROL                     0x0008
#define ZCL_HA_CLUSTER_ID_GEN_ALARMS                            0x0009
#define ZCL_HA_CLUSTER_ID_GEN_TIME                              0x000A
#define ZCL_HA_CLUSTER_ID_GEN_LOCATION                          0x000B

// Closures Clusters
#define ZCL_HA_CLUSTER_ID_CLOSURES_SHADE_CONFIG                 0x0100

// HVAC Clusters
#define ZCL_HA_CLUSTER_ID_HVAC_PUMP_CONFIG_CONTROL              0x0200
#define ZCL_HA_CLUSTER_ID_HAVC_THERMOSTAT                       0x0201
#define ZCL_HA_CLUSTER_ID_HAVC_FAN_CONTROL                      0x0202
#define ZCL_HA_CLUSTER_ID_HAVC_DIHUMIDIFICATION_CONTROL         0x0203
#define ZCL_HA_CLUSTER_ID_HAVC_USER_INTERFACE_CONFIG            0x0204

// Lighting Clusters
#define ZCL_HA_CLUSTER_ID_LIGHTING_COLOR_CONTROL                0x0300
#define ZCL_HA_CLUSTER_ID_LIGHTING_BALLAST_CONFIG               0x0301
  
// Measurement and Sensing Clusters
#define ZCL_HA_CLUSTER_ID_MS_ILLUMINANCE_MEASUREMENT            0x0400
#define ZCL_HA_CLUSTER_ID_MS_ILLUMINANCE_LEVEL_SENSING_CONFIG   0x0401
#define ZCL_HA_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT            0x0402
#define ZCL_HA_CLUSTER_ID_MS_PRESSURE_MEASUREMENT               0x0403
#define ZCL_HA_CLUSTER_ID_MS_FLOW_MEASUREMENT                   0x0404
#define ZCL_HA_CLUSTER_ID_MS_RELATIVE_HUMIDITY                  0x0405
#define ZCL_HA_CLUSTER_ID_MS_OCCUPANCY_SENSING                  0x0406

// Security and Safety (SS) Clusters
#define ZCL_HA_CLUSTER_ID_SS_IAS_ZONE                           0x0500
#define ZCL_HA_CLUSTER_ID_SS_IAS_ACE                            0x0501
#define ZCL_HA_CLUSTER_ID_SS_IAS_WD                             0x0502

// Protocol Interface
#define ZCL_HA_CLUSTER_ID_PI_BACNET_INTERFACE                   0x0600
 
// Generic Device IDs
#define ZCL_HA_DEVICEID_ON_OFF_SWITCH                           0x0000
#define ZCL_HA_DEVICEID_LEVEL_CONTROL_SWITCH                    0x0001
#define ZCL_HA_DEVICEID_ON_OFF_OUTPUT                           0x0002
#define ZCL_HA_DEVICEID_LEVEL_CONTROLLABLE_OUTPUT               0x0003
#define ZCL_HA_DEVICEID_SCENE_SELECTOR                          0x0004
#define ZCL_HA_DEVICEID_CONFIGURATIOPN_TOOL                     0x0005
#define ZCL_HA_DEVICEID_REMOTE_CONTROL                          0x0006
#define ZCL_HA_DEVICEID_COMBINED_INETRFACE                      0x0007
#define ZCL_HA_DEVICEID_RANGE_EXTENDER                          0x0008
#define ZCL_HA_DEVICEID_MAINS_POWER_OUTLET                      0x0009
  // temp: nnl
#define ZCL_HA_DEVICEID_TEST_DEVICE                             0x00FF

// Lighting Device IDs
#define ZCL_HA_DEVICEID_ON_OFF_LIGHT                            0x0100
#define ZCL_HA_DEVICEID_DIMMABLE_LIGHT                          0x0101
#define ZCL_HA_DEVICEID_COLORED_DIMMABLE_LIGHT                  0x0102
#define ZCL_HA_DEVICEID_ON_OFF_LIGHT_SWITCH                     0x0103
#define ZCL_HA_DEVICEID_DIMMER_SWITCH                           0x0104
#define ZCL_HA_DEVICEID_COLOR_DIMMER_SWITCH                     0x0105
#define ZCL_HA_DEVICEID_LIGHT_SENSOR                            0x0106
#define ZCL_HA_DEVICEID_OCCUPANCY_SENSOR                        0x0107

// Closures Device IDs
#define ZCL_HA_DEVICEID_SHADE                                   0x0200
#define ZCL_HA_DEVICEID_SHADE_CONTROLLER                        0x0201

// HVAC Device IDs
#define ZCL_HA_DEVICEID_HEATING_COOLING_UNIT                    0x0300
#define ZCL_HA_DEVICEID_THERMOSTAT                              0x0301
#define ZCL_HA_DEVICEID_TEMPERATURE_SENSOR                      0x0302
#define ZCL_HA_DEVICEID_PUMP                                    0x0303
#define ZCL_HA_DEVICEID_PUMP_CONTROLLER                         0x0304
#define ZCL_HA_DEVICEID_PRESSURE_SENSOR                         0x0305
#define ZCL_HA_DEVICEID_FLOW_SENSOR                             0x0306

// Intruder Alarm Systems (IAS) Device IDs
#define ZCL_HA_DEVICEID_IAS_CONTROL_INDICATING_EQUIPMENT        0x0400
#define ZCL_HA_DEVICEID_IAS_ANCILLARY_CONTROL_EQUIPMENT         0x0401
#define ZCL_HA_DEVICEID_IAS_ZONE                                0x0402
#define ZCL_HA_DEVICEID_IAS_WARNING_DEVICE                      0x0403

/*********************************************************************
 * MACROS
 */
#define ZCL_HA_CLUSTER_ID_GEN( id )      ( /* id >= ZCL_HA_CLUSTER_ID_GEN_BASIC &&*/ \
                                           id <= ZCL_HA_CLUSTER_ID_GEN_LOCATION )
#define ZCL_HA_CLUSTER_ID_CLOSURES( id ) ( id == ZCL_HA_CLUSTER_ID_CLOSURES_SHADE_CONFIG )
#define ZCL_HA_CLUSTER_ID_HVAC( id )     ( id >= ZCL_HA_CLUSTER_ID_HVAC_PUMP_CONFIG_CONTROL && \
                                           id <= ZCL_HA_CLUSTER_ID_HAVC_USER_INTERFACE_CONFIG )
#define ZCL_HA_CLUSTER_ID_LIGHTING( id ) ( id >= ZCL_HA_CLUSTER_ID_LIGHTING_COLOR_CONTROL && \
                                           id <= ZCL_HA_CLUSTER_ID_LIGHTING_BALLAST_CONFIG )
#define ZCL_HA_CLUSTER_ID_MS( id )       ( id >= ZCL_HA_CLUSTER_ID_MS_ILLUMINANCE_MEASUREMENT && \
                                           id <= ZCL_HA_CLUSTER_ID_MS_OCCUPANCY_SENSING )
#define ZCL_HA_CLUSTER_ID_SS( id )       ( id >= ZCL_HA_CLUSTER_ID_SS_IAS_ZONE && \
                                           id <= ZCL_HA_CLUSTER_ID_SS_IAS_WD )
  
/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * FUNCTIONS
 */

 /*
  *  ZCL Home Automation Profile initialization function
  */
extern void zclHA_Init( SimpleDescriptionFormat_t *simpleDesc );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ZCL_HA_H */
