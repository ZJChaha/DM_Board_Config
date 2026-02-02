/**
  * @author  Liu heng
  * 卡尔曼滤波器来自RoboMaster论坛  
  */
  
#ifndef _KALMAN_H
#define _KALMAN_H
#include "All_struct.h"





//创建滤波器
void KalmanCreate(extKalman_t *p,float T_Q,float T_R);

//往滤波器塞数据
float KalmanFilter(extKalman_t* p,float dat);


//清空卡尔曼滤波器
void KalmanClear(extKalman_t *p);

#endif
