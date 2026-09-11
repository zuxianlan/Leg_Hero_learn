/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       can_receive.c/h
  * @brief      there is CAN interrupt function  to receive motor data,
  *             and CAN send function to send motor current to control motor.
  *             这里是CAN中断接收函数，接收电机数据,CAN发送函数发送电机电流控制电机.
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. done
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 DJI****************************
  */

#ifndef CAN_RECEIVE_H
#define CAN_RECEIVE_H

#include "stdint-gcc.h"
#include "fdcan.h"


#define CAN_CLASS   0
#define CAN_FD_BRS  1

#define CAN_BR_125K 0
#define CAN_BR_200K 1
#define CAN_BR_250K 2
#define CAN_BR_500K 3
#define CAN_BR_1M   4
#define CAN_BR_2M   5
#define CAN_BR_2M5  6
#define CAN_BR_3M2  7
#define CAN_BR_4M   8
#define CAN_BR_5M   9

// 滤波器编号
#define CAN_FILTER(x) ((x) << 3)

// 接收队列
#define CAN_FIFO_0 (0 << 2)
#define CAN_FIFO_1 (1 << 2)

//标准帧或扩展帧
#define CAN_STDID (0 << 1)
#define CAN_EXTID (1 << 1)

// 数据帧或遥控帧
#define CAN_DATA_TYPE (0 << 0)
#define CAN_REMOTE_TYPE (1 << 0)


extern uint8_t CAN_Send_Data(FDCAN_HandleTypeDef *hcan, uint16_t ID, uint8_t *Data, uint16_t Length);

typedef struct
{
    FDCAN_RxHeaderTypeDef Header;
    uint8_t Data[8];
} Struct_CAN_Rx_Buffer;

/**
 * @brief CAN通信接收回调函数数据类型
 *
 */
typedef void (*CAN_Call_Back)(Struct_CAN_Rx_Buffer *);

typedef struct
{
    FDCAN_HandleTypeDef *CAN_Handler;
    Struct_CAN_Rx_Buffer Rx_Buffer;
    CAN_Call_Back Callback_Function;
} Struct_CAN_Manage_Object;

extern Struct_CAN_Manage_Object CAN1_Manage_Object;
extern Struct_CAN_Manage_Object CAN2_Manage_Object;
extern Struct_CAN_Manage_Object CAN3_Manage_Object;

extern uint8_t CAN1_0x100_Tx_Data[];
extern uint8_t CAN1_0x1fe_Tx_Data[];
extern uint8_t CAN1_0x1ff_Tx_Data[];
extern uint8_t CAN1_0x200_Tx_Data[];
extern uint8_t CAN1_0x2fe_Tx_Data[];
extern uint8_t CAN1_0x2ff_Tx_Data[];
extern uint8_t CAN1_0x3fe_Tx_Data[];
extern uint8_t CAN1_0x4fe_Tx_Data[];

extern uint8_t CAN2_0x100_Tx_Data[];
extern uint8_t CAN2_0x1fe_Tx_Data[];
extern uint8_t CAN2_0x1ff_Tx_Data[];
extern uint8_t CAN2_0x200_Tx_Data[];
extern uint8_t CAN2_0x2fe_Tx_Data[];
extern uint8_t CAN2_0x2ff_Tx_Data[];
extern uint8_t CAN2_0x3fe_Tx_Data[];
extern uint8_t CAN2_0x4fe_Tx_Data[];

extern uint8_t CAN3_0x100_Tx_Data[];
extern uint8_t CAN3_0x1fe_Tx_Data[];
extern uint8_t CAN3_0x1ff_Tx_Data[];
extern uint8_t CAN3_0x200_Tx_Data[];
extern uint8_t CAN3_0x2fe_Tx_Data[];
extern uint8_t CAN3_0x2ff_Tx_Data[];
extern uint8_t CAN3_0x3fe_Tx_Data[];
extern uint8_t CAN3_0x4fe_Tx_Data[];

extern uint8_t CAN_Board_Tx_Data[];
extern uint8_t CAN_REFREE_Tx_Data[];
extern uint8_t CAN_Super_Cap_Tx_Data[];

void CAN_Init(FDCAN_HandleTypeDef *hcan, CAN_Call_Back Callback_Function);

void CAN_Filter_Mask_Config(FDCAN_HandleTypeDef *hcan, uint8_t Object_Para, uint32_t ID, uint32_t Mask_ID);

void TIM_CAN_PeriodElapsedCallback(void);

void FDCAN_Set_Baud(FDCAN_HandleTypeDef *hfdcan, uint8_t mode, uint8_t baud);

#endif