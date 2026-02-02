#include <string.h>
#include "bsp_can.h"
#include "Buzzer_task.h"
#include "filter.h"
#include "All_struct.h"
#include "Chassis_Task.h"
#include "Detect_Task.h"

motor_measure_t motor_chassis[4];//底盘
motor_measure_t motor_gimbal[5];//云台


uint8_t IMU_enable_flag;
uint8_t IMU_disable_flag;


float last_velocity,velocity,motor_angle;
float angle_acc,acc_temp;





/**
************************************************************************
* @brief:      	can_bsp_init(void)
* @param:       void
* @retval:     	void
* @details:    	CAN 使能
************************************************************************
**/

#define get_motor_measure(ptr, data)                                    \
    {                                                                   \
        (ptr)->last_ecd = (ptr)->ecd;                                   \
        (ptr)->ecd = (uint16_t)((data)[0] << 8 | (data)[1]);            \
        (ptr)->speed_rpm = (uint16_t)((data)[2] << 8 | (data)[3]);      \
        (ptr)->given_current = (uint16_t)((data)[4] << 8 | (data)[5]);  \
        (ptr)->temperate = (data)[6];                                   \
    }



void can_bsp_init(void)
{
    can_filter_init();
    HAL_FDCAN_Start(&hfdcan1);                               //开启FDCAN
    HAL_FDCAN_Start(&hfdcan2);
    HAL_FDCAN_Start(&hfdcan3);
    HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
    HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
    HAL_FDCAN_ActivateNotification(&hfdcan3, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
}
/**
************************************************************************
* @brief:      	can_filter_init(void)
* @param:       void
* @retval:     	void
* @details:    	CAN滤波器初始化
************************************************************************
**/
void can_filter_init(void)
{
    FDCAN_FilterTypeDef fdcan_filter;

    fdcan_filter.IdType = FDCAN_STANDARD_ID;
    fdcan_filter.FilterIndex = 0;
    fdcan_filter.FilterType = FDCAN_FILTER_RANGE;
    fdcan_filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    fdcan_filter.FilterID1 = 0x100;
    fdcan_filter.FilterID2 = 0x500;
    HAL_FDCAN_ConfigFilter(&hfdcan1,&fdcan_filter);
    HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);
    HAL_FDCAN_ConfigFifoWatermark(&hfdcan1, FDCAN_CFG_RX_FIFO0, 1);
}
/**
************************************************************************
* @brief:      	fdcanx_send_data(FDCAN_HandleTypeDef *hfdcan, uint16_t id, uint8_t *data, uint32_t len)
* @param:       hfdcan：FDCAN句柄
* @param:       id：CAN设备ID
* @param:       data：发送的数据
* @param:       len：发送的数据长度
* @retval:     	void
* @details:    	发送数据
************************************************************************
**/
uint8_t fdcanx_send_data(FDCAN_HandleTypeDef *hfdcan, uint16_t id, uint8_t *data, uint32_t len)
{
    FDCAN_TxHeaderTypeDef TxHeader;

    TxHeader.Identifier = id;
    TxHeader.IdType = FDCAN_STANDARD_ID;																// 标准ID
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;														// 数据帧
    TxHeader.DataLength = len;																		// 发送数据长度
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;										// 设置错误状态指示
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;															// 不开启可变波特率
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;															// 普通CAN格式
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;										// 用于发送事件FIFO控制, 不存储
    TxHeader.MessageMarker = 0x00; 			// 用于复制到TX EVENT FIFO的消息Maker来识别消息状态，范围0到0xFF

    if(HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &TxHeader, data)!=HAL_OK)
        return 1;//发送
    return 0;
}

/**
************************************************************************
* @brief:      	fdcan_rx_callback(void)
* @param:       void
* @retval:     	void
* @details:    	供用户调用的接收弱函数
************************************************************************
**/
uint8_t rx_data1[8] = {0};
void fdcan1_rx_callback(void)
{
    fdcanx_receive(&hfdcan1, rx_data1);

}
uint8_t rx_data2[8] = {0};
void fdcan2_rx_callback(void)
{
    fdcanx_receive(&hfdcan2, rx_data2);
}
uint8_t rx_data3[8] = {0};
void fdcan3_rx_callback(void)
{
    fdcanx_receive(&hfdcan3, rx_data3);
}


