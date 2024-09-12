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
#include "tmr.h"
#include "wsf_types.h"
#include "wsf_trace.h"
#include "wsf_timer.h"
#include "accelerometer.h"

/***** Definitions *****/

#define ContPulse_PT_L 2
#define ContPulse_PT_R 3
#define ContPulse_L_Pin 16
#define ContPulse_R_Pin 17
#define SPEED_CHANGE_RATE 100 // (ms)
#define SPEED_CHANGE_VALUE 2
#define TURN_SPEED_PLUS 3
#define TURN_SPEED_DIFFERENCE 8
#define MIN_DUTY_CYCLE 30 // (%)
#define MAX_DUTY_CYCLE 64 // (%)

// This is the variable for unbalanced vehicle
// If the vehicle is super balanced and it won't naturally turn left or right
// This macro can be set to zero
// positive value for naturally turn left vehicle
// negative value for naturally turn right vehicle
// This value will be combined with duty cycle to balance the vehicle
#define VEHICLE_CAB (-1)

#define PWM_CLOCK_SOURCE MXC_TMR_APB_CLK 
#define FREQ 4000 // (Hz)

#define PWM_TIMER_LEFT MXC_TMR1 // must change PWM_PORT and PWM_PIN if changed
#define PWM_TIMER_RIGHT MXC_TMR2 // must change PWM_PORT and PWM_PIN if changed


#define IN_MOTOR_LEFT_PORT MXC_GPIO1
#define IN_MOTOR_RIGHT_PORT MXC_GPIO1
#define IN_MOTOR_RIGHT1_PIN MXC_GPIO_PIN_6
#define IN_MOTOR_RIGHT2_PIN MXC_GPIO_PIN_7
#define IN_MOTOR_LEFT1_PIN MXC_GPIO_PIN_8
#define IN_MOTOR_LEFT2_PIN MXC_GPIO_PIN_9

#define EN_MOTOR_LEFT_PORT MXC_GPIO0
#define EN_MOTOR_LEFT_PIN MXC_GPIO_PIN_20
#define EN_MOTOR_RIGHT_PORT MXC_GPIO0
#define EN_MOTOR_RIGHT_PIN MXC_GPIO_PIN_24

/**************************************************************************************************
  Local Variables
**************************************************************************************************/

static mxc_gpio_cfg_t PWMLeft = {
    EN_MOTOR_LEFT_PORT, // port
    EN_MOTOR_LEFT_PIN, // mask
    MXC_GPIO_FUNC_ALT2, // pad
    MXC_GPIO_PAD_NONE, // func
    MXC_GPIO_VSSEL_VDDIOH, // vssel
    MXC_GPIO_DRVSTR_0, // drvstr
};

static mxc_gpio_cfg_t PWMRight = {
    EN_MOTOR_RIGHT_PORT, // port
    EN_MOTOR_RIGHT_PIN, // mask
    MXC_GPIO_FUNC_ALT2, // pad
    MXC_GPIO_PAD_NONE, // func
    MXC_GPIO_VSSEL_VDDIOH, // vssel
    MXC_GPIO_DRVSTR_0, // drvstr
};

static mxc_gpio_cfg_t left1 = {
    IN_MOTOR_LEFT_PORT, // port
    IN_MOTOR_LEFT1_PIN, // mask
    MXC_GPIO_FUNC_OUT, // pad
    MXC_GPIO_PAD_NONE, // func
    MXC_GPIO_VSSEL_VDDIOH, // vssel
    MXC_GPIO_DRVSTR_0, // drvstr
};

static mxc_gpio_cfg_t left2 = {
    IN_MOTOR_LEFT_PORT, // port
    IN_MOTOR_LEFT2_PIN, // mask
    MXC_GPIO_FUNC_OUT, // pad
    MXC_GPIO_PAD_NONE, // func
    MXC_GPIO_VSSEL_VDDIOH, // vssel
    MXC_GPIO_DRVSTR_0, // drvstr
};

