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
#include <stdio.h>

typedef enum status {
    STOP,
    ACC,
    SLOW
} Status;

typedef enum direction {
    STRAIGHT,
    LEFT,
    RIGHT
} Direction;

typedef struct myVehicle
{
    uint8_t dutyCycle;
    Status status;
    Direction direction;

} MyVehicle;

/*************************************************************************************************/
/*!
 *  \brief  Initialize the vehicle status.
 *
 *  \return None.
 */
/*************************************************************************************************/
void vehicle_init(void);

/*************************************************************************************************/
/*!
 *  \brief  Perform left key press operation.
 *
 *  \return None.
 */
/*************************************************************************************************/
void left(void);

/*************************************************************************************************/
/*!
 *  \brief  Perform right key press operation.
 *
 *  \return None.
 */
/*************************************************************************************************/
void right(void);

/*************************************************************************************************/
/*!
 *  \brief  Perform direction key (left & right) release operation.
 *
 *  \return None.
 */
/*************************************************************************************************/
void turn_back(void);

/*************************************************************************************************/
/*!
 *  \brief  Perform up key press operation.
 *
 *  \return None.
 */
/*************************************************************************************************/
void forward(void);

/*************************************************************************************************/
/*!
 *  \brief  Perform down key press operation.
 *
 *  \return None.
 */
/*************************************************************************************************/
void backward(void);

/*************************************************************************************************/
/*!
 *  \brief  Perform space (brake) key press operation.
 *
 *  \return None.
 */
/*************************************************************************************************/
void stop(void);

/*************************************************************************************************/
/*!
 *  \brief  Perform moving key (up & down) release operation.
 *
 *  \return None.
 */
/*************************************************************************************************/
void slow(void);