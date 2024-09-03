/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Maxim Custom service server.
 *
 *  Copyright (c) 2012-2019 Arm Ltd. All Rights Reserved.
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

#include <stdbool.h>
#include "bcv_api.h"
#include "app_ui.h"
#include "pal_led.h"
#include "wsf_trace.h"
#include "board.h"
#include "pt.h"
#include "vehicle.h"
#include "accelerometer.h"
#include "lidar.h"


/**************************************************************************************************
  Macros
**************************************************************************************************/

/**************************************************************************************************
  Local Variables
**************************************************************************************************/

/*************************************************************************************************/
/*!
 *  \brief  ATTS write callback for maxim custom service.  Use this function as a parameter
 *          to SvcBcvCbackRegister().
 *
 *  \return ATT status.
 */
/*************************************************************************************************/
uint8_t BcvWriteCback(dmConnId_t connId, uint16_t handle, uint8_t operation, uint16_t offset,
                      uint16_t len, uint8_t *pValue, attsAttr_t *pAttr)
{
    AttsSetAttr(handle, sizeof(*pValue), (uint8_t *)pValue);
    /* Turn LED on if non-zero value was written */
    bool on = *pValue != 0;

    // control the vehicle according to different key press
    switch (handle) {
    case BCV_UP_HDL:
        if (on)
        {
            forward();
        }
        else
        {
            slow();
        }
        break;
    case BCV_DOWN_HDL:
        if (on)
        {
            backward();
        }
        else
        {
            slow();
        }
        break;
    case BCV_LEFT_HDL:
        if (on)
        {
            left();
        }
        else
        {
            turn_back();
        }
        break;
    case BCV_RIGHT_HDL:
        if (on)
        {
            right();
        }
        else
        {
            turn_back();
        }
        break;
    case BCV_STOP_HDL:
        if (on)
        {
            stop();
        }
        else
        {
            stop();
        }
        break;
    }

    return ATT_SUCCESS;
}

/*************************************************************************************************/

/*!
 *  \brief  Setting characteristic value and send the button value to the peer device
 *
 *  \return None.
 */
/*************************************************************************************************/
void BcvSendAcc(uint8_t* features)
{
    AttsSetAttr(BCV_ACC_HDL, 10,
                (uint8_t *)features); /*Setting bcvAccVal characteristic value */
    dmConnId_t connId = AppConnIsOpen(); /*Getting connected */
    if (connId != DM_CONN_ID_NONE) {
        AttsHandleValueNtf(connId, BCV_ACC_HDL, 10,
                           (uint8_t *)features); /*Send notify */
    }
}

/*************************************************************************************************/

/*!
 *  \brief  Setting characteristic value and send the button value to the peer device
 *
 *  \return None.
 */
/*************************************************************************************************/
void BcvSendLid(uint8_t* features)
{
    AttsSetAttr(BCV_LID_HDL, 2,
                (uint8_t *)features); /*Setting bcvAccVal characteristic value */
    dmConnId_t connId = AppConnIsOpen(); /*Getting connected */
    if (connId != DM_CONN_ID_NONE) {
        
        AttsHandleValueNtf(connId, BCV_LID_HDL, 2,
                           (uint8_t *)features); /*Send notify */
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Initialize the bcv server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void BcvInit(void)
{
    /* De-init the PAL LEDs so we can control them here */
    PalLedDeInit();

    vehicle_init(); // Initialize the vehicle status
    AccInit(); // Initialize the accelerometer
    LidarInit(); // Initialize the lidar
}