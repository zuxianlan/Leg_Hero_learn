
/**
  ******************************************************************************
  * @file           : Can_Comm_Task.c
  * @author         : Chen Haoran
  * @brief          : None
  * @attention      : None
  * @date           : 2026/1/9
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "Can_Comm_Task.h"
#include "arm_math.h"
#include "cmsis_os.h"
#include "math.h"
#include "FreeRTOS.h"
#include "referee.h"
#include "task.h"
/* Define --------------------------------------------------------------------*/

/* Enum ----------------------------------------------------------------------*/

/* Variable && Struct --------------------------------------------------------*/

Can_Comm_t Can_Comm;

/* Function Declaration ------------------------------------------------------*/
static void CAN_Comm_Init(Can_Comm_t *Can_Comm);

static void Can_Comm_Update(Can_Comm_t *Can_Comm);

static void Can_Comm_Out_Put(Can_Comm_t *Can_Comm);

/* Function ------------------------------------------------------------------*/


void Can_Comm_Task(void const *pvParameters)
{
    vTaskDelay(CAN_COMM_TASK_INIT_TIME);
    CAN_Comm_Init(&Can_Comm);
    while (1)
    {
        Can_Comm_Update(&Can_Comm);
        Can_Comm_Out_Put(&Can_Comm);
        osDelay(CAN_COMM_TASK_TIME);
    }
}

static void CAN_Comm_Init(Can_Comm_t *Can_Comm)
{
    Can_Referee_Data_Init(&Can_Comm->Can_Referee_Data, &hfdcan3, 0x20C);
}

static void Can_Comm_Update(Can_Comm_t *Can_Comm)
{
    if (robot_state.robot_id > 100)
    {
        switch (robot_state.robot_id)
        {
        case 101:
            Can_Comm->Can_Referee_Data.robot_id = Blue_Hero;
            break;
        case 102:
            Can_Comm->Can_Referee_Data.robot_id = Blue_Engineer;
            break;
        case 103:
            Can_Comm->Can_Referee_Data.robot_id = Blue_Infantry_3;
            break;
        case 104:
            Can_Comm->Can_Referee_Data.robot_id = Blue_Infantry_4;
            break;
        case 105:
            Can_Comm->Can_Referee_Data.robot_id = Blue_Infantry_5;
            break;
        case 107:
            Can_Comm->Can_Referee_Data.robot_id = Blue_Sentry;
            break;
        default:
            break;
        }
    }
    else
    {
        Can_Comm->Can_Referee_Data.robot_id = robot_state.robot_id;
    }

    Can_Comm->Can_Referee_Data.Can_Referee_Tx_Data.robot_id = robot_state.robot_id;
    Can_Comm->Can_Referee_Data.Can_Referee_Tx_Data.shoot_heat = power_heat_data.shooter_42mm_heat;
    Can_Comm->Can_Referee_Data.Can_Referee_Tx_Data.shoot_speed = (int16_t)(shoot_data.initial_speed * 1000.0f);
    Can_Comm->Can_Referee_Data.Can_Referee_Tx_Data.shoot_heat_limit = robot_state.shooter_heat_limit;
}

static void Can_Comm_Out_Put(Can_Comm_t *Can_Comm)
{
    static int mod = 0;
    mod++;

    if (mod == 20)
    {
        Can_Referee_Data_Out_Put(&Can_Comm->Can_Referee_Data);
        mod = 0;
    }
}