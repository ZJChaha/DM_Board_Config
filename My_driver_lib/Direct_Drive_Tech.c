//
// Created by ZJC on 2024/6/5.
//
#include "Direct_Drive_Tech.h"
#include "CRC8_CRC16.h"
#include "bsp_usart.h"
#include "cmsis_os.h"
#include "Buzzer_task.h"
#include "Chassis_Task.h"
#include "kalman.h"

DDT_M0603A_t DDT_M0603A;

uint8_t motor_enable_cmd[10]={0X01,0XA0,0X08,0X00,0X00,0X00,0X00,0X00,0X00,0X6F};//电机使能指令
/*电机控制模式指令*/
uint8_t openloop_cmd[10]={0X01,0XA0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X9E};//开环
uint8_t currentloop_cmd[10]={0X01,0XA0,0X01,0X00,0X00,0X00,0X00,0X00,0X00,0XA3};//电流环
uint8_t velocityloop_cmd[10]={0X01,0XA0,0X02,0X00,0X00,0X00,0X00,0X00,0X00,0XE4};//速度环
uint8_t positionloop_cmd[10]={0X01,0XA0,0X03,0X00,0X00,0X00,0X00,0X00,0X00,0XD9};//位置环
/*获取位置，里程圈数反馈指令*/
uint8_t feedback_cmd[10]={1,0x74,0,0,0,0,0,0,0,0x04};

uint8_t break_cmd[10]={0x01,0x64,0,0,0,0,0,0xFF,0,0xD1};//电机1刹车指令

uint8_t tx_buff[10]={0};
uint8_t enable_flag=0,send_flag=0,feedback_flag=0;
int cnt_tim_test=0;
float zero_angle=171.45f;

void Motor_enable(uint8_t id,uint8_t mode)
{
uint32_t test_cnt=0;
    while(1) {
        if (id == 1) {
            motor_enable_cmd[0] = 0x01;
            openloop_cmd[0] = 0x01;
            currentloop_cmd[0] = 0x01;
            velocityloop_cmd[0] = 0x01;
            positionloop_cmd[0] = 0x01;
        }
//        else if (id == 2) {
//            motor_enable_cmd[0] = 0x02;
//            openloop_cmd[0] = 0x02;
//            currentloop_cmd[0] = 0x02;
//            velocityloop_cmd[0] = 0x02;
//            positionloop_cmd[0] = 0x02;
//        }
        HAL_UART_Transmit(&huart7, motor_enable_cmd, 10,0xff);


        osDelay(20);
        if (enable_flag == 1)
            break;
        test_cnt++;
        if(test_cnt>250)
        {buzzer_flag=2;}
    }

//    HAL_Delay(100);
//
//    switch (mode)
//    {
//        case OPENLOOP:
//            HAL_UART_Transmit_DMA(&huart7,openloop_cmd,10);break;
//        case CURRENTLOOP:
//            HAL_UART_Transmit_DMA(&huart7,currentloop_cmd,10);break;
//        case VELOCITYLOOP:
//            HAL_UART_Transmit_DMA(&huart7,velocityloop_cmd,10);break;
//        case POSITIONLOOP:
//            HAL_UART_Transmit_DMA(&huart7,positionloop_cmd,10);break;
//    }



}
float NormalizeAngle(const float angle) {
    float a = fmodf(angle + 180.f, 2.0f * 180.f);
    if (a < 0.0f) {
        a += (2.0f * 180.f);
    }
    return a - 180.f;
}

void Motor_current_send(uint8_t id,short current)
{
    uint8_t high_eight_bits=current>>8;
    uint8_t low_eight_bits=current;


     tx_buff[0]=id;
    tx_buff[1]=0x64;
    tx_buff[2]=high_eight_bits;
    tx_buff[3]=low_eight_bits;
    append_CRC8_check_sum(tx_buff,10);

    HAL_UART_Transmit_DMA(&huart7,tx_buff,10);

}

void Motor_data_process(const uint8_t *data,DDT_M0603A_t *para)
{
    if(data[1]==0x65)
    {
        para->current=(short)(data[4]<<8|data[5]);
        para->temp=data[7];
        para->err_code=data[8];
        para->velocity=((float)((short)(data[2]<<8|data[3]))/10.f)*0.10732f;
        para->angular_acc=(para->velocity-para->last_velocity)*75.f;
        Torque_temp=0.005632f*DDT_M0603A.angular_acc;
        Torque=KalmanFilter(&torque_k,Torque_temp);

        para->last_velocity=para->velocity;
        //cnt_tim_test++;
        send_flag=1;
    }
    else if(data[1]==0x75)
    {
        para->turns=data[2]<<24|data[3]<<16|data[4]<<8|data[5];
        para->angle=NormalizeAngle(((float)(data[6]<<8|data[7])/32767)*360.f-180.f-zero_angle);
        para->err_code=data[8];
        feedback_flag=1;
    }
    else if(data[1]==0xA1 && data[2]==0x01)
    {
        enable_flag=1;
    }

}

void Get_other_feedback(uint8_t id)
{

    HAL_UART_Transmit_DMA(&huart7,feedback_cmd,10);

}

float calc_derivation(float val)
{

}

