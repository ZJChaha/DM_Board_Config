//
// Created by ZJC on 2024/4/19.
//

#ifndef DM_BOARD_CONFIG_CHASSIS_TASK_H
#define DM_BOARD_CONFIG_CHASSIS_TASK_H
#include "stdint.h"
#include "All_struct.h"
#include "pid.h"
#include "bsp_can.h"

#define ZERO 0
#define VEL 1
#define POS 2
#define BRAKE 3


void Chassis_Task(void const * argument);

extern short test_current;
extern float test_vel;


extern float Torque;
extern float Torque_temp;
extern extKalman_t torque_k;
extern pid_type_def test_motor_pid;
extern float set_vel,set_pos,set_acc,set_t_torque;
uint16_t angle_to_arr(float angle);
extern uint8_t Mode;
extern uint8_t brake_flag;//刹车标志位
extern float vel_cnt;//梯形加速使用的全局变量
short set_torque(float T);
short set_current(float C);



short vel_control(test_motor_t *motor,float vel_set,float acc_set);
short pos_vel_control(test_motor_t *motor,float pos_set,float vel_set);
short electric_brake(test_motor_t *motor,float threshold_torque);
void muti_turns_pos_calc(test_motor_t *motor);
void para_init(void);






#endif //DM_BOARD_CONFIG_CHASSIS_TASK_H
