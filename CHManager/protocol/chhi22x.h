//SPDX-License-Inentifier: Apache-2.0
/*
 * Copyright (C) 2016-2020 HiPNUC All rights reserved.
 *
 */

#ifndef CHHI22X_H
#define CHHI22X_H
#define MAX_SLV_CNT 8

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    kItemTest8F =               0x60,
    kItemExtend =               0x61,   /* extend data */
    kItemRFQuat =               0x71,
    kItemRFEul  =               0x72,
    kItemRFAcc  =               0x75,
    kItemRFGyr  =               0x78,
    kItemRFMag  =               0x7A,
    kItemRFData =               0x62,
    kItemID =                   0x90,   /* user programed ID    size: 1 */
    kItemAccRaw =               0xA0,   /* raw acc              size: 3x2 */
    kItemAccCalibrated =        0xA1,
    kItemAccFiltered =          0xA2,
    kItemAccLinear   =          0xA5,
    kItemAccGravity  =          0xA6,
    kItemAccNorm =              0xA8,
    kItemGyrRaw =               0xB0,   /* raw gyro             size: 3x2 */
    kItemGyrCalibrated =        0xB1,
    kItemGyrFiltered =          0xB2,
    kItemGyrNorm =              0xB8,
    kItemMagRaw =               0xC0,   /* raw mag              size: 3x2 */
    kItemMagCalibrated =        0xC1,
    kItemMagFiltered =          0xC2,
    kItemRotationEular =        0xD0,   /* eular angle          size:3x2 */
    kItemRotationEular2 =       0xD9,   /* new eular angle      size:3x4 */
    kItemRotationQuat =         0xD1,   /* att q,               size:4x4 */
    kItemTemperature =          0xE0,
    kItemPressure =             0xF0,   /* pressure             size:1x4 */
    kItemEnd =                  0x00,
}ItemID_t;

typedef struct
{
    uint8_t id;
    float       q[4];
    int16_t     acc[3];
    int16_t     gyr[3];
    int16_t     mag[3];
    float       eul[3];
    float       temperature;
    float       rf_quat[MAX_SLV_CNT][4];
    int16_t     rf_acc[MAX_SLV_CNT][3];
    int16_t     rf_gyr[MAX_SLV_CNT][3];
    int16_t     rf_mag[MAX_SLV_CNT][3];
    int16_t     rf_eul[MAX_SLV_CNT][3];
}imu_data_t;

void imu_data_parse(imu_data_t *imu, uint8_t *buf, uint32_t len);
int imu_data_decode_init(void);
int dump_rf_data(imu_data_t *imu);

#endif // CHHI22X_H
