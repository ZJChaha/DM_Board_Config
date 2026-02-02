//
// Created by ZJC on 2024/4/21.
//

#include "LCD_Task.h"
#include "cmsis_os.h"


#include "WeChat.h"
//#include "lcd.h"
#include "remote_control.h"
#include "pic.h"



void LCD_Task(void const * argument)
{



//    osDelay(200);
//    LCD_Init();//LCD初始化
//    LCD_Address_Set(0,0,239,279);
//    LCD_Fill(0,0,LCD_W, LCD_H,BLACK);
//
//    for(uint16_t i=0;i<=12;i++)
//    {
//        LCD_Show_Reduced_Picture(gImage_success, 238, 278, (float)i/12.f,(float)i/12.f);
//        video_memory_update();
//        osDelay(1);
//    }

    for(;;)
    {

/************************************测试程序*********************************************
        LCD_Fill(150,150,230,200,GREEN);
        LCD_DrawLine(10,10,50,50,WHITE);
        LCD_ShowIntNum(50,100,1000,4,WHITE,BLACK,32);
        LCD_ShowFloatNum(50,200,12.34f,3,2,WHITE,BLACK,32);
        LCD_ShowString(150, 200,(uint8_t *)"dmBot", BRRED, BLACK, 24, 1);
        LCD_ShowChinese(84, 100,(uint8_t *)"达妙科技" , WHITE, BLACK,32);(float)video_test/100
****************************************************************************************/



//        video_memory_update();
        osDelay(1);

    }

}

