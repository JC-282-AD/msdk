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

/**************************************************************************************************
  Macros
**************************************************************************************************/

/***** Definitions *****/
#define ALL_PT 0x0C

#define ContPulse_PT_L 2
#define ContPulse_PT_R 3
#define ContPulse_L_Pin 16
#define ContPulse_R_Pin 17

#define MXC_GPIO_PORT_OUT MXC_GPIO0
#define MXC_GPIO_PIN_OUT MXC_GPIO_PIN_24

#define MXC_GPIO_PORT_INTERRUPT_STATUS MXC_GPIO0
#define MXC_GPIO_PIN_INTERRUPT_STATUS MXC_GPIO_PIN_25

#define LED1_Pin 24
#define LED2_Pin 25


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


    switch (handle) {
    case BCV_UP_HDL:
        if (on)
        {
            printf("on forward!\n");
            forward();
        }
        else
        {
            printf("off forward!\n");
            stop();
        }
        break;
    case BCV_DOWN_HDL:
        if (on)
        {
            printf("on backward!\n");
            backward();
        }
        else
        {
            printf("off backward!\n");
            stop();
        }
        break;
    case BCV_LEFT_HDL:
        if (on)
        {
            printf("on left!\n");
            left();
   
        }
        else
        {
            printf("off left!\n");
            turn_back();
        }
        break;
    case BCV_RIGHT_HDL:
        if (on)
        {
            printf("on right!\n");
            right();
        }
        else
        {
            printf("off right!\n");
            turn_back();
        }
        break;
    case BCV_STOP_HDL:
        if (on)
        {
            printf("on stop!\n");
        }
        else
        {
            printf("off stop!\n");
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
void BcvSetFeatures(uint8_t features)
{
    AttsSetAttr(BCV_BUTTON_HDL, sizeof(features),
                (uint8_t *)&features); /*Setting bcvButtonVal characteristic value */
    dmConnId_t connId = AppConnIsOpen(); /*Getting connected */
    if (connId != DM_CONN_ID_NONE) {
        AttsHandleValueNtf(connId, BCV_BUTTON_HDL, sizeof(features),
                           (uint8_t *)&features); /*Send notify */
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
    NVIC_EnableIRQ(PT_IRQn); //enabled default interrupt handler
    MXC_PT_EnableStopInt(ALL_PT); //enabled interrupts for all PT
    MXC_PT_Init(MXC_PT_CLK_DIV1); //initialize pulse trains

    vehicle_init();
}

void PT_IRQHandler(void)
{
    MXC_PT_ClearStopFlags(ALL_PT);
}

