/**
 * @file fsm.c
 *
 */
#include "fsm.h"

/**
 * @brief 状态机初始化
 *
 * @param __Status_Number 状态数量
 * @param __Now_Status_Serial 当前指定状态机初始编号
 */
void FSM_Init(FSM_t *FSM, uint8_t __Status_Number, uint8_t __Now_Status_Serial)
{
    FSM->Status_Number = __Status_Number;
    FSM->Now_Status = __Now_Status_Serial;

    // 所有状态全刷0
    for (int i = 0; i < FSM->Status_Number; i++)
    {
        FSM->Status[i].Status_Stage = Status_Stage_DISABLE;
        FSM->Status[i].Count_Time = 0;
    }

    // 使能初始状态
    FSM->Status[__Now_Status_Serial].Status_Stage = Status_Stage_ENABLE;
}

/**
 * @brief 获取FSM当前状态
 *
 * @return uint8_t FSM当前状态
 */
uint8_t FSM_Get_Now_Status(FSM_t *FSM)
{
    return (FSM->Now_Status);
}

/**
  * @brief 设置状态改变
  *
  * @param Next_Status 下一个状态
  */
void FSM_Set_Status(FSM_t *FSM, uint8_t Next_Status)
{
    // 失能当前状态，计数清零
    FSM->Status[FSM->Now_Status].Status_Stage = Status_Stage_DISABLE;
    FSM->Status[FSM->Now_Status].Count_Time = 0;

    // 转到下一个状态
    FSM->Status[Next_Status].Status_Stage = Status_Stage_ENABLE;
    FSM->Now_Status = Next_Status;
}

/**
 * @brief 定时器处理函数, 计算周期与模型有关
 * 这是一个模板, 使用时请根据不同处理情况在不同文件内重新定义
 *
 */
void FSM_TIM_Calculate_PeriodElapsedCallback_tmp(FSM_t *FSM)
{
    FSM->Status[FSM->Now_Status].Count_Time++;

    // 自己接着编写状态转移函数
}