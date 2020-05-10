//SPDX-License-Inentifier: Apache-2.0
/*
 * Copyright (C) 2016-2020 HiPNUC All rights reserved.
 *
 */

#include "chhi22x.h"
#include "kptl.h"

#include <string.h>

static pkt_dec_t M_dec;
static frame_packet_t rx_pkt;
static imu_data_t imu_data;

/* default rx slave cnt */
static uint8_t rf_slave_cnt = 12;

typedef struct
{
    uint8_t flag;           /* 0:read  1:write */
    uint8_t rsvd_addr;
    uint8_t param_cnt;
}ext_data_info_t;

int dump_rf_data(imu_data_t *imu)
{
    *imu = imu_data;

    return 0;
}

/*  callback function of  when recv a data frame successfully */
void imu_data_parse(imu_data_t *dat, uint8_t *buf, uint32_t len)
{
    uint32_t offset;
    uint8_t *p;
    ext_data_info_t *ext = NULL;
    p = buf;
    offset = 0;

    while(offset < len)
    {
        switch(p[offset])
        {
            case kItemID:
                dat->id = p[1];
                offset += 2;
                break;
            case kItemTest8F:
                offset += 8 * 4 + 1;
                break;
            case kItemAccRaw:
            case kItemAccCalibrated:
            case kItemAccFiltered:
            case kItemAccLinear:
                memcpy(dat->acc, p + offset + 1, sizeof(dat->acc));
                offset += 7;
                break;
            case kItemGyrRaw:
            case kItemGyrCalibrated:
            case kItemGyrFiltered:
                memcpy(dat->gyr, p + offset + 1, sizeof(dat->gyr));
                offset += 7;
                break;
            case kItemMagRaw:
            case kItemMagCalibrated:
            case kItemMagFiltered:
                memcpy(dat->mag, p + offset + 1, sizeof(dat->mag));
                offset += 7;
                break;
            case kItemRotationEular:
                dat->eul[0] = ((float)(int16_t)(p[offset+1] + (p[offset+2]<<8)))/100;
                dat->eul[1] = ((float)(int16_t)(p[offset+3] + (p[offset+4]<<8)))/100;
                dat->eul[2] = ((float)(int16_t)(p[offset+5] + (p[offset+6]<<8)))/10;
                offset += 7;
                break;
            case kItemRotationEular2:
                //memcpy(eular, p + offset + 1, sizeof(eular));
                offset += 13;
                break;
            case kItemRotationQuat:
                memcpy(dat->q, p + offset + 1, sizeof(dat->q));
                offset += 17;
                break;
            case kItemPressure:
                offset += 5;
                break;
            case kItemTemperature:
                memcpy(&dat->temperature, p + offset + 1, sizeof(dat->temperature));
                offset += 5;
                break;
            case kItemExtend:
                ext = (ext_data_info_t*)(p + offset + 1);

                /* get rf slave cnt */
                rf_slave_cnt = ext->param_cnt;
                offset += 4;
                break;
            case kItemRFQuat:
                memcpy(&dat->rf_quat[0][0], p + offset + 1, sizeof(dat->rf_quat[0]) * rf_slave_cnt);
                offset += 1 + sizeof(dat->rf_quat[0]) * rf_slave_cnt;
                break;
            case kItemRFEul:

                memcpy(&dat->rf_eul[0][0], p + offset + 1, sizeof(dat->rf_eul[0]) * rf_slave_cnt);
                offset += 1 + sizeof(dat->rf_eul[0]) * rf_slave_cnt;
                break;
            default:
                offset++;
                break;
        }
    }
}

void kptl_cb(frame_packet_t *pkt)
{
    imu_data_parse(&imu_data, pkt->payload, kptl_get_payload_len(pkt));
}

int imu_data_decode_init(void)
{
    M_dec.fp = &rx_pkt;
    M_dec.cb = kptl_cb;
    kptl_decode_init(&M_dec);

    return 0;
}

