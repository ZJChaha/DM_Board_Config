//
// Created by ZJC on 2024/10/25.
//

#ifndef DM_BOARD_CONFIG_DETECT_TASK_H
#define DM_BOARD_CONFIG_DETECT_TASK_H


enum ERR
{
    locked=1,
    high_temp,
    dis_online

};



extern uint8_t update_flag;







void Detect_Task(void const * argument);




#endif //DM_BOARD_CONFIG_DETECT_TASK_H
