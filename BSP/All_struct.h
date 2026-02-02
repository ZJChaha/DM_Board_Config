//
// Created by ZJC on 2024/4/27.
//

#ifndef DM_BOARD_CONFIG_ALL_STRUCT_H
#define DM_BOARD_CONFIG_ALL_STRUCT_H

#include <stdint.h>

#define CUR_CONSTANT 5461.33333f
#define TORQUE_CONSTANT 0.741f

typedef struct
{
    uint16_t x;
    uint16_t y;
}Point_t;

typedef struct
{
    uint8_t Big_endian;
    uint8_t Little_endian;
}Pixel_t;


typedef struct
{
    short current;  //-32767~32767
    float velocity; //-40~40 rad/s
    float last_velocity;
    float angular_acc;//角加速度 rad/(s*s)
    float angle;    //-180°~180°
    int turns; //圈数
    uint8_t temp;   //单位:℃
    uint8_t err_code;//故障码
}DDT_M0603A_t;


typedef struct {
    float X_last; //上一时刻的最优结果  X(k-|k-1)
    float X_mid;  //当前时刻的预测结果  X(k|k-1)
    float X_now;  //当前时刻的最优结果  X(k|k)
    float P_mid;  //当前时刻预测结果的协方差  P(k|k-1)
    float P_now;  //当前时刻最优结果的协方差  P(k|k)
    float P_last; //上一时刻最优结果的协方差  P(k-1|k-1)
    float kg;     //kalman增益
    float A;      //系统参数
    float B;
    float Q;
    float R;
    float H;
}extKalman_t;











#endif //DM_BOARD_CONFIG_ALL_STRUCT_H
