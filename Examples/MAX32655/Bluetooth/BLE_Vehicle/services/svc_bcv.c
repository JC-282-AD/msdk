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

#include "svc_bcv.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! Characteristic read permissions */
#ifndef BCV_SEC_PERMIT_READ
#define BCV_SEC_PERMIT_READ SVC_SEC_PERMIT_READ
#endif

/*! Characteristic write permissions */
#ifndef BCV_SEC_PERMIT_WRITE
#define BCV_SEC_PERMIT_WRITE SVC_SEC_PERMIT_WRITE
#endif

/**************************************************************************************************
 Service variables
**************************************************************************************************/

/*Service variables declaration*/
const uint8_t attBcvSvcUuid[ATT_128_UUID_LEN] = { ATT_UUID_BCV_SERVICE };

/*Characteristic variables declaration*/
const uint8_t svcBcvAccUuid[ATT_128_UUID_LEN] = { ATT_UUID_BCV_ACC };
const uint8_t svcBcvLidUuid[ATT_128_UUID_LEN] = { ATT_UUID_BCV_LID };
const uint8_t svcBcvUpUuid[ATT_128_UUID_LEN] = { ATT_UUID_BCV_UP };
const uint8_t svcBcvDownUuid[ATT_128_UUID_LEN] = { ATT_UUID_BCV_DOWN };
const uint8_t svcBcvLeftUuid[ATT_128_UUID_LEN] = { ATT_UUID_BCV_LEFT };
const uint8_t svcBcvRightUuid[ATT_128_UUID_LEN] = { ATT_UUID_BCV_RIGHT };
const uint8_t svcBcvStopUuid[ATT_128_UUID_LEN] = { ATT_UUID_BCV_STOP };

static const uint8_t bcvValSvc[] = { ATT_UUID_BCV_SERVICE };
static const uint16_t bcvLenSvc = sizeof(bcvValSvc);

static const uint8_t bcvAccValCh[] = { ATT_PROP_READ | ATT_PROP_NOTIFY,
                                          UINT16_TO_BYTES(BCV_ACC_HDL), ATT_UUID_BCV_ACC };
static const uint16_t bcvAccLenCh = sizeof(bcvAccValCh);

static const uint8_t bcvLidValCh[] = { ATT_PROP_READ | ATT_PROP_NOTIFY,
                                          UINT16_TO_BYTES(BCV_LID_HDL), ATT_UUID_BCV_LID };
static const uint16_t bcvLidLenCh = sizeof(bcvLidValCh);

static const uint8_t bcvUpValCh[] = { ATT_PROP_READ | ATT_PROP_WRITE, UINT16_TO_BYTES(BCV_UP_HDL),
                                     ATT_UUID_BCV_UP };
static const uint16_t bcvUpLenCh = sizeof(bcvUpValCh);

static const uint8_t bcvDownValCh[] = { ATT_PROP_READ | ATT_PROP_WRITE, UINT16_TO_BYTES(BCV_DOWN_HDL),
                                     ATT_UUID_BCV_DOWN };
static const uint16_t bcvDownLenCh = sizeof(bcvDownValCh);

static const uint8_t bcvLeftValCh[] = { ATT_PROP_READ | ATT_PROP_WRITE, UINT16_TO_BYTES(BCV_LEFT_HDL),
                                     ATT_UUID_BCV_LEFT };
static const uint16_t bcvLeftLenCh = sizeof(bcvLeftValCh);

static const uint8_t bcvRightValCh[] = { ATT_PROP_READ | ATT_PROP_WRITE, UINT16_TO_BYTES(BCV_RIGHT_HDL),
                                     ATT_UUID_BCV_RIGHT };
static const uint16_t bcvRightLenCh = sizeof(bcvRightValCh);

static const uint8_t bcvStopValCh[] = { ATT_PROP_READ | ATT_PROP_WRITE, UINT16_TO_BYTES(BCV_STOP_HDL),
                                     ATT_UUID_BCV_STOP };
