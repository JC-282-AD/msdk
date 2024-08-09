/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Vehicle Configuration
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


#include "vehicle.h"
#include "mxc.h"
#include "pt.h"

/***** Definitions *****/
#define ALL_PT 0x0C

#define ContPulse_PT_L 2
#define ContPulse_PT_R 3
#define ContPulse_L_Pin 16
#define ContPulse_R_Pin 17


/**************************************************************************************************
  Local Variables
**************************************************************************************************/


static mxc_pt_cfg_t ptConfigL = {
    ContPulse_PT_L, // channel
    1000, //bps
    0x2AA, //pattern 1010101010 50% duty cycle
    10, //ptLength
    0, //loop
    0, // loopDelay
};

static mxc_pt_cfg_t ptConfigR = {
    ContPulse_PT_R, // channel
    1000, //bps
    0x2AA, //pattern 1010101010 50% duty cycle
    10, //ptLength
    0, //loop
    0, // loopDelay
};

static mxc_gpio_cfg_t left1 = {
    MXC_GPIO0, // port
    MXC_GPIO_PIN_24, // mask
    MXC_GPIO_FUNC_OUT, // pad
    MXC_GPIO_PAD_NONE, // func
    MXC_GPIO_VSSEL_VDDIO, // vssel
    MXC_GPIO_DRVSTR_0, // drvstr
};

static mxc_gpio_cfg_t left2 = {
    MXC_GPIO0, // port
    MXC_GPIO_PIN_25, // mask
    MXC_GPIO_FUNC_OUT, // pad
    MXC_GPIO_PAD_NONE, // func
    MXC_GPIO_VSSEL_VDDIO, // vssel
    MXC_GPIO_DRVSTR_0, // drvstr
};

static mxc_gpio_cfg_t right1 = {
    MXC_GPIO0, // port
    MXC_GPIO_PIN_18, // mask
    MXC_GPIO_FUNC_OUT, // pad
    MXC_GPIO_PAD_NONE, // func
    MXC_GPIO_VSSEL_VDDIO, // vssel
    MXC_GPIO_DRVSTR_0, // drvstr
};

static mxc_gpio_cfg_t right2 = {
    MXC_GPIO0, // port
    MXC_GPIO_PIN_19, // mask
    MXC_GPIO_FUNC_OUT, // pad
    MXC_GPIO_PAD_NONE, // func
    MXC_GPIO_VSSEL_VDDIO, // vssel
    MXC_GPIO_DRVSTR_0, // drvstr
};

    
struct vehicle_ctr {
    uint8_t speed;
    uint8_t state;
    uint8_t direction;
};

static struct vehicle_ctr myVehicle = {
    0,
    0,
    0,
};

/*************************************************************************************************/
void vehicle_init(void)
{
    MXC_GPIO_Config(&left1);
    MXC_GPIO_Config(&right1);
    MXC_GPIO_Config(&left2);
    MXC_GPIO_Config(&right2);
    stop();

     //setup PT configuration
    MXC_PT_Config(&ptConfigL);

    //start PT
    MXC_PT_Start(1 << ContPulse_PT_L);

    //setup PT configuration
    MXC_PT_Config(&ptConfigR);

    //start PT
    MXC_PT_Start(1 << ContPulse_PT_R);
}

// speed adjust
// void speed(void);

void left(void)
{
    //setup PT configuration
    ptConfigL.pattern = 0x88;
    ptConfigL.ptLength = 8;
    MXC_PT_Config(&ptConfigL);
    MXC_PT_Start(1 << ContPulse_PT_L);
}

void right(void)
{
    ptConfigR.pattern = 0x88;
    ptConfigR.ptLength = 8;
    MXC_PT_Config(&ptConfigR);
    MXC_PT_Start(1 << ContPulse_PT_R);
}

void turn_back(void)
{
    ptConfigL.pattern = 0x2AA;
    ptConfigL.ptLength = 10;
    MXC_PT_Config(&ptConfigL);
    MXC_PT_Start(1 << ContPulse_PT_L);
    ptConfigR.pattern = 0x2AA;;
    ptConfigR.ptLength = 10;
    MXC_PT_Config(&ptConfigR);
    MXC_PT_Start(1 << ContPulse_PT_R);
}

void stop(void)
{
    MXC_GPIO_OutClr(left1.port, left1.mask);
    MXC_GPIO_OutClr(left2.port, left2.mask);
    MXC_GPIO_OutClr(right1.port, right1.mask);
    MXC_GPIO_OutClr(right2.port, right2.mask);
}

void forward(void)
{
    MXC_GPIO_OutSet(left1.port, left1.mask);
    MXC_GPIO_OutClr(left2.port, left2.mask);
    MXC_GPIO_OutSet(right1.port, right1.mask);
    MXC_GPIO_OutClr(right2.port, right2.mask);
}

void backward(void)
{
    MXC_GPIO_OutClr(left1.port, left1.mask);
    MXC_GPIO_OutSet(left2.port, left2.mask);
    MXC_GPIO_OutClr(right1.port, right1.mask);
    MXC_GPIO_OutSet(right2.port, right2.mask);
}
