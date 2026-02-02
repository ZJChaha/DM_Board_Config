

#include "vofa.h"
#include "usbd_cdc_if.h"
#include "usbd_core.h"
#include "usbd_cdc.h"
#include "math.h"
#include "usart.h"
#include "All_struct.h"
#include "Direct_Drive_Tech.h"
#include "Chassis_Task.h"
#include "MC6C_RC.h"
#include "bsp_can.h"
#include "bsp_usart.h"
#include "kalman_filter.h"
#include "QuaternionEKF.h"
#include "imu_temp_ctrl.h"
#include "pid.h"
#include "MahonyAHRS.h"
#define MAX_BUFFER_SIZE 1024
uint8_t send_buf[MAX_BUFFER_SIZE];
uint16_t cnt = 0;

/**
***********************************************************************
* @brief:      vofa_start(void)
* @param:		   void
* @retval:     void
* @details:    发送数据给上位机
***********************************************************************
**/
void vofa_start(void)
{
    vofa_demo();		// demo示例

//	vofa_sendframetail();
}

/**
***********************************************************************
* @brief:      vofa_transmit(uint8_t* buf, uint16_t len)
* @param:		void
* @retval:     void
* @details:    修改通信工具，USART或者USB
***********************************************************************
**/
void vofa_transmit(uint8_t* buf, uint16_t len)
{
	HAL_UART_Transmit_DMA(&huart7, (uint8_t *)buf, len);
//    CDC_Transmit_HS((uint8_t *)buf, len);
}
/**
***********************************************************************
* @brief:      vofa_send_data(float data)
* @param[in]:  num: 数据编号 data: 数据
* @retval:     void
* @details:    将浮点数据拆分成单字节
***********************************************************************
**/
void vofa_send_data(uint8_t num, float data)
{
    send_buf[cnt++] = byte0(data);
    send_buf[cnt++] = byte1(data);
    send_buf[cnt++] = byte2(data);
    send_buf[cnt++] = byte3(data);
}
/**
***********************************************************************
* @brief      vofa_sendframetail(void)
* @param      NULL
* @retval     void
* @details:   给数据包发送帧尾
***********************************************************************
**/
void vofa_sendframetail(void)
{
    send_buf[cnt++] = 0x00;
    send_buf[cnt++] = 0x00;
    send_buf[cnt++] = 0x80;
    send_buf[cnt++] = 0x7f;

    /* 将数据和帧尾打包发送 */
    vofa_transmit((uint8_t *)send_buf, cnt);
    cnt = 0;// 每次发送完帧尾都需要清零
}
/**
***********************************************************************
* @brief      vofa_demo(void)
* @param      NULL
* @retval     void
* @details:   demo示例
***********************************************************************
**/
void vofa_demo(void)
{


//    float demo_test=(float)MC_RC.ch[0];
//	vofa_send_data(0, demo_test);
//    float demo_test2=(float)MC_RC.ch[1];
//	vofa_send_data(1,demo_test2 );
//    float demo_test3=(float)MC_RC.ch[2];
//    vofa_send_data(2,demo_test3);
//
//    float demo_test4=(float)MC_RC.ch[3];
//    vofa_send_data(3,demo_test4);
//
//    float demo_test5=(float)MC_RC.ch[4];
//    vofa_send_data(4,demo_test5);
//
//    float demo_test6=(float)MC_RC.ch[5];
//    vofa_send_data(5,demo_test6);

//float demo_test=(float)motor_gimbal[0].temperate;
//    vofa_send_data(0,test_motor.vel);
//	vofa_send_data(1,test_motor.pos);
//    vofa_send_data(2,test_motor.torque);
//    vofa_send_data(3,test_motor.turns);
//    vofa_send_data(4,test_motor.muti_turns_pos);
//    vofa_send_data(5,test_current);

    vofa_send_data(0,QEKF_INS.Pitch);
	vofa_send_data(1,QEKF_INS.Roll);
    vofa_send_data(2,QEKF_INS.Yaw);


    vofa_sendframetail();
}











