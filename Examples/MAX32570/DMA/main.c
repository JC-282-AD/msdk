/******************************************************************************
 * Copyright (C) 2022 Maxim Integrated Products, Inc., All rights Reserved.
 * 
 * This software is protected by copyright laws of the United States and
 * of foreign countries. This material may also be protected by patent laws
 * and technology transfer regulations of the United States and of foreign
 * countries. This software is furnished under a license agreement and/or a
 * nondisclosure agreement and may only be used or reproduced in accordance
 * with the terms of those agreements. Dissemination of this information to
 * any party or parties not specified in the license agreement and/or
 * nondisclosure agreement is expressly prohibited.
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 ******************************************************************************/

/**
 * @file        main.c
 * @brief       DMA Example
 * @details     This runs through two DMA examples, first being memory-to-memory,
 *              second being a transfer with reload and callback.
 */

/***** Includes *****/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <MAX32xxx.h>

/***** Definitions *****/

#define MAX_CHANNEL 16
#define MAX_SIZE    64

/***** Globals *****/
int mychannel     = -1;
int fail          = 0;
volatile int flag = 0;

/***** Functions *****/
void my_int_func(int a, int b)
{
    int flags;
    flags = MXC_DMA_ChannelGetFlags(mychannel);
    MXC_DMA_ChannelClearFlags(mychannel, flags);
}

void memCpyComplete(void* dest)
{
    flag++;
}

void DMA0_IRQHandler()
{
    MXC_DMA_Handler();
}

void example1(void)
{
    printf("Transfer from memory to memory.\n");

    int retval;
    int i = 0;

    //Initialize data before transfer
    uint8_t *srcdata, *dstdata;
    srcdata = (uint8_t*)malloc(MAX_SIZE);
    dstdata = (uint8_t*)malloc(MAX_SIZE);

    for (i = 0; i < MAX_SIZE; ++i) {
        srcdata[i] = i;
        dstdata[i] = 0;
    }

    retval = MXC_DMA_Init();

    if (retval != E_NO_ERROR) {
        printf("Failed MXC_DMA_Init().\n");

        while (1)
            ;
    }

    flag = 0;
    MXC_DMA_MemCpy(dstdata, srcdata, MAX_SIZE, memCpyComplete);

    while (flag == 0)
        ;

    // //Demo of acquiring channels
    // for (i = 0; i < MAX_CHANNEL; ++i) {
    //     retval = MXC_DMA_AcquireChannel();
    //     if (retval == E_BAD_STATE) {
    //         printf("Failed to acquire channel: %d\n", i);
    //         while(1);
    //     }
    //     channels[i] = retval;
    // }

    // //Only keeping the first channel [0] for use
    // for (i = 1; i < MAX_CHANNEL; ++i) {
    //     retval = MXC_DMA_ReleaseChannel(channels[i]);
    //     if (retval != E_NO_ERROR) {
    //         printf("Failed to release channel %d\n", i);
    //         while(1);
    //     }
    // }

    // mxc_dma_srcdst_t firstTransfer;
    // firstTransfer.ch = channels[0];
    // firstTransfer.source = srcdata;
    // firstTransfer.dest = dstdata;
    // firstTransfer.len = MAX_SIZE;

    // mxc_dma_config_t config;
    // config.ch = channels[0];
    // config.reqsel = MXC_DMA_REQUEST_MEMTOMEM;
    // config.srcwd = MXC_DMA_WIDTH_WORD;
    // config.dstwd = MXC_DMA_WIDTH_WORD;
    // config.srcinc_en = 1;
    // config.dstinc_en = 1;

    // retval = MXC_DMA_ConfigChannel(config, firstTransfer);

    // if (retval != E_NO_ERROR) {
    //     printf("Failed to config channel\n");
    //     while(1);
    // }

    // printf("Starting transfer\n");

    // if (MXC_DMA_Start(channels[0]) != E_NO_ERROR) {
    //     printf("Failed to start.\n");
    //     while(1);
    // }

    //Validate
    if (memcmp(srcdata, dstdata, MAX_SIZE) != 0) {
        printf("Data mismatch.\n");

        while (1)
            ;

        fail += 1;
    } else {
        printf("Data verified.\n");
    }

    // if (MXC_DMA_ReleaseChannel(channels[0]) != E_NO_ERROR) {
    //     printf("Failed to release channel 0\n");
    //     while(1);
    // }

    free(srcdata);
    free(dstdata);

    return;
}

