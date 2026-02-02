//
// Created by ZJC on 2024/4/19.
//
#include "Chassis_Task.h"
#include "Direct_Drive_Tech.h"
#include "cmsis_os.h"
#include "arm_math.h"
#include "Buzzer_task.h"
#include "kalman.h"
#include "All_struct.h"
#include "bsp_can.h"
#include "QuaternionEKF.h"
#include "stdlib.h"
#include "stdio.h"
#include "tim.h"
#include "pid.h"
#include "bsp_usart.h"

uint8_t data_temp[8]={1,2,3,4,5,5,5,5};

uint8_t DDT_test[10]={0x01,0x64, 0xEC, 0x78, 0x00 ,0x00, 0x00, 0x00 ,0x00 ,0xD3};

extKalman_t torque_k;


pid_type_def pos_pid_t;
pid_type_def vel_pid_t;
float pos_pid_para[3]={40,0,4.0f};
float vel_pid_para[3]={190,15,0};
test_motor_t test_motor;

float vel_cnt;//梯形加速使用的全局变量
short test_current;
uint8_t brake_flag;//刹车标志位
float brake_pos;//刹车时对应的电机角度
float set_vel,set_pos,set_acc,set_t_torque;
uint8_t Mode;
uint32_t test_cnt=0;

void Chassis_Task(void const * argument)
{
    osDelay(1000);

    data_temp[2] = 0;
    data_temp[3] = 0;
    data_temp[4] = 0;
    data_temp[5] = 0;
    data_temp[6] = 0;
    data_temp[7] = 0;
//    KalmanCreate(&torque_k,0.5f,0.5f);


    PID_init(&pos_pid_t,PID_POSITION,pos_pid_para,30,0);
    PID_init(&vel_pid_t,PID_POSITION,vel_pid_para, set_current(2.5f),set_current(1));


    test_current=0;

    for(;;)
    {
        //test_cnt++;
        //test_current=vel_control(&test_motor,33,6);
        //test_current=electric_brake(&test_motor,0.8f);
        //test_current=pos_vel_control(&test_motor,(float)debug_pid.set,3);

        switch (Mode)
        {
            case ZERO:
            {
                test_current=0;
            }break;
            case VEL:
            {
                test_current=vel_control(&test_motor,set_vel,set_acc);
            }break;
            case POS:
            {
                test_current=pos_vel_control(&test_motor,set_pos,set_vel);
            }break;
            case BRAKE:
            {
                test_current=electric_brake(&test_motor,set_t_torque);
            }break;

        }

        data_temp[0] = (test_current>>8);
        data_temp[1] =test_current;
        fdcanx_send_data(&hfdcan1,0x1FE,data_temp,8);
        osDelay(2);

    }
}

uint16_t angle_to_arr(float angle)
{
    return (uint16_t)(500+11.11f*angle);
}
short set_torque(float T)
{
    return (short)(T*CUR_CONSTANT/TORQUE_CONSTANT);
}
short set_current(float C)
{
    return (short)(C*CUR_CONSTANT);
}

short vel_control(test_motor_t *motor,float vel_set,float acc_set)//速度模式(梯形加速)
{
    LimitMax(acc_set,33.f)
    if(acc_set<0)
    {
        return (short)PID_calc(&vel_pid_t,motor->vel,vel_set);
    }
    else
    {
        if(vel_cnt<=vel_set)
        {
            vel_cnt += acc_set / 500.f;
        }
        return (short)PID_calc(&vel_pid_t,motor->vel,vel_cnt);
    }
}

short pos_vel_control(test_motor_t *motor,float pos_set,float vel_set)//位置速度模式(混合控制)
{
    float vel_temp;
    vel_temp=PID_calc(&pos_pid_t,motor->pos,pos_set);
    if(vel_set>0)
    {
        LimitMax(vel_temp, vel_set)
    }
    return (short)PID_calc(&vel_pid_t,motor->vel,vel_temp);
}

short electric_brake(test_motor_t *motor,float threshold_torque)//电刹车
{
    float vel_temp;
    short current_temp=0;
    if(brake_flag==0)
    {
        brake_flag=1;
        brake_pos=motor->muti_turns_pos;
    }
    vel_temp=PID_calc(&pos_pid_t,motor->muti_turns_pos,brake_pos);
    current_temp=(short)PID_calc(&vel_pid_t,motor->vel,vel_temp);
    LimitMax(current_temp, set_torque(threshold_torque))
    if(fabsf(motor->torque)>threshold_torque)
    {
        brake_pos=motor->muti_turns_pos;
    }
    return current_temp;
}

void muti_turns_pos_calc(test_motor_t *motor)
{
    float diff = motor->pos - motor->last_pos;
    if(diff < -PI)
    {
        motor->turns++;
    }
    else if(diff>PI)
    {
        motor->turns--;
    }
    motor->muti_turns_pos = motor->pos + motor->turns*2*PI;
}

void para_init(void)
{
    vel_cnt=0;
    brake_flag=0;

}