//
// Created by ZJC on 2024/7/3.
//

#ifndef DM_BOARD_CONFIG_MC6C_RC_H
#define DM_BOARD_CONFIG_MC6C_RC_H
#include "stdint.h"


typedef struct
{
    uint16_t ch[6];


}MC6C_RC_t;

extern MC6C_RC_t MC_RC;

void MC6C_RC_frame_parse(MC6C_RC_t *RC,const uint8_t *data);





#endif //DM_BOARD_CONFIG_MC6C_RC_H
