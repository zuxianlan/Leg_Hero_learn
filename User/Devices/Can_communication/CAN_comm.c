/**
 * @file can_comm.c
 * @author yuanluochen,CHR
 * @brief 多设备通信模块，主要用于控制板之间的通信，使用can总线实现
 * @version 0.1
 * @date 2023-09-17
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "CAN_comm.h"
#include "ChassisL_Task.h"
#include "Host_Comm_Task.h"
#include "UI_Task.h"


void Can_Receive_Solution(Can_Remote_Data_t *can_comm, uint8_t *Rx_Data)
{
    Can_Remote_Rx_Data_t *tmp_buff = (Can_Remote_Rx_Data_t *) Rx_Data;

    can_comm->Rx_Data.ch_1 = tmp_buff->ch_1;
    can_comm->Rx_Data.ch_2 = tmp_buff->ch_2;
    can_comm->Rx_Data.ch_3 = tmp_buff->ch_3;
    can_comm->Rx_Data.sw = tmp_buff->sw;

    rc_ctrl.RC.ch[1] = ((float) (can_comm->Rx_Data.ch_1) / 10000.0f) - 2.0f;
    rc_ctrl.RC.ch[2] = ((float) (can_comm->Rx_Data.ch_2) / 10000.0f) - 2.0f;
    rc_ctrl.RC.ch[3] = ((float) (can_comm->Rx_Data.ch_3) / 10000.0f) - 2.0f;
    rc_ctrl.RC.sw[1] = can_comm->Rx_Data.sw / 1000;
    rc_ctrl.RC.sw[2] = can_comm->Rx_Data.sw / 100 % 10;
    rc_ctrl.RC.sw[3] = can_comm->Rx_Data.sw / 10 % 10;
    rc_ctrl.RC.sw[4] = can_comm->Rx_Data.sw % 10;
}



void Can_Motor_Status_Data_Solution(Can_Motor_Status_Data_t *can_comm, uint8_t *Rx_Data)
{
    Can_Motor_Status_Rx_Data_t tmp_buff;
    memcpy(&tmp_buff, Rx_Data, sizeof(Can_Motor_Status_Rx_Data_t));
    can_comm->Rx_Data.left_fric_motor_status = tmp_buff.left_fric_motor_status;
    can_comm->Rx_Data.right_fric_motor_status = tmp_buff.right_fric_motor_status;
    can_comm->Rx_Data.yaw_motor_status = tmp_buff.yaw_motor_status;
    can_comm->Rx_Data.pitch_motor_status = tmp_buff.pitch_motor_status;
    can_comm->Rx_Data.trigger_motor_status = tmp_buff.trigger_motor_status;
    can_comm->Rx_Data.pitch_angle = tmp_buff.pitch_angle;

    UI_draw.pitch_angle=can_comm->Rx_Data.pitch_angle;

}


void Can_Control_Data_Solution(Can_Control_Data_t *can_comm, uint8_t *Rx_Data)
{
    Can_Control_Rx_Data_t tmp_buff;
    memcpy(&tmp_buff, Rx_Data, sizeof(Can_Control_Rx_Data_t));
    can_comm->Rx_Data.key = tmp_buff.key;
    can_comm->Rx_Data.flag = tmp_buff.flag;
    can_comm->Rx_Data.trigger_status = tmp_buff.trigger_status;
    can_comm->Rx_Data.robot_move_flag = tmp_buff.robot_move_flag;
    can_comm->Rx_Data.robot_control_status = tmp_buff.robot_control_status;
    can_comm->Rx_Data.fric_speed_set_average = tmp_buff.fric_speed_set_average;

    Host_communication.Tx_data.key = can_comm->Rx_Data.key;
    Host_communication.init_flag = can_comm->Rx_Data.flag / 10 % 10;

    UI_draw.vision_flag = can_comm->Rx_Data.flag % 10;
    UI_draw.fric_flag = can_comm->Rx_Data.flag / 100 % 10;
    UI_draw.auto_aim_flag = can_comm->Rx_Data.flag /1000;
    UI_draw.trigger_status = can_comm->Rx_Data.trigger_status;
    UI_draw.fric_speed_set_average = can_comm->Rx_Data.fric_speed_set_average;
}

void Can_VT13_Data_Solution(Can_VT13_Data_t *can_comm, uint8_t *Rx_Data)
{
    Can_VT13_Rx_Data_t *tmp_buff = (Can_VT13_Rx_Data_t *) Rx_Data;

    can_comm->Rx_Data.ch_1 = tmp_buff->ch_1;
    can_comm->Rx_Data.ch_2 = tmp_buff->ch_2;
    can_comm->Rx_Data.ch_3 = tmp_buff->ch_3;
    can_comm->Rx_Data.sw = tmp_buff->sw;

    chassis_move.Chassis_RC.RC.ch[1] = ((float) can_comm->Rx_Data.ch_1 / 10000.0f) - 2.0f;
    chassis_move.Chassis_RC.RC.ch[2] = ((float) can_comm->Rx_Data.ch_2 / 10000.0f) - 2.0f;
    chassis_move.Chassis_RC.RC.ch[3] = ((float) can_comm->Rx_Data.ch_3 / 10000.0f) - 2.0f;

    chassis_move.Chassis_RC.RC.sw[1] = can_comm->Rx_Data.sw / 1000;
    chassis_move.Chassis_RC.RC.sw[2] = can_comm->Rx_Data.sw / 100 % 10;
    chassis_move.Chassis_RC.RC.sw[3] = can_comm->Rx_Data.sw / 10 % 10;
    chassis_move.Chassis_RC.RC.sw[4] = can_comm->Rx_Data.sw % 10;
}

void Can_Referee_Data_Init(Can_Referee_Data_t *Can_Referee_Data, FDCAN_HandleTypeDef *hdfcan, uint16_t __CAN_Tx_ID)
{
    if(hdfcan->Instance == FDCAN1)
    {
        Can_Referee_Data->Can_Manage_Object = &CAN1_Manage_Object;
    }
    else if(hdfcan->Instance == FDCAN2)
    {
        Can_Referee_Data->Can_Manage_Object = &CAN2_Manage_Object;
    }
    else if(hdfcan->Instance == FDCAN3)
    {
        Can_Referee_Data->Can_Manage_Object = &CAN3_Manage_Object;
    }

    Can_Referee_Data->CAN_Tx_ID = __CAN_Tx_ID;
    Can_Referee_Data->Tx_Data = CAN_Board_Tx_Data;
}

void Can_Referee_Data_Out_Put(Can_Referee_Data_t *Can_Referee_Data)
{
    Can_Referee_Tx_Data_t *tmp_buff = (Can_Referee_Tx_Data_t *) Can_Referee_Data->Tx_Data;

    tmp_buff->robot_id = Can_Referee_Data->Can_Referee_Tx_Data.robot_id;
    tmp_buff->shoot_heat = Can_Referee_Data->Can_Referee_Tx_Data.shoot_heat;
    tmp_buff->shoot_speed = Can_Referee_Data->Can_Referee_Tx_Data.shoot_speed;
    tmp_buff->shoot_heat_limit = Can_Referee_Data->Can_Referee_Tx_Data.shoot_heat_limit;

    CAN_Send_Data(Can_Referee_Data->Can_Manage_Object->CAN_Handler, Can_Referee_Data->CAN_Tx_ID, Can_Referee_Data->Tx_Data, 8);
}