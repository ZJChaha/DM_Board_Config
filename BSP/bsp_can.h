#ifndef __BSP_CAN_H__
#define __BSP_CAN_H__
#include "main.h"
#include "fdcan.h"


//rm motor data
typedef struct
{
    uint16_t ecd;
    int16_t speed_rpm;
    int16_t given_current;
    uint8_t temperate;
    int16_t last_ecd;
} motor_measure_t;

typedef struct
{
    float pos;//位置
    float last_pos;//上一时刻位置
    float turns;//圈数
    float muti_turns_pos;//多圈位置
    float vel;//速度
    float torque;//力矩
    float temparature;//温度
    uint8_t err_code;//错误码
}test_motor_t;



typedef struct{
    uint16_t raw_value;
    uint16_t last_raw_value;
    int32_t diff;
    int32_t round_cnt;
    float ecd_angle;
}extend_angle_t;

extern uint8_t IMU_enable_flag;
extern uint8_t IMU_disable_flag;


extern motor_measure_t motor_chassis[4];//底盘
extern motor_measure_t motor_gimbal[5];//云台
extern test_motor_t test_motor;

extern float last_velocity,velocity,motor_angle;
extern float angle_acc,acc_temp;

void can_bsp_init(void);
void can_filter_init(void);
uint8_t fdcanx_send_data(FDCAN_HandleTypeDef *hfdcan, uint16_t id, uint8_t *data, uint32_t len);
uint8_t fdcanx_receive(FDCAN_HandleTypeDef *hfdcan, uint8_t *buf);
void fdcan1_rx_callback(void);
void fdcan2_rx_callback(void);
void fdcan3_rx_callback(void);

float convert(uint16_t raw_angle);



#endif /* __BSP_CAN_H_ */