static const uint16_t bcvStopLenCh = sizeof(bcvStopValCh);

/*Characteristic values declaration*/
static uint8_t bcvAccVal[] = { 0 };
static const uint16_t bcvAccValLen = sizeof(bcvAccVal);

static uint8_t bcvAccValChCcc[] = { UINT16_TO_BYTES(0x0000) };
static const uint16_t bcvAccLenValChCcc = sizeof(bcvAccValChCcc);

static uint8_t bcvLidVal[] = { 0 };
static const uint16_t bcvLidValLen = sizeof(bcvLidVal);

static uint8_t bcvLidValChCcc[] = { UINT16_TO_BYTES(0x0000) };
static const uint16_t bcvLidLenValChCcc = sizeof(bcvLidValChCcc);

static uint8_t bcvUpVal[] = { 0 };
static const uint16_t bcvUpValLen = sizeof(bcvUpVal);

static uint8_t bcvDownVal[] = { 0 };
static const uint16_t bcvDownValLen = sizeof(bcvDownVal);

static uint8_t bcvLeftVal[] = { 0 };
static const uint16_t bcvLeftValLen = sizeof(bcvLeftVal);

static uint8_t bcvRightVal[] = { 0 };
static const uint16_t bcvRightValLen = sizeof(bcvRightVal);

static uint8_t bcvStopVal[] = { 0 };
static const uint16_t bcvStopValLen = sizeof(bcvStopVal);
/**************************************************************************************************
 Maxim Custom Service group
**************************************************************************************************/

