/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       usb_task.c/h
  * @brief      usb outputs the error message.usb输出错误信息
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Nov-11-2019     RM              1. done
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 DJI****************************
  */
#include "USB_task.h"

#include "cmsis_os.h"

#include "usb_device.h"
#include "usbd_cdc_if.h"
#include <stdio.h>
#include <stdarg.h>
#include "string.h"
#include "vofa.h"
#include "bsp_usart.h"
#include "usart.h"


static void usb_printf(const char *fmt,...);

static uint8_t usb_buf[256];



//引入需要观测的变量
/************************************************/

float chassis_power,chassis_buffer;

/***********************************************/


void USB_task(void const * argument)
{
	
	  osDelay(800);

    MX_USB_DEVICE_Init();




float temp_1,temp_2,temp_3,temp_4,voltage;

    while(1)
    {
        osDelay(10);

        //usb_printf("test\r\n");

        vofa_start();

    }

}

static void usb_printf(const char *fmt,...)
{
    static va_list ap;
    uint16_t len = 0;

    va_start(ap, fmt);

    len = vsprintf((char *)usb_buf, fmt, ap);

    va_end(ap);


    CDC_Transmit_HS(usb_buf, len);
}

uint8_t data[11]={0};

void bluetooth_debug(float* set,float* ref)
{

    data[0]=0xA5;
    memcpy(&data[1],set,4);
    memcpy(&data[5],ref,4);

data[9]=data[1]+data[2]+data[3]+data[4]+data[5]+data[6]+data[7]+data[8];

    data[10]=0x5A;
    HAL_UART_Transmit_DMA(&huart1,data,11);
}
