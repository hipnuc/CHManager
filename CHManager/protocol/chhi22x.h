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
    kItemTest8F = 0x60,
    kItemRFSOL =               0x62,   /* new RF data */
    kItemIMUSOL =               0x91,   /* new IMU data */
    kItemID =                   0x90,   /* user programed ID    size: 1 */
    kItemAccRaw =               0xA0,   /* raw acc              size: 3x2 */
    kItemGyrRaw =               0xB0,   /* raw gyro             size: 3x2 */
    kItemMagRaw =               0xC0,   /* raw mag              size: 3x2 */
    kItemRotationEular =        0xD0,   /* eular angle          size:3x2 */
    kItemRotationEular2 =       0xD9,   /* new eular angle      size:3x4 */
    kItemRotationQuat =         0xD1,   /* att q,               size:4x4 */
    kItemPressure =             0xF0,   /* pressure             size:1x4 */
    kItemEnd =                  0x00,
}ItemID_t;


typedef struct
{
    uint8_t     tag;                /* data packet tag */
    uint8_t     id;
    uint8_t     rev[6];             /* reserved */
    uint32_t    ts;                 /* timestamp */
    float       acc[3];
    float       gyr[3];
    float       mag[3];
    float       eul[3];             /* eular angles:R/P/Y */
    float       quat[4];            /* quaternion */
}__attribute__((packed)) id0x91_t;


typedef struct
{
    uint8_t         tag;
    uint8_t         gwid;
    uint8_t         node_cnt;
    uint8_t         rev[5];
    id0x91_t        node[16];
}__attribute__((packed)) id0x62_t;


void imu_data_parse(id0x91_t *data, uint8_t *buf, uint32_t len);
int imu_data_decode_init(void);
int dump_rf_data(id0x91_t *imu);

#endif // CHHI22X_H
