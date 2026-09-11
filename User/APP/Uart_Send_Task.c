#include "Uart_Send_Task.h"
#include "cmsis_os.h"
#include "vofa.h"
#include "ChassisL_Task.h"
#include "led_flow_task.h"
#include "bsp_usart.h"
#include "Can_Comm_Task.h"
#include "CRC8_CRC16.h"
#include "INS_task.h"
#include "Transmission_link.h"
#include "Host_Comm_Task.h"
#include "UI_Task.h"
#include "usbd_cdc_if.h"
#include "VMC&LQR_Calc.h"

// 全局缓冲区，VOFA最多16个通道
float vofa_buf[15];
void UART_Send_feedback_update(float data[]);
/**
  * @brief          串口发送任务
  * @param[in]      pvParameters: NULL
  * @retval         none
  */
void Other_Task(void)
{
    osDelay(2000);
    VOFA_Init(&huart7);
    while (1)
    {
        LED_Flow();
        UART_Send_feedback_update(vofa_buf);
        osDelay(5);
    }
}

void UART_Send_feedback_update(float data[])
{
    //data[0] = chassis_move.chassis_INS_point->Roll;
    data[1] = chassis_move.chassis_INS_point->Pitch;
    VOFA_Transmit_JustFloat(data,15);
}

// void UART_Send_feedback_update(SEND_Message *sendMessage)
// {
//     sendMessage->v6 = Host_communication.Rx_data.torque[3];
//     sendMessage->v7 = Host_communication.Rx_data.torque[2];
//     sendMessage->v8 = Host_communication.Rx_data.torque[1];
//     sendMessage->v9 = Host_communication.Rx_data.torque[0];
//     sendMessage->v10 = Host_communication.Rx_data.wheel_torque[0];
//
//     sendMessage->v0 = chassis_move.Motor_Joint[3].Control_Torque;
//     sendMessage->v1 = chassis_move.Motor_Joint[2].Control_Torque;
//     sendMessage->v2 = chassis_move.Motor_Joint[1].Control_Torque;
//     sendMessage->v3 = chassis_move.Motor_Joint[0].Control_Torque;
//     sendMessage->v4 = chassis_move.Motor_Wheel[0].Target_Current;
//
//     sendMessage->v5 = Can_Comm.Can_Control_Data.Rx_Data.robot_control_status;
//
//     sendMessage->v11 = ceshi_pai;
//     sendMessage->v12 = Host_communication.init_flag;
//     sendMessage->v13 = robot_state.chassis_output;
//     sendMessage->v14 = chassis_move.Motor_Wheel[0].CAN_Motor_Status;
//     sendMessage->v15 = chassis_move.Motor_Wheel[0].CAN_Motor_Status;
//
//     // sendMessage->v8  = Host_communication.Key_B.key_state;
//     // sendMessage->v9  = UI_draw.UI_status;
//     // sendMessage->v10 = UI_draw.jingtai_flag;
//     // sendMessage->v11 = UI_draw.dongtai_flag;
//     // sendMessage->v12 = 0;
//     // sendMessage->v13 = 0;
//
//     // sendMessage->v0 = chassis_move.Motor_Joint[0].Rx_Data.Now_Angle;
//     // sendMessage->v1 = chassis_move.Motor_Joint[1].Rx_Data.Now_Angle;
//     // sendMessage->v2 = chassis_move.Motor_Joint[2].Rx_Data.Now_Angle;
//     // sendMessage->v3 = chassis_move.Motor_Joint[3].Rx_Data.Now_Angle;
//     // sendMessage->v4 = rc_ctrl.RC.sw[2];
//     // sendMessage->v5 = rc_ctrl.RC.sw[3];
//     // sendMessage->v6 = rc_ctrl.RC.sw[4];
//     // sendMessage->v7 = chassis_move.Motor_Wheel[0].Rx_Data.Now_Angle;
//     // sendMessage->v8 = chassis_move.Motor_Wheel[0].Target_Current;
//     // sendMessage->v9 = chassis_move.Motor_Wheel[1].Target_Current;
//
//     // sendMessage->v10 = Can_Comm.Can_Motor_Status_Data.Rx_Data.pitch_angle;
//     // sendMessage->v11 = Can_Comm.Can_Motor_Status_Data.Rx_Data.left_fric_motor_status;
//     // sendMessage->v12 = Can_Comm.Can_Motor_Status_Data.Rx_Data.right_fric_motor_status;
//     // sendMessage->v13 = Can_Comm.Can_Motor_Status_Data.Rx_Data.trigger_motor_status;
//     // sendMessage->v14 = Can_Comm.Can_Motor_Status_Data.Rx_Data.yaw_motor_status;
//     // sendMessage->v15 = Can_Comm.Can_Motor_Status_Data.Rx_Data.pitch_motor_status;
//
//     // sendMessage->v10 = chassis_move.joint_motor_status[0];
//     // sendMessage->v11 = chassis_move.joint_motor_status[1];
//     // sendMessage->v12 = chassis_move.joint_motor_status[2];
//     // sendMessage->v13 = chassis_move.joint_motor_status[3];
//     // sendMessage->v14 = chassis_move.wheel_motor_status[0];
//     // sendMessage->v15 = chassis_move.wheel_motor_status[1];
//
//     // sendMessage->v0 = chassis_move.Motor_Joint[0].Rx_Data.Now_Angle;
//     // sendMessage->v1 = chassis_move.Motor_Joint[1].Rx_Data.Now_Angle;
//     // sendMessage->v2 = chassis_move.Motor_Joint[2].Rx_Data.Now_Angle;
//     // sendMessage->v3 = chassis_move.Motor_Joint[3].Rx_Data.Now_Angle;
//     // sendMessage->v5 = chassis_move.Motor_Wheel[0].Rx_Data.Now_Angle;
//     // sendMessage->v6 = chassis_move.Motor_Wheel[1].Rx_Data.Now_Angle;
//     //
//     // sendMessage->v8 = chassis_move.Super_Cap_Rx.cap_volt;
//     // sendMessage->v9 = chassis_move.Super_Cap_Rx.flag;
//     // sendMessage->v10 =chassis_move.Super_Cap_Rx.cap_target_power;
//     // sendMessage->v11 = chassis_move.Super_Cap_Rx.err;
//     // sendMessage->v12 = chassis_move.Super_Cap_Rx.flag;
//     // sendMessage->v13 = chassis_move.Super_Cap_Rx.status;
//     //
//     // sendMessage->v10 = robot_state.chassis_output;
//     // sendMessage->v11 = Host_communication.init_flag;
//     // sendMessage->v12 = Can_Comm.Can_Control_Data.Rx_Data.robot_control_status;
//     // sendMessage->v13 = chassis_move.Super_Cap_Rx.status;
//
//     // sendMessage->v14 = power_heat_data.shooter_42mm_heat;
//
//     // sendMessage->v8 = power_heat_data.shooter_42mm_heat;
//     // sendMessage->v9 = robot_state.robot_id;
//     // sendMessage->v10 = power_heat_data.shooter_42mm_heat;
//     // sendMessage->v11 = shoot_data.initial_speed ;
//     // sendMessage->v12 =robot_state.shooter_heat_limit;
//     // sendMessage->v13 =0;
// }