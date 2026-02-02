//
// Created by ZJC on 2024/4/24.
//

#include "Key_Task.h"
#include "bsp_key.h"
#include "key_driver.h"
#include "cmsis_os.h"
#include "adc_modlue.h"
#include "All_struct.h"




float vbus;



void Key_Task(void const * argument)
{

    osDelay(200);




    for(;;)
    {
//        get_key_adc();
//        key_process();
//        vbus = (adc_val[0]*3.3f/4096)*11.0f;


        osDelay(1);

    }

}