void example2(void)
{
    printf("\nTransfer with Reload and Callback.\n");

    int i, retval;

    //Init data
    uint8_t *srcdata, *dstdata, *srcdata2, *dstdata2;
    srcdata  = (uint8_t*)malloc(MAX_SIZE);
    dstdata  = (uint8_t*)malloc(MAX_SIZE);
    srcdata2 = (uint8_t*)malloc(MAX_SIZE);
    dstdata2 = (uint8_t*)malloc(MAX_SIZE);

    for (i = 0; i < MAX_SIZE; ++i) {
        srcdata[i] = i;
        dstdata[i] = 0;
        //Different set of data
        srcdata2[i] = MAX_SIZE - 1 - i;
        dstdata2[i] = 0;
    }

    NVIC_EnableIRQ(DMA0_IRQn);
    __enable_irq();
    MXC_DMA_Init();
    mychannel = MXC_DMA_AcquireChannel();

    mxc_dma_srcdst_t firstTransfer;
    firstTransfer.ch     = mychannel;
    firstTransfer.source = srcdata;
    firstTransfer.dest   = dstdata;
    firstTransfer.len    = MAX_SIZE;

    mxc_dma_config_t config;
    config.ch        = mychannel;
    config.reqsel    = MXC_DMA_REQUEST_MEMTOMEM;
    config.srcwd     = MXC_DMA_WIDTH_WORD;
    config.dstwd     = MXC_DMA_WIDTH_WORD;
    config.srcinc_en = 1;
    config.dstinc_en = 1;

    mxc_dma_adv_config_t advConfig;
    advConfig.ch         = mychannel;
    advConfig.prio       = MXC_DMA_PRIO_HIGH;
    advConfig.reqwait_en = 0;
    advConfig.tosel      = MXC_DMA_TIMEOUT_4_CLK;
    advConfig.pssel      = MXC_DMA_PRESCALE_DISABLE;
    advConfig.burst_size = 32;

    MXC_DMA_ConfigChannel(config, firstTransfer);
    MXC_DMA_AdvConfigChannel(advConfig);

    mxc_dma_srcdst_t secondTransfer;
    secondTransfer.ch     = mychannel;
    secondTransfer.source = srcdata2;
    secondTransfer.dest   = dstdata2;
    secondTransfer.len    = MAX_SIZE;

    MXC_DMA_SetSrcDst(firstTransfer);

    retval = MXC_DMA_SetSrcReload(secondTransfer);

    if (retval != E_NO_ERROR) {
        printf("Failed MXC_DMA_SetReload.\n");
    }

    MXC_DMA_SetCallback(mychannel, my_int_func);
    MXC_DMA_EnableInt(mychannel);
    MXC_DMA_Start(mychannel);

    // Validate
    if (memcmp(srcdata, dstdata, MAX_SIZE) != 0 || memcmp(srcdata2, dstdata2, MAX_SIZE) != 0) {
        printf("Data mismatch.\n");

        while (1)
            ;

        fail += 1;
    } else {
        printf("Data verified.\n");
    }

    if (MXC_DMA_ReleaseChannel(mychannel) != E_NO_ERROR) {
        printf("Failed to release channel 0\n");

        while (1)
            ;
    }

    free(srcdata);
    free(dstdata);
    free(srcdata2);
    free(dstdata2);

    return;
}

// *****************************************************************************
int main(void)
{
    printf("***** DMA Example *****\n");

    NVIC_EnableIRQ(DMA0_IRQn);
    example1();
    example2();

    printf("\n");

    if (fail == 0) {
        printf("Example Succeeded\n");
    } else {
        printf("Example failed\n");

        while (1)
            ;
    }

    while (1)
        ;

    return 0;
}
