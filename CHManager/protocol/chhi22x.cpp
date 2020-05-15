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
id0x91_t imu_data;

int dump_rf_data(id0x91_t *imu)
{
    *imu = imu_data;

    return 0;
}

/* convert binary stream to int16 array */
static void stream2int16(int16_t *dat, uint8_t *buf)
{
    dat[0] = (int16_t)(buf[0] | buf[1] << 8);
    dat[1] = (int16_t)(buf[2] | buf[3] << 8);
    dat[2] = (int16_t)(buf[4] | buf[5] << 8);
}

/*  callback function of  when recv a data frame successfully */
void imu_data_parse(id0x91_t *dat, uint8_t *buf, uint32_t len)
{
    uint16_t offset, node_num;
    int16_t tmp[3];
    uint8_t *p;
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
            case kItemAccRaw:
            case 0xA1:
                stream2int16(tmp, &buf[offset + 1]);
                dat->acc[0] = (float)tmp[0] / 1000;
                dat->acc[1] = (float)tmp[1] / 1000;
                dat->acc[2] = (float)tmp[2] / 1000;
                offset += 7;
                break;
            case kItemGyrRaw:
            case 0xB1:
                stream2int16(tmp, &buf[offset + 1]);
                dat->gyr[0] = (float)tmp[0] / 10;
                dat->gyr[1] = (float)tmp[1] / 10;
                dat->gyr[2] = (float)tmp[2] / 10;
                offset += 7;
                break;
            case kItemMagRaw:
            case 0xC1:
                stream2int16(tmp, &buf[offset + 1]);
                dat->mag[0] = (float)tmp[0] / 10;
                dat->mag[1] = (float)tmp[1] / 10;
                dat->mag[2] = (float)tmp[2] / 10;
                offset += 7;
                break;
            case kItemRotationEular:
                stream2int16(tmp, &buf[offset + 1]);
                dat->eul[1] = (float)tmp[0] / 100;
                dat->eul[0] = (float)tmp[1] / 100;
                dat->eul[2] = (float)tmp[2] / 10;
                offset += 7;
                break;
            case kItemRotationQuat:
                memcpy((void*)dat->quat, p + offset + 1, sizeof(dat->quat));
                offset += 17;
                break;
            case kItemPressure:
                offset += 5;
                break;
            case kItemIMUSOL:
                memcpy(dat, p + offset, sizeof(id0x91_t));
                offset += 76;
                break;
            case kItemRFSOL: /* TODO: RFSOL not parsed */
                node_num = buf[offset+2];
                offset += 8+76*node_num;
                break;
            default:
                //printf("data decode wrong\r\n");
                return;
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
