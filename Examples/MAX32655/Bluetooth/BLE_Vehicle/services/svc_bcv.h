/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Maxim Custom service server.
 *
 *  Copyright (c) 2012-2018 Arm Ltd. All Rights Reserved.
 *
 *  Copyright (c) 2022-2023 Analog Devices, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/*************************************************************************************************/

#ifndef EXAMPLES_MAX32655_BLUETOOTH_BLE_BCV_SERVICES_SVC_BCV_H_
#define EXAMPLES_MAX32655_BLUETOOTH_BLE_BCV_SERVICES_SVC_BCV_H_

#include "wsf_types.h"
#include "att_api.h"
#include "util/bstream.h"
#include "svc_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \addtogroup Bcv
 *  \{ */
/**************************************************************************************************
  Macros
**************************************************************************************************/
/*BCV service UUID*/
#define ATT_UUID_BCV_SERVICE                                                                  \
    0xBE, 0xC5, 0xD1, 0x24, 0x99, 0x33, 0xC6, 0x87, 0x85, 0x41, 0xD9, 0x31, 0x7D, 0x56, 0xFC, \
        0x85 /*!< \brief Test Service UUID*/

/* BCV service GATT characteristic UUIDs*/
#define ATT_UUID_BCV_ACC \
    0xBE, 0xC5, 0xD1, 0x24, 0x99, 0x33, 0xC6, 0x87, 0x85, 0x41, 0xD9, 0x31, 0x7E, 0x56, 0xFC, 0x85
#define ATT_UUID_BCV_UP \
    0xBE, 0xC5, 0xD1, 0x24, 0x99, 0x33, 0xC6, 0x87, 0x85, 0x41, 0xD9, 0x31, 0x7F, 0x56, 0xFC, 0x85
#define ATT_UUID_BCV_DOWN \
    0xBE, 0xC5, 0xD1, 0x24, 0x99, 0x33, 0xC6, 0x87, 0x85, 0x41, 0xD9, 0x31, 0x80, 0x56, 0xFC, 0x85
#define ATT_UUID_BCV_LEFT \
    0xBE, 0xC5, 0xD1, 0x24, 0x99, 0x33, 0xC6, 0x87, 0x85, 0x41, 0xD9, 0x31, 0x81, 0x56, 0xFC, 0x85
#define ATT_UUID_BCV_RIGHT \
    0xBE, 0xC5, 0xD1, 0x24, 0x99, 0x33, 0xC6, 0x87, 0x85, 0x41, 0xD9, 0x31, 0x82, 0x56, 0xFC, 0x85
#define ATT_UUID_BCV_STOP \
    0xBE, 0xC5, 0xD1, 0x24, 0x99, 0x33, 0xC6, 0x87, 0x85, 0x41, 0xD9, 0x31, 0x83, 0x56, 0xFC, 0x85
#define ATT_UUID_BCV_LID \
    0xBE, 0xC5, 0xD1, 0x24, 0x99, 0x33, 0xC6, 0x87, 0x85, 0x41, 0xD9, 0x31, 0x84, 0x56, 0xFC, 0x85

/**************************************************************************************************
 Handle Ranges
**************************************************************************************************/

/** \name Maxim custom Service Handles
 *
 */
/**@{*/
#define BCV_START_HDL 0x1500 /*!< \brief Start handle. */
#define BCV_END_HDL (BCV_MAX_HDL - 1) /*!< \brief End handle. */

/**************************************************************************************************
 Handles
**************************************************************************************************/

/*! \brief Maxim custom Service Handles */
enum {
    BCV_SVC_HDL = BCV_START_HDL, /*!< \brief Maxim custom service declaration */
    BCV_ACC_CH_HDL, /*!< \brief Acc characteristic */
    BCV_ACC_HDL, /*!< \brief Acc*/
    BCV_ACC_CH_CCC_HDL, /*!< \brief Acc CCCD*/
    BCV_LID_CH_HDL, /*!< \brief Acc characteristic */
    BCV_LID_HDL, /*!< \brief Acc*/
    BCV_LID_CH_CCC_HDL, /*!< \brief Acc CCCD*/
    BCV_UP_CH_HDL, /*!< \brief Up characteristic */
    BCV_UP_HDL, /*!< \brief Up*/
    BCV_DOWN_CH_HDL, /*!< \brief Down characteristic */
    BCV_DOWN_HDL, /*!< \brief Down*/
    BCV_LEFT_CH_HDL, /*!< \brief Left characteristic */
    BCV_LEFT_HDL, /*!< \brief Left*/
    BCV_RIGHT_CH_HDL, /*!< \brief Right characteristic */
    BCV_RIGHT_HDL, /*!< \brief Right*/
    BCV_STOP_CH_HDL, /*!< \brief Stop characteristic */
    BCV_STOP_HDL, /*!< \brief Stop*/
    BCV_MAX_HDL /*!< \brief Maximum handle. */
};
/**@}*/

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \brief  Add the services to the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcBcvAddGroup(void);

/*************************************************************************************************/
/*!
 *  \brief  Remove the services from the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcBcvRemoveGroup(void);

/*************************************************************************************************/
/*!
 *  \brief  Register callbacks for the service.
 *
 *  \param  readCback   Read callback function.
 *  \param  writeCback  Write callback function.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcBcvCbackRegister(attsReadCback_t readCback, attsWriteCback_t writeCback);

/*! \} */ /* TEST_SERVICE */

#ifdef __cplusplus
};
#endif

#endif // EXAMPLES_MAX32655_BLUETOOTH_BLE_BCV_SERVICES_SVC_BCV_H_
