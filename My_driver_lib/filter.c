//
// Created by ZJC on 2024/10/11.
//
#include "filter.h"

LowPassFilter motor_angle_acc;
LowPassFilter motor_velocity;

void initializeFilter(LowPassFilter* filter, float alpha)
{
    filter->alpha = alpha;
    filter->previous_output = 0.0f;
}


float filterValue(LowPassFilter* filter, float input)
{

    float output = (1.0f - filter->alpha) * filter->previous_output + filter->alpha * input;

    filter->previous_output = output;

    return output;
}
