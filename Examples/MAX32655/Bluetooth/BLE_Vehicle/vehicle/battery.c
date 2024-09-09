/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  battery Configuration
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
#include <stdio.h>
#include <stdint.h>
#include <mxc.h>
#include "tmr.h"
#include "wsf_types.h"
#include "wsf_trace.h"
#include "wsf_timer.h"
#include "adc.h"
#include "bas/bas_api.h"
#include "svc_ch.h"
#include "svc_batt.h"
#include "app_api.h"
#include "vehicle.h"

#define BAT_PERIOD 100
#define FULL_12V 0x30C
#define EMPTY_5V 0x144

static int32_t adc_val;
uint8_t overflow;
wsfHandlerId_t batTimerHandlerId;
wsfTimer_t batTimer;

void batTimerHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg)
{
    if (getVehicleStatus() == STOP)
    {
        adc_val = MXC_ADC_StartConversion(MXC_ADC_CH_0);
        overflow = (adc_val == E_OVERFLOW ? 1 : 0);
        // Calculate the battery level precent
        int32_t temp = (adc_val - EMPTY_5V)  > 0 ? ((adc_val - EMPTY_5V) * 100) : 0;
        uint8_t level = temp / (FULL_12V - EMPTY_5V);
        if (level > 100) level = 100;

        // Send notification
        dmConnId_t connId = AppConnIsOpen(); /*Getting connected */
        if (connId != DM_CONN_ID_NONE) {
            AttsHandleValueNtf(connId, BATT_LVL_HDL, 1, &level);
        }
    }
    WsfTimerStartMs(&batTimer, BAT_PERIOD);
}

void batInit(void)
{
    if (MXC_ADC_Init() != E_NO_ERROR) {
        printf("Error Bad Parameter\n");

        while (1) {}
    }

    /* Set up LIMIT0 to monitor high and low trip points */
    MXC_ADC_SetMonitorChannel(MXC_ADC_MONITOR_0, MXC_ADC_CH_0);
    MXC_ADC_SetMonitorHighThreshold(MXC_ADC_MONITOR_0, 0x300);
    MXC_ADC_SetMonitorLowThreshold(MXC_ADC_MONITOR_0, 0x25);
    MXC_ADC_EnableMonitor(MXC_ADC_MONITOR_0);

    batTimerHandlerId = WsfOsSetNextHandler(batTimerHandler);
    batTimer.handlerId = batTimerHandlerId;

    // somewhere you have to start the timer
    WsfTimerStartMs(&batTimer, BAT_PERIOD);

}