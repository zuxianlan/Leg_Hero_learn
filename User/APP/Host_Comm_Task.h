/**
******************************************************************************
  * @file           : Host_Comm_Task.h
  * @author         : Chen Haoran
  * @brief          : None
  * @attention      : None
  * @date           : 2025/11/1
  ******************************************************************************
  */
#ifndef CTRLBOARD_H7_IMU_HOST_COMM_TASK_H
#define CTRLBOARD_H7_IMU_HOST_COMM_TASK_H
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "INS_Task.h"
#include "ChassisL_Task.h"
#include "VMC&LQR_Calc.h"
#include "CRC8_CRC16.h"
#include "usbd_cdc_if.h"
#include "Transmission_link.h"
/* Define --------------------------------------------------------------------*/

/* Enum ----------------------------------------------------------------------*/

/* Variable && Struct --------------------------------------------------------*/

/* Function Declaration ------------------------------------------------------*/

/* Function ------------------------------------------------------------------*/

typedef struct
{
    float ch[5];
    float rotory_sw[3];
    char sw[5];
} RC_t;

typedef struct
{
    float Position;
    float omega;
} Motor_data_t;

typedef struct
{
    uint8_t header;

    float gyro[3];
    float accel[3];

    RC_t RC_ctrl;

    Motor_data_t Motor_joint[4];
    Motor_data_t Motor_wheel[2];

    float roll;
    float pitch;
    float yaw;
    float motor_yaw_angle;
    uint16_t key;
} Host_send_data_t;

typedef struct
{
    uint8_t header;

    float torque[4];
    float wheel_torque[2];
    uint8_t spin_flag;
} Host_receive_data_t;

typedef struct
{
    Host_send_data_t Tx_data;
    Host_receive_data_t Rx_data;

    uint8_t Tx_buffer[512];
    uint8_t Rx_buffer[512];

    float motor_yaw_angle;
    uint8_t init_flag;

    VT13_RC_ctrl_t VT13_RC_ctrl;

    Key_Pressed Key_B;
} Host_communication_t;

void Host_Receive_Data(uint8_t *data, const uint32_t *length);

extern Host_communication_t Host_communication;

extern uint8_t ceshi_pai;

#endif //CTRLBOARD_H7_IMU_HOST_COMM_TASK_H