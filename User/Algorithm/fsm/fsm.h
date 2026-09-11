/**
 * @file fsm.h
 */

#ifndef ALG_FSM_H
#define ALG_FSM_H

#include "Mathh.h"

#define STATUS_MAX (10)

/**
  * @brief 状态所处的阶段
  *
  */
typedef enum
{
    Status_Stage_DISABLE = 0,
    Status_Stage_ENABLE,
} Enum_Status_Stage;

/**
  * @brief 状态结构体
  *
  */
typedef struct
{
    Enum_Status_Stage Status_Stage;
    uint32_t Count_Time;
} Struct_Status;

typedef struct
{
    // 有限状态机状态变量结构体
    Struct_Status Status[STATUS_MAX];
    // 状态数量
    uint8_t Status_Number;
    // FSM当前状态
    uint8_t Now_Status;
} FSM_t;

void FSM_Init(FSM_t *FSM, uint8_t __Status_Number, uint8_t __Now_Status_Serial);

uint8_t FSM_Get_Now_Status(FSM_t *FSM);

void FSM_Set_Status(FSM_t *FSM, uint8_t Next_Status);

#endif