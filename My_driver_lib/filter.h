//
// Created by ZJC on 2024/10/11.
//

#ifndef DM_BOARD_CONFIG_FILTER_H
#define DM_BOARD_CONFIG_FILTER_H

typedef struct {
    float alpha;           // 时间常数
    float previous_output; // 上一时刻的输出
} LowPassFilter;


extern LowPassFilter motor_angle_acc;
extern LowPassFilter motor_velocity;

void initializeFilter(LowPassFilter* filter, float alpha);

float filterValue(LowPassFilter* filter, float input);





#endif //DM_BOARD_CONFIG_FILTER_H