/* Attribute list for bcv group */
static const attsAttr_t bcvList[] = {
    /*-----------------------------*/
    /* Service declaration */
    { attPrimSvcUuid, (uint8_t *)bcvValSvc, (uint16_t *)&bcvLenSvc, sizeof(bcvValSvc), 0,
      BCV_SEC_PERMIT_READ },

    /*----------------------------*/
    /* Acc characteristic declaration */
    { attChUuid, (uint8_t *)bcvAccValCh, (uint16_t *)&bcvAccLenCh, sizeof(bcvAccValCh), 0,
      BCV_SEC_PERMIT_READ },
    /* Acc characteristic value */
    { svcBcvAccUuid, (uint8_t *)bcvAccVal, (uint16_t *)&bcvAccValLen, sizeof(bcvAccVal),
      0, BCV_SEC_PERMIT_READ },
    /*Acc characteristic value descriptor*/
    { attCliChCfgUuid, (uint8_t *)bcvAccValChCcc, (uint16_t *)&bcvAccLenValChCcc,
      sizeof(bcvAccValChCcc), ATTS_SET_CCC, (ATTS_PERMIT_READ | SVC_SEC_PERMIT_WRITE) },

    /* Lid characteristic declaration */
    { attChUuid, (uint8_t *)bcvLidValCh, (uint16_t *)&bcvLidLenCh, sizeof(bcvLidValCh), 0,
      BCV_SEC_PERMIT_READ },
    /* Lid characteristic value */
    { svcBcvLidUuid, (uint8_t *)bcvLidVal, (uint16_t *)&bcvLidValLen, sizeof(bcvLidVal),
      0, BCV_SEC_PERMIT_READ },
    /*Lid characteristic value descriptor*/
    { attCliChCfgUuid, (uint8_t *)bcvLidValChCcc, (uint16_t *)&bcvLidLenValChCcc,
      sizeof(bcvLidValChCcc), ATTS_SET_CCC, (ATTS_PERMIT_READ | SVC_SEC_PERMIT_WRITE) },

    /* Up key press characteristic declaration */
    { attChUuid, (uint8_t *)bcvUpValCh, (uint16_t *)&bcvUpLenCh, sizeof(bcvUpValCh),
      ATTS_SET_WRITE_CBACK, (BCV_SEC_PERMIT_READ | BCV_SEC_PERMIT_WRITE) },
    /* Up key press characteristic value */
    { svcBcvUpUuid, (uint8_t *)bcvUpVal, (uint16_t *)&bcvUpValLen, sizeof(bcvUpVal),
      ATTS_SET_WRITE_CBACK, (BCV_SEC_PERMIT_READ | BCV_SEC_PERMIT_WRITE) },

    /* Down key press characteristic declaration */
    { attChUuid, (uint8_t *)bcvDownValCh, (uint16_t *)&bcvDownLenCh, sizeof(bcvDownValCh),
      ATTS_SET_WRITE_CBACK, (BCV_SEC_PERMIT_READ | BCV_SEC_PERMIT_WRITE) },
    /* Down key press characteristic value */
    { svcBcvDownUuid, (uint8_t *)bcvDownVal, (uint16_t *)&bcvDownValLen, sizeof(bcvDownVal),
      ATTS_SET_WRITE_CBACK, (BCV_SEC_PERMIT_READ | BCV_SEC_PERMIT_WRITE) },

    /* Left key press characteristic declaration */
    { attChUuid, (uint8_t *)bcvLeftValCh, (uint16_t *)&bcvLeftLenCh, sizeof(bcvLeftValCh),
      ATTS_SET_WRITE_CBACK, (BCV_SEC_PERMIT_READ | BCV_SEC_PERMIT_WRITE) },
    /* Left key press characteristic value */
    { svcBcvLeftUuid, (uint8_t *)bcvLeftVal, (uint16_t *)&bcvLeftValLen, sizeof(bcvLeftVal),
      ATTS_SET_WRITE_CBACK, (BCV_SEC_PERMIT_READ | BCV_SEC_PERMIT_WRITE) },

    /* Right key press characteristic declaration */
    { attChUuid, (uint8_t *)bcvRightValCh, (uint16_t *)&bcvRightLenCh, sizeof(bcvRightValCh),
      ATTS_SET_WRITE_CBACK, (BCV_SEC_PERMIT_READ | BCV_SEC_PERMIT_WRITE) },
    /* Right key press characteristic value */
    { svcBcvRightUuid, (uint8_t *)bcvRightVal, (uint16_t *)&bcvRightValLen, sizeof(bcvRightVal),
      ATTS_SET_WRITE_CBACK, (BCV_SEC_PERMIT_READ | BCV_SEC_PERMIT_WRITE) },

    /* Stop (space) key press characteristic declaration */
    { attChUuid, (uint8_t *)bcvStopValCh, (uint16_t *)&bcvStopLenCh, sizeof(bcvStopValCh),
      ATTS_SET_WRITE_CBACK, (BCV_SEC_PERMIT_READ | BCV_SEC_PERMIT_WRITE) },
    /* Stop (space) key press characteristic value */
    { svcBcvStopUuid, (uint8_t *)bcvStopVal, (uint16_t *)&bcvStopValLen, sizeof(bcvStopVal),
      ATTS_SET_WRITE_CBACK, (BCV_SEC_PERMIT_READ | BCV_SEC_PERMIT_WRITE) }
};

/* Test group structure */
static attsGroup_t svcBcvGroup = { NULL, (attsAttr_t *)bcvList, NULL,
                                   NULL, BCV_START_HDL,         BCV_END_HDL };

/*************************************************************************************************/
/*!
 *  \brief  Add the services to the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcBcvAddGroup(void)
{
    AttsAddGroup(&svcBcvGroup);
}

/*************************************************************************************************/
/*!
 *  \brief  Remove the services from the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcBcvRemoveGroup(void)
{
    AttsRemoveGroup(BCV_START_HDL);
}

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
void SvcBcvCbackRegister(attsReadCback_t readCback, attsWriteCback_t writeCback)
{
    svcBcvGroup.readCback = readCback;
    svcBcvGroup.writeCback = writeCback;
}
