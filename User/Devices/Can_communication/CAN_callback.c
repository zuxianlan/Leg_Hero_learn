#include "CAN_callback.h"
#include "CAN_comm.h"
#include "ChassisL_Task.h"
#include "Can_Comm_Task.h"

/**
 * @brief FDCAN1报文回调函数
 *
 * @param Rx_Buffer FDCAN接收的信息结构体
 */
void CAN_Motor_Call_Back_FDCAN1(Struct_CAN_Rx_Buffer *Rx_Buffer)
{
    switch (Rx_Buffer->Header.Identifier)
    {
    case 0x11:
        Motor_DM_Normal_CAN_RxCpltCallback(&chassis_move.Motor_Joint[3], Rx_Buffer->Data);
        break;
    case 0x22:
        Motor_DM_Normal_CAN_RxCpltCallback(&chassis_move.Motor_Joint[2], Rx_Buffer->Data);
        break;
    case 0x33:
        Motor_DM_Normal_CAN_RxCpltCallback(&chassis_move.Motor_Joint[1], Rx_Buffer->Data);
        break;
    case 0x44:
        Motor_DM_Normal_CAN_RxCpltCallback(&chassis_move.Motor_Joint[0], Rx_Buffer->Data);
        break;
    default:
        break;
    }
}

/**
 * @brief FDCAN2报文回调函数
 *
 * @param Rx_Buffer FDCAN接收的信息结构体
 */
void CAN_Motor_Call_Back_FDCAN2(Struct_CAN_Rx_Buffer *Rx_Buffer)
{
    switch (Rx_Buffer->Header.Identifier)
    {
    case 0x201:
        Motor_C620_CAN_RxCpltCallback(&chassis_move.Motor_Wheel[1], Rx_Buffer->Data);
        break;
    case 0x202:
        Motor_C620_CAN_RxCpltCallback(&chassis_move.Motor_Wheel[0], Rx_Buffer->Data);
        break;
    case 0x210:
        // Can_Receive_Solution(&Can_Comm.Can_Remote_Data, Rx_Buffer->Data);
        // Motor_C620_CAN_RxCpltCallback(&chassis_move.Motor_Wheel[0], Rx_Buffer->Data);
        CAP_GetRxPacket(&chassis_move.Super_Cap_Rx, Rx_Buffer->Data);
        break;
    default:
        break;
    }
}

/**
 * @brief FDCAN3报文回调函数
 *
 * @param Rx_Buffer FDCAN接收的信息结构体
 */
void CAN_Motor_Call_Back_FDCAN3(Struct_CAN_Rx_Buffer *Rx_Buffer)
{
    switch (Rx_Buffer->Header.Identifier)
    {
    case 0x88:
        Can_Receive_Solution(&Can_Comm.Can_Remote_Data, Rx_Buffer->Data);
        break;
    case 0x99:
        Can_Control_Data_Solution(&Can_Comm.Can_Control_Data, Rx_Buffer->Data);
        break;
    case 0x100:
        Can_VT13_Data_Solution(&Can_Comm.Can_VT13_Data, Rx_Buffer->Data);
        break;
    case 0x110:
        Can_Motor_Status_Data_Solution(&Can_Comm.Can_Motor_Status_Data, Rx_Buffer->Data);
        break;
    case 0x55:
        Motor_DM_Normal_CAN_RxCpltCallback(&chassis_move.Motor_Yaw, Rx_Buffer->Data);
        break;
    default:
        break;
    }
}
