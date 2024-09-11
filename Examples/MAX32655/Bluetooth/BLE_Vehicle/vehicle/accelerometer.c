/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  accelerometer Configuration
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
#include <math.h>
#include "i2c.h"
#include "tmr.h"
#include "wsf_types.h"
#include "wsf_trace.h"
#include "wsf_timer.h"
#include "adxl343.h"
#include "accelerometer.h"
#include "bcv_api.h"
#include "arm_math.h"
#include "vehicle.h"


#define I2C_MASTER MXC_I2C2 ///< I2C instance
#define I2C_FREQ 300000 ///< I2C clock frequency
#define I2C_PORT MXC_GPIO0
#define I2C_SCL_PIN MXC_GPIO_PIN_31
#define I2C_SDA_PIN MXC_GPIO_PIN_30

#define ACC_G 9.80665f

// Parameters for Acc and Cal timer
#define ACC_PERIOD 25 // (ms)
#define CAL_PERIOD 1

// Define filter parameters
#define SAMPLE_RATE 40.0f    // Sample rate in Hz
#define CUTOFF_FREQ 2.0f     // Cutoff frequency in Hz
#define NUM_TAPS 29          // Number of filter coefficients (Taps)
#define BLOCK_SIZE 1         // Process one sample at a time

// Filter coefficients (example)
const float32_t firCoeffs32[NUM_TAPS] = {
 -0.0017925669387052663 , -0.0018788835119309842 , -0.0020285055838561426 , -0.001671210034246938 , 
 1.0227572163598086e-18 , 0.003855260328680345 , 0.010606038043477255 , 0.020585454958369463 , 
 0.033584618515455 , 0.048789468603431335 , 0.06484204393622812 , 0.08002274600839686 , 
 0.09252242589602286 , 0.1007515123545129 , 0.10362319484833048 , 0.1007515123545129 , 
 0.09252242589602286 , 0.08002274600839686 , 0.06484204393622812 , 0.048789468603431335 , 
 0.033584618515455 , 0.020585454958369463 , 0.010606038043477255 , 0.003855260328680345 , 
 1.0227572163598086e-18 , -0.001671210034246938 , -0.0020285055838561426 , -0.0018788835119309842 , 
 -0.0017925669387052663
};

// Filter state buffer (must be NUM_TAPS + BLOCK_SIZE - 1 in size)
float32_t state_x[NUM_TAPS + BLOCK_SIZE - 1];
float32_t state_y[NUM_TAPS + BLOCK_SIZE - 1];
float32_t state_z[NUM_TAPS + BLOCK_SIZE - 1];

// FIR filter instance
arm_fir_instance_f32 fx;
arm_fir_instance_f32 fy;
arm_fir_instance_f32 fz;

float32_t desired_speed = 0; // Target motor speed in RPM
float32_t actual_speed = 0.0f;     // Current motor speed in RPM
float32_t control_signal = 0.0f;   // Output from PID controller (PWM duty cycle)


// PID parameters
float32_t Kp = 0.2f;
float32_t Ki = 0.1f;
float32_t Kd = 5.0f;

// PID controller instance
arm_pid_instance_f32 PID;


static void initFilter(void) {
    // Initialize the FIR filter
    arm_fir_init_f32(&fx, NUM_TAPS, (float32_t *)firCoeffs32, state_x, BLOCK_SIZE);
    arm_fir_init_f32(&fy, NUM_TAPS, (float32_t *)firCoeffs32, state_y, BLOCK_SIZE);
    arm_fir_init_f32(&fz, NUM_TAPS, (float32_t *)firCoeffs32, state_z, BLOCK_SIZE);
}

static void processSample(int16_t* inputSample) {
    float32_t inputF32_x = (float32_t) (inputSample[0]);
    float32_t outputF32_x;
    // Process the single sample
    arm_fir_f32(&fx, &inputF32_x, &outputF32_x, BLOCK_SIZE);
    // Convert back to int16_t
    inputSample[0] = (int16_t)outputF32_x;
    
    float32_t inputF32_y = (float32_t) (inputSample[1]);
    float32_t outputF32_y;
    // Process the single sample
    arm_fir_f32(&fy, &inputF32_y, &outputF32_y, BLOCK_SIZE);
    // Convert back to int16_t
    inputSample[1] = (int16_t)outputF32_y;

    float32_t inputF32_z = (float32_t) (inputSample[2]);
    float32_t outputF32_z;
    // Process the single sample
    arm_fir_f32(&fz, &inputF32_z, &outputF32_z, BLOCK_SIZE);
    // Convert back to int16_t
    inputSample[2] = (int16_t)outputF32_z;
    
    return;
}

static void trainingFilter(void)
{
    for(int i = 0; i < NUM_TAPS + BLOCK_SIZE; i++)
    {
        int16_t axis_data[3] = {0};
        adxl343_get_axis_data(axis_data);
        processSample(axis_data);
    }
}

wsfHandlerId_t accTimerHandlerId;
wsfTimer_t accTimer;

wsfHandlerId_t calTimerHandlerId;
wsfTimer_t calTimer;

