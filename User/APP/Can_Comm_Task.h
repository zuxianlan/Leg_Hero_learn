/**
  ******************************************************************************
  * @file           : Can_Comm_Task.h
  * @author         : Chen Haoran
  * @brief          : None
  * @attention      : None
  * @date           : 2026/1/9
  ******************************************************************************
  */
#ifndef CTRLBOARD_H7_IMU_CAN_COMM_TASK_H
#define CTRLBOARD_H7_IMU_CAN_COMM_TASK_H
/* Includes ------------------------------------------------------------------*/
#include "can_comm.h"
/* Define --------------------------------------------------------------------*/
//can通信任务初始化时间 单位ms
#define CAN_COMM_TASK_INIT_TIME 2000
//can通信任务运行时间间隔 单位ms
#define CAN_COMM_TASK_TIME 1
/* Enum ----------------------------------------------------------------------*/
typedef enum
{
    Blue_Hero = 11,
    Blue_Engineer,
    Blue_Infantry_3,
    Blue_Infantry_4,
    Blue_Infantry_5,
    Blue_Sentry,
}Robot_Blue_id;
/* Variable && Struct --------------------------------------------------------*/

/* Function Declaration ------------------------------------------------------*/

/* Function ------------------------------------------------------------------*/


typedef struct
{
    //控制信息结构体
    Can_Remote_Data_t Can_Remote_Data;

    Can_Control_Data_t Can_Control_Data;

    Can_Motor_Status_Data_t Can_Motor_Status_Data;

    Can_VT13_Data_t Can_VT13_Data;

    Can_Referee_Data_t Can_Referee_Data;
} Can_Comm_t;

extern Can_Comm_t Can_Comm;

#endif //CTRLBOARD_H7_IMU_CAN_COMM_TASK_H