static mxc_gpio_cfg_t right1 = {
    IN_MOTOR_RIGHT_PORT, // port
    IN_MOTOR_RIGHT1_PIN, // mask
    MXC_GPIO_FUNC_OUT, // pad
    MXC_GPIO_PAD_NONE, // func
    MXC_GPIO_VSSEL_VDDIOH, // vssel
    MXC_GPIO_DRVSTR_0, // drvstr
};

static mxc_gpio_cfg_t right2 = {
    IN_MOTOR_RIGHT_PORT, // port
    IN_MOTOR_RIGHT2_PIN, // mask
    MXC_GPIO_FUNC_OUT, // pad
    MXC_GPIO_PAD_NONE, // func
    MXC_GPIO_VSSEL_VDDIOH, // vssel
    MXC_GPIO_DRVSTR_0, // drvstr
};

MyVehicle vehicle = {
    MIN_DUTY_CYCLE, // dutycycle
    STOP, // status
    STRAIGHT // direction
};

wsfHandlerId_t spdTimerHandlerId;
wsfTimer_t spdTimer;

// variable for PID wheel diff
int16_t wheelDiff = -1;

void spdTimerHandlerCB(wsfEventMask_t event, wsfMsgHdr_t *pMsg)
{

    unsigned int periodTicksL = MXC_TMR_GetPeriod(PWM_TIMER_LEFT, PWM_CLOCK_SOURCE, 16, FREQ);
    unsigned int periodTicksR = MXC_TMR_GetPeriod(PWM_TIMER_RIGHT, PWM_CLOCK_SOURCE, 16, FREQ);
    unsigned int dutyTicksL;
    unsigned int dutyTicksR;
    if (vehicle.direction == LEFT)
    {
        dutyTicksL = periodTicksL * (vehicle.dutyCycle + wheelDiff - TURN_SPEED_DIFFERENCE) / 100;
        dutyTicksR = periodTicksR * (vehicle.dutyCycle - wheelDiff) / 100;
    }
    else if (vehicle.direction == RIGHT)
    {
        dutyTicksL = periodTicksL * (vehicle.dutyCycle + wheelDiff) / 100;
        dutyTicksR = periodTicksR * (vehicle.dutyCycle - wheelDiff - TURN_SPEED_DIFFERENCE) / 100;
    }
    else
    {
        if ((vehicle.status == ACC))
        {
            if (vehicle.dutyCycle < MAX_DUTY_CYCLE)
            {
                vehicle.dutyCycle += SPEED_CHANGE_VALUE;
            }
        }
        else if ((vehicle.status == SLOW))
        {
            vehicle.dutyCycle -= SPEED_CHANGE_VALUE;
        }
        else
        {
            vehicle.dutyCycle = MIN_DUTY_CYCLE;
        }
            dutyTicksL = periodTicksL * (vehicle.dutyCycle + wheelDiff) / 100;
            dutyTicksR = periodTicksR * (vehicle.dutyCycle - wheelDiff) / 100;
    }
    
    MXC_TMR_SetPWM(PWM_TIMER_LEFT, dutyTicksL);
    MXC_TMR_SetPWM(PWM_TIMER_RIGHT, dutyTicksR);
    if (vehicle.dutyCycle <= MIN_DUTY_CYCLE)
    {
        stop();
    }

    WsfTimerStartMs(&spdTimer, SPEED_CHANGE_RATE);
}