static float initAngleX; // (fraction of G on x axis)
static float initAngleY; // (fraction of G on y axis)

static float initAccX;
static float initAccY;

static float speedX;
static float speedY;

void AccTimerHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg)
{
    int16_t axis_data[3] = {0};
    adxl343_get_axis_data(axis_data);
    processSample(axis_data);
    int16_t toSend[5] = {
        axis_data[0], axis_data[1], axis_data[2],
        (int16_t) (speedX * 100), (int16_t) (speedY * 100)
    };
    printf("%2.2f   %2.2f   \n",speedX, speedY);
    BcvSendAcc((uint8_t*) toSend);
    WsfTimerStartMs(&accTimer, ACC_PERIOD);
}



void CalTimerHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg)
{
    int16_t axis_data[3] = {0};
    adxl343_get_axis_data(axis_data);


    float accX = (float) ( ((axis_data[0] - initAngleX * axis_data[2]) * ADXL343_SF_2G * ACC_G)) - initAccX;
    float accY = (float) ( ((axis_data[1] - initAngleY * axis_data[2]) * ADXL343_SF_2G * ACC_G)) - initAccY;
    //float rounded_accX = ((int) (accX * 10)) / 10.0f;
    //float rounded_accY = ((int) (accY * 10)) / 10.0f;

    speedX += accX * CAL_PERIOD / 1000;
    speedY += accY * CAL_PERIOD / 1000;
    if (getVehicleStatus() == STOP || getVehicleDirection() != STRAIGHT)
    {
        arm_pid_init_f32(&PID, 1);
    }
    else
    {
        actual_speed = accY;
        float32_t error = actual_speed - desired_speed;
        control_signal = arm_pid_f32(&PID, error);
        setWheelDiff((int16_t) control_signal);
    }


    WsfTimerStartMs(&calTimer, CAL_PERIOD);
}

void CaliAcc(void)
{
    speedX = 0;
    speedY = 0;
    int16_t axis_data[3] = {0};
    adxl343_get_axis_data(axis_data);

    initAngleX = (float) axis_data[0] / (float) axis_data[2];
    initAngleY = (float) axis_data[1] / (float) axis_data[2];

    adxl343_get_axis_data(axis_data);


    initAccX = (float) ( ((axis_data[0] - initAngleX * axis_data[2]) * ADXL343_SF_2G * ACC_G));
    initAccY = (float) ( ((axis_data[1] - initAngleY * axis_data[2]) * ADXL343_SF_2G * ACC_G));
}

void AccInit(void)
{
    MXC_GPIO_SetVSSEL(I2C_PORT, MXC_GPIO_VSSEL_VDDIOH, I2C_SCL_PIN | I2C_SDA_PIN);

    int error = E_NO_ERROR;
    error = MXC_I2C_Init(I2C_MASTER, 1, 0);
    if (error != E_NO_ERROR) {
        printf("I2C master configure failed with error %d\n", error);
    }

    MXC_I2C_SetFrequency(I2C_MASTER, I2C_FREQ);

    
    error = adxl343_init(I2C_MASTER);
    if (error != E_NO_ERROR) {
        printf("Trouble initializing ADXL343.%d\n",error);
    }
    int result;
    int16_t tmp[3];
    int8_t axis_offsets[3] = { -2, -2, 7 }; 

    result = 0;
    result |= adxl343_set_power_control(ADXL343_PWRCTL_STANDBY);
    result |= adxl343_set_int_enable(0);
    result |= adxl343_get_axis_data(tmp); // Unload any unread data

    result |= adxl343_set_data_rate(ADXL343_DR_25HZ);
    result |= adxl343_set_range(ADXL343_RANGE_2G);
    
    result |= adxl343_set_power_mode(ADXL343_PWRMOD_NORMAL);
    result |= adxl343_set_offsets(axis_offsets);
    result |= adxl343_set_fifo_mode(ADXL343_FIFO_BYPASS);
    result |= adxl343_set_int_map(ADXL343_INT_DATA_READY);
    
    result |= adxl343_set_int_enable(ADXL343_INT_DATA_READY);
    result |= adxl343_set_power_control(ADXL343_PWRCTL_MEASURE);

    if (result != E_NO_ERROR) {
        printf("Trouble configuring ADXL343.\n");
    }

    
    PID.Kp = Kp;
    PID.Ki = Ki;
    PID.Kd = Kd;
    arm_pid_init_f32(&PID, 1);  // Reset the PID controller's state

    // Initialize the DSP filter
    initFilter();
    trainingFilter();

    // Calibrate the accelerometer
    CaliAcc();
    
    // start the timer
    accTimerHandlerId = WsfOsSetNextHandler(AccTimerHandler);
    accTimer.handlerId = accTimerHandlerId;
    WsfTimerStartMs(&accTimer, ACC_PERIOD);

    calTimerHandlerId = WsfOsSetNextHandler(CalTimerHandler);
    calTimer.handlerId = calTimerHandlerId;
    WsfTimerStartMs(&calTimer, CAL_PERIOD);
}   
