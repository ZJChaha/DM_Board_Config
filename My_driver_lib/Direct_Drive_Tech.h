//
// Created by ZJC on 2024/6/5.
//

#ifndef DM_BOARD_CONFIG_DIRECT_DRIVE_TECH_H
#define DM_BOARD_CONFIG_DIRECT_DRIVE_TECH_H
#include "All_struct.h"

#define Rad_to_Angle 57.29578f

enum Motor_Mode
{
    OPENLOOP,
    CURRENTLOOP,
    VELOCITYLOOP,
    POSITIONLOOP
};


extern DDT_M0603A_t DDT_M0603A;

extern uint8_t break_cmd[10];

extern uint8_t enable_flag,send_flag,feedback_flag;
extern int cnt_tim_test;
float NormalizeAngle(const float angle);

void Motor_current_send(uint8_t id,short current);
void Motor_enable(uint8_t id,uint8_t mode);
void Motor_data_process(const uint8_t *data,DDT_M0603A_t *para);
void Get_other_feedback(uint8_t id);

#endif //DM_BOARD_CONFIG_DIRECT_DRIVE_TECH_H