/*************************************************************************************************/
static void PWMTimer(void)
{
    // Declare variables
    mxc_tmr_cfg_t tmrL; // to configure timer
    mxc_tmr_cfg_t tmrR; // to configure timer
    unsigned int periodTicksL = MXC_TMR_GetPeriod(PWM_TIMER_LEFT, PWM_CLOCK_SOURCE, 16, FREQ);
    unsigned int periodTicksR = MXC_TMR_GetPeriod(PWM_TIMER_RIGHT, PWM_CLOCK_SOURCE, 16, FREQ);
    unsigned int dutyTicksL = periodTicksL * (vehicle.dutyCycle + wheelDiff) / 100;
    unsigned int dutyTicksR = periodTicksR * (vehicle.dutyCycle - wheelDiff) / 100;

    MXC_TMR_Shutdown(PWM_TIMER_LEFT);
    MXC_TMR_Shutdown(PWM_TIMER_RIGHT);

    tmrL.pres = TMR_PRES_16;
    tmrL.mode = TMR_MODE_PWM;
    tmrL.bitMode = TMR_BIT_MODE_32;
    tmrL.clock = PWM_CLOCK_SOURCE;
    tmrL.cmp_cnt = periodTicksL;
    tmrL.pol = 1;

    tmrR.pres = TMR_PRES_16;
    tmrR.mode = TMR_MODE_PWM;
    tmrR.bitMode = TMR_BIT_MODE_32;
    tmrR.clock = PWM_CLOCK_SOURCE;
    tmrR.cmp_cnt = periodTicksR;
    tmrR.pol = 1;

    MXC_TMR_Init(PWM_TIMER_LEFT, &tmrL, true);
    MXC_TMR_SetPWM(PWM_TIMER_LEFT, dutyTicksL);
    MXC_TMR_Start(PWM_TIMER_LEFT);

    MXC_TMR_Init(PWM_TIMER_RIGHT, &tmrR, true);
    MXC_TMR_SetPWM(PWM_TIMER_RIGHT, dutyTicksR);
    MXC_TMR_Start(PWM_TIMER_RIGHT);

    MXC_GPIO_Config(&PWMLeft);
    MXC_GPIO_Config(&PWMRight);
}


void vehicle_init(void)
{
    MXC_GPIO_Config(&left1);
    MXC_GPIO_Config(&right1);
    MXC_GPIO_Config(&left2);
    MXC_GPIO_Config(&right2);
    stop();
    PWMTimer();
    spdTimerHandlerId = WsfOsSetNextHandler(spdTimerHandlerCB);
    spdTimer.handlerId = spdTimerHandlerId;
    
    // somewhere you have to start the timer
    WsfTimerStartMs(&spdTimer, SPEED_CHANGE_RATE);
}


void left(void)
{
    vehicle.direction = LEFT;
}

void right(void)
{
    vehicle.direction = RIGHT;
}

void turn_back(void)
{
    vehicle.direction = STRAIGHT;
}

void slow(void)
{
    vehicle.status = SLOW;
}

void stop(void)
{
    vehicle.status = STOP;
    vehicle.dutyCycle = MIN_DUTY_CYCLE;
    vehicle.direction = STRAIGHT;
    CaliAcc();
    MXC_GPIO_OutClr(left1.port, left1.mask);
    MXC_GPIO_OutClr(left2.port, left2.mask);
    MXC_GPIO_OutClr(right1.port, right1.mask);
    MXC_GPIO_OutClr(right2.port, right2.mask);

}

void forward(void)
{
    vehicle.status = ACC;
    MXC_GPIO_OutClr(left1.port, left1.mask);
    MXC_GPIO_OutSet(left2.port, left2.mask);
    MXC_GPIO_OutClr(right1.port, right1.mask);
    MXC_GPIO_OutSet(right2.port, right2.mask);
   
}

void backward(void)
{
    vehicle.status = ACC;
    MXC_GPIO_OutSet(left1.port, left1.mask);
    MXC_GPIO_OutClr(left2.port, left2.mask);
    MXC_GPIO_OutSet(right1.port, right1.mask);
    MXC_GPIO_OutClr(right2.port, right2.mask);
}

void stopObstacle(void)
{
    MXC_GPIO_OutClr(left2.port, left2.mask);
    MXC_GPIO_OutClr(right2.port, right2.mask);
}

int getVehicleStatus(void)
{
    return vehicle.status;
}

int getVehicleDirection(void)
{
    return vehicle.direction;
}

void setWheelDiff(int16_t PID_control)
{
    //wheelDiff = PID_control;
}


