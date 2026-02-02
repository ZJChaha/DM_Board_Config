//
// Created by ZJC on 2024/7/3.
//
#include "bsp_usart.h"
#include "usart.h"
#include "MC6C_RC.h"

MC6C_RC_t MC_RC;


void MC6C_RC_frame_parse(MC6C_RC_t *RC,const uint8_t *buf)
{

    if(buf[0]==0x0F && buf[24]==0)
    {
        RC->ch[0] = ((buf[1] | buf[2] << 8) & 0x07FF);
        RC->ch[1] = ((buf[2] >> 3 | buf[3] << 5) & 0x07FF);
        RC->ch[2] = ((buf[3] >> 6 | buf[4] << 2 | buf[5] << 10) & 0x07FF);
        RC->ch[3] = ((buf[5] >> 1 | buf[6] << 7) & 0x07FF);
        RC->ch[4] = ((buf[6] >> 4 | buf[7] << 4) & 0x07FF);
        RC->ch[5] = ((buf[7] >> 7 | buf[8] << 1 | buf[9] << 9) & 0x07FF);
    }
}




