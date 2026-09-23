/**
  ******************************************************************************
  * @file           : Host_Comm_Task.c
  * @author         : Chen Haoran
  * @brief          : None
  * @attention      : None
  * @date           : 2025/11/1
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "Host_Comm_Task.h"
/* Define --------------------------------------------------------------------*/

/* Enum ----------------------------------------------------------------------*/

/* Variable && Struct --------------------------------------------------------*/
extern vmc_leg_t left_leg;
extern vmc_leg_t right_leg;

Host_communication_t Host_communication;

int error_time = 0;
float tmp_pos[5] = {};

/* Function Declaration ------------------------------------------------------*/
static void Host_Transmit(Host_communication_t *data);

static void Host_Transmit_judge_error(Host_communication_t *data);

uint8_t ceshi_pai=0;

/* Function ------------------------------------------------------------------*/


void Host_Comm_Task()
{
    osDelay(2000);
    while (1)
    {
        // Host_Transmit(&Host_communication);
        // Host_Transmit_judge_error(&Host_communication);
        //
        // error_time++;

        osDelay(1);
    }
}


static void Host_Transmit(Host_communication_t *data)
{
    Host_send_data_t *tmp_buff = (Host_send_data_t *) data->Tx_buffer;

    tmp_buff->header = 0x5A;

    tmp_buff->gyro[0] = BMI088.Gyro[0];
    tmp_buff->gyro[1] = -BMI088.Gyro[1];
    tmp_buff->gyro[2] = -BMI088.Gyro[2];
    tmp_buff->accel[0] = BMI088.Accel[0];
    tmp_buff->accel[1] = -BMI088.Accel[1];
    tmp_buff->accel[2] = -BMI088.Accel[2];

    tmp_buff->RC_ctrl.ch[1] = rc_ctrl.RC.ch[1];
    tmp_buff->RC_ctrl.ch[2] = rc_ctrl.RC.ch[2];
    tmp_buff->RC_ctrl.ch[3] = rc_ctrl.RC.ch[3];
    tmp_buff->RC_ctrl.sw[1] = rc_ctrl.RC.sw[1];
    tmp_buff->RC_ctrl.sw[2] = rc_ctrl.RC.sw[2];
    tmp_buff->RC_ctrl.sw[3] = rc_ctrl.RC.sw[3];
    tmp_buff->RC_ctrl.sw[4] = rc_ctrl.RC.sw[4];

    tmp_pos[0] = normalizeAngleToPi_Robust(chassis_move.Motor_Joint[0].Rx_Data.Now_Angle);
    tmp_pos[1] = normalizeAngleToPi_Robust(chassis_move.Motor_Joint[1].Rx_Data.Now_Angle);
    tmp_pos[2] = normalizeAngleToPi_Robust(chassis_move.Motor_Joint[2].Rx_Data.Now_Angle);
    tmp_pos[3] = normalizeAngleToPi_Robust(chassis_move.Motor_Joint[3].Rx_Data.Now_Angle);
    tmp_pos[4] = normalizeAngleToPi_Robust(chassis_move.Motor_Yaw.Rx_Data.Now_Angle);

    tmp_buff->Motor_joint[0].Position = tmp_pos[0];
    tmp_buff->Motor_joint[1].Position = tmp_pos[1];
    tmp_buff->Motor_joint[2].Position = tmp_pos[2];
    tmp_buff->Motor_joint[3].Position = tmp_pos[3];
    tmp_buff->Motor_wheel[0].Position = chassis_move.Motor_Wheel[0].Rx_Data.Now_Angle;
    tmp_buff->Motor_wheel[1].Position = chassis_move.Motor_Wheel[1].Rx_Data.Now_Angle;

    tmp_buff->Motor_joint[0].omega = chassis_move.Motor_Joint[0].Rx_Data.Now_Omega;
    tmp_buff->Motor_joint[1].omega = chassis_move.Motor_Joint[1].Rx_Data.Now_Omega;
    tmp_buff->Motor_joint[2].omega = chassis_move.Motor_Joint[2].Rx_Data.Now_Omega;
    tmp_buff->Motor_joint[3].omega = chassis_move.Motor_Joint[3].Rx_Data.Now_Omega;
    tmp_buff->Motor_wheel[0].omega = chassis_move.Motor_Wheel[0].Rx_Data.Now_Omega;
    tmp_buff->Motor_wheel[1].omega = chassis_move.Motor_Wheel[1].Rx_Data.Now_Omega;

    tmp_buff->yaw = INS.Yaw;
    tmp_buff->roll = INS.Roll - 0.017f;
    tmp_buff->pitch = INS.Pitch;

    // tmp_buff->current[0] = chassis_move.Motor_Wheel[0].Rx_Data.Now_Current;
    // tmp_buff->current[1] = chassis_move.Motor_Wheel[1].Rx_Data.Now_Current;

    Host_communication.Tx_data.motor_yaw_angle = tmp_pos[4];

    tmp_buff->motor_yaw_angle = Host_communication.Tx_data.motor_yaw_angle;
    tmp_buff->key = Host_communication.Tx_data.key;


    // ????CRC16????¦Â
    append_CRC16_check_sum(data->Tx_buffer, sizeof(Host_send_data_t) + 2);


    CDC_Transmit_HS(data->Tx_buffer, sizeof(Host_send_data_t) + 2);
}

void Host_Receive_Data(uint8_t *data, const uint32_t *length)
{
    if (data == NULL || *length < 2)
    {
        return;
    }
    //CRC§µ??
    if (verify_CRC16_check_sum(data, *length))
    {
        Host_receive_data_t *tmp_buff = (Host_receive_data_t *) data;

        if (tmp_buff->header == 0xA5)
        {
            Host_communication.Rx_data.torque[0] = tmp_buff->torque[0];
            Host_communication.Rx_data.torque[1] = tmp_buff->torque[1];
            Host_communication.Rx_data.torque[2] = tmp_buff->torque[2];
            Host_communication.Rx_data.torque[3] = tmp_buff->torque[3];
            Host_communication.Rx_data.wheel_torque[0] = tmp_buff->wheel_torque[0];
            Host_communication.Rx_data.wheel_torque[1] = tmp_buff->wheel_torque[1];
            Host_communication.Rx_data.spin_flag = tmp_buff->spin_flag;

            error_time = 0;

            ceshi_pai=0;
        }
    }
}

static void Host_Transmit_judge_error(Host_communication_t *data)
{
    if (error_time > 300)
    {
        ceshi_pai=1;

        Host_communication.Rx_data.torque[0] = 0.0f;
        Host_communication.Rx_data.torque[1] = 0.0f;
        Host_communication.Rx_data.torque[2] = 0.0f;
        Host_communication.Rx_data.torque[3] = 0.0f;
        Host_communication.Rx_data.wheel_torque[0] = 0.0f;
        Host_communication.Rx_data.wheel_torque[1] = 0.0f;
        Host_communication.Rx_data.spin_flag = 0;
    }

    data->Key_B.key_state = data->Tx_data.key & KEY_PRESSED_OFFSET_B;
    process_key(&data->Key_B);
}
