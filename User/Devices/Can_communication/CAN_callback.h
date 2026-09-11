#ifndef CAN_CALLBACK_H__
#define CAN_CALLBACK_H__


#include "main.h"
// #include "motor_dji.h"
#include "motor_dm.h"
#include "CAN_callback.h"
#include "open_source_4010.h"
#include "CAN_comm.h"


void CAN_Motor_Call_Back_FDCAN1(Struct_CAN_Rx_Buffer *Rx_Buffer);

void CAN_Motor_Call_Back_FDCAN2(Struct_CAN_Rx_Buffer *Rx_Buffer);

void CAN_Motor_Call_Back_FDCAN3(Struct_CAN_Rx_Buffer *Rx_Buffer);

#endif
