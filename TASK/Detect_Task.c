//
// Created by ZJC on 2024/10/25.
//

#include "All_struct.h"
#include "bsp_can.h"
#include "Chassis_Task.h"
#include "Detect_Task.h"
#include "cmsis_os.h"
#include "Buzzer_task.h"



uint8_t update_flag=0;


uint32_t cnt_detect;
uint32_t cnt_locked;

void Detect_Task(void const * argument)
{


    osDelay(1000);
uint8_t temp_flag_1,temp_flag_2;
    for(;;)
    {
/************************************离线检测************************************/
        if(update_flag==0)
            cnt_detect++;
        else
            cnt_detect=0;

        if(cnt_detect>9){
            test_motor.err_code=dis_online;
            temp_flag_1=1;
        }
        else
            temp_flag_1=0;
/*******************************************************************************/

        if(Mode==POS || Mode==VEL)
        {
            if(test_motor.vel<1.f && test_motor.torque>0.4)
            {cnt_locked++;}
            else
            {cnt_locked=0;}

            if(cnt_locked>20)
            {
                test_motor.err_code=locked;
                temp_flag_2=1;
            }
            else
            {
                temp_flag_2=0;
            }
        }

        if(temp_flag_1==0 && temp_flag_2==0)
        {
            test_motor.err_code=0;

        }



        switch(test_motor.err_code)
        {
            case 0:
                buzzer_flag=0;
                break;
            case locked:
                buzzer_flag=3;
                break;
            case dis_online:
                buzzer_flag=2;
                break;
        }





        update_flag=0;
        osDelay(10);


    }
}