/**
************************************************************************
* @brief:      	fdcanx_receive(FDCAN_HandleTypeDef *hfdcan, uint8_t *buf)
* @param:       hfdcan：FDCAN句柄
* @param:       buf：接收数据缓存
* @retval:     	接收的数据长度
* @details:    	接收数据
************************************************************************
**/

uint8_t imu_enablecmd[8]={'s','u','c','c','e','s','s',0x01};
uint8_t imu_disablecmd[8]={'s','u','c','c','e','s','s',0x00};
uint8_t fdcanx_receive(FDCAN_HandleTypeDef *hfdcan, uint8_t *buf)
{
    FDCAN_RxHeaderTypeDef fdcan_RxHeader;
    if(HAL_FDCAN_GetRxMessage(hfdcan,FDCAN_RX_FIFO0, &fdcan_RxHeader, buf)!=HAL_OK)
        return 0;//接收数据

    if(hfdcan==&hfdcan2)//注意这里本来是1
    {
        if(fdcan_RxHeader.DataLength==8)
        {
            switch (fdcan_RxHeader.Identifier)
            {
                case 0x2B:
                {
                    uint8_t cnt_temp=0;
                    for (int i = 0; i < 7; i++)
                    {
                         if(rx_data1[i]==imu_enablecmd[i])
                         {
                             cnt_temp++;
                         }
                    }

                    if(cnt_temp==7)
                    {
                        if(rx_data1[7]==0x01)
                        {IMU_enable_flag=1;
                            buzzer_flag=6;}
                        else
                        {IMU_enable_flag=0;
                        buzzer_flag=5;}
                    }

                }break;

            }
        }
    }
    else if(hfdcan==&hfdcan1)
    {
        if(fdcan_RxHeader.DataLength==8)
        {
            switch (fdcan_RxHeader.Identifier)
            {
                case 0x201:
                {
                    get_motor_measure(&motor_chassis[0], rx_data1);//拨弹测试临时更改
                }break;
                case 0x202:
                {
                    get_motor_measure(&motor_chassis[1], rx_data2);
                }break;
                case 0x203:
                {
                    get_motor_measure(&motor_chassis[2], rx_data2);
                }break;
                case 0x204:
                {
                    get_motor_measure(&motor_chassis[3], rx_data2);
                }break;
                case 0x205:
                {
                    get_motor_measure(&motor_gimbal[0], rx_data1);
                    test_motor.pos=convert(motor_gimbal[0].ecd);
                    muti_turns_pos_calc(&test_motor);
                    test_motor.vel=(float)motor_gimbal[0].speed_rpm*0.10472f;
                    test_motor.torque=(float)motor_gimbal[0].given_current*TORQUE_CONSTANT/CUR_CONSTANT;
                    test_motor.last_pos=test_motor.pos;
                    update_flag=1;
                }break;
                case 0x206:
                {
                    get_motor_measure(&motor_gimbal[1], rx_data2);
                }break;
                case 0x207:
                {
                    get_motor_measure(&motor_gimbal[2], rx_data2);
                }break;
                case 0x208:
                {
                    get_motor_measure(&motor_gimbal[3], rx_data2);
                }break;
            }
        }
    }

    return fdcan_RxHeader.DataLength;
}
/**
************************************************************************
* @brief:      	HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
* @param:       hfdcan；FDCAN句柄
* @param:       RxFifo0ITs：中断标志位
* @retval:     	void
* @details:    	HAL库的FDCAN中断回调函数
************************************************************************
**/
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
    {
        if(hfdcan == &hfdcan1)
        {
            fdcan1_rx_callback();
        }
        if(hfdcan == &hfdcan2)
        {
            fdcan2_rx_callback();
        }
        if(hfdcan == &hfdcan3)
        {
            fdcan3_rx_callback();
        }
    }
}



float convert(uint16_t raw_angle)
{
    float val;
    val=(float)raw_angle*360.0f/8191.0f-180.0f;
    return val/57.29578f;
}




