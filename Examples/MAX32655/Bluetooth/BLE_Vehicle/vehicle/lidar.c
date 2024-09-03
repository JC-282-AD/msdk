/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Lidar Configuration
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
#include <string.h>
#include "tmr.h"
#include "wsf_types.h"
#include "wsf_trace.h"
#include "wsf_timer.h"
#include "mxc_device.h"
#include "led.h"
#include "board.h"
#include "mxc_delay.h"
#include "uart.h"
#include "nvic_table.h"
#include "dma.h"
#include "bcv_api.h"
#include "pal_uart.h"

/***** Definitions *****/
#define UART_BAUD 115200
#define BUFF_SIZE 9
#define LIDAR_UART MXC_UART3
#define LIDAR_UART_IRQ UART3_IRQn
#define TIMER_DELAY 100

/***** Globals *****/
uint8_t RxData[BUFF_SIZE];
PalUartConfig_t liuart;
volatile int READ_FLAG;
volatile int WRITE_FLAG;
wsfHandlerId_t lidarTimerHandlerId;
wsfTimer_t lidarTimer;



/***** Functions *****/


void readCallback(void)
{
    READ_FLAG = 1;
    if (RxData[1] == 0x59 && RxData[0] == 0x59)
    {
        volatile uint16_t distance_cm = 0;
        distance_cm = RxData[3] << 8 | RxData[2];
        BcvSendLid((uint8_t *) &distance_cm);
    }
}

void writeCallback(void)
{
    WRITE_FLAG = 1;
    PalUartReadData(PAL_UART_ID_CHCI, RxData, 9);
}

void lidarTimerHandlerCB(wsfEventMask_t event, wsfMsgHdr_t *pMsg)
{
    if (WRITE_FLAG == 1 && READ_FLAG == 1)
    {
        uint8_t dframe[4] = {0x5a, 0x04,0x04,0x62};
        WRITE_FLAG = 0;
        READ_FLAG = 0;
        PalUartWriteData(PAL_UART_ID_CHCI, dframe, 4);
    }

     WsfTimerStartMs(&lidarTimer, TIMER_DELAY);

}


void LidarInit(void)
{
    READ_FLAG = 1;
    WRITE_FLAG = 1;

    memset(RxData, 0x0, BUFF_SIZE);

    NVIC_ClearPendingIRQ(LIDAR_UART_IRQ);
    NVIC_EnableIRQ(LIDAR_UART_IRQ);
    
    liuart.baud = UART_BAUD;
    liuart.hwFlow = 0;
    liuart.rdCback = readCallback;
    liuart.wrCback = writeCallback;
    
    PalUartInit(PAL_UART_ID_CHCI, &liuart);
    
    lidarTimerHandlerId = WsfOsSetNextHandler(lidarTimerHandlerCB);
    lidarTimer.handlerId = lidarTimerHandlerId;

    // somewhere you have to start the timer
    WsfTimerStartMs(&lidarTimer, TIMER_DELAY);
    return;
}

