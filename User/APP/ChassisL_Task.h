/**
  ******************************************************************************
  * @file           : ChassisL_Task.h
  * @author         : gagami
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/6
  ******************************************************************************
  */
#ifndef CHASSISL_TASK_H
#define CHASSISL_TASK_H

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"
#include "slope.h"
#include "motor_dm.h"
#include "remote_control.h"
#include "fsm.h"
#include "motor_dji.h"
#include "super_cap.h"
#include "VMC&LQR_Calc.h"

/* Define --------------------------------------------------------------------*/

/* Enum ----------------------------------------------------------------------*/

/* Struct --------------------------------------------------------------------*/
typedef enum {
    Motor_Status_OFFLINE = 0,
    Motor_Status_NORMAL,
    Motor_Status_LIGHT_STALL,
    Motor_Status_HEAVY_STALL,
} Motor_Status_t;

typedef struct
{
    const RC_ctrl_t *chassis_RC; //µ×ÅÌÊ¹ÓÃµÄÒ£¿ØÆ÷Ö¸Õë, the point to remote control
    const INS_t *chassis_INS_point;

    RC_ctrl_t Chassis_RC;

    cap_rx_data_t Super_Cap_Rx;
    cap_tx_data_t Super_Cap_Tx;

    vmc_leg_t left_leg;
    vmc_leg_t right_leg;

    float pitch;
    float d_pitch;

    PID_control PID_buffer; // ×óÍÈ±äÍÈ³¤pid

    PID_control PID_legL; // ×óÍÈ±äÍÈ³¤pid
    PID_control PID_legR; // ÓÒÍÈ±äÍÈ³¤pid
    PID_control PID_yaw_angle; // Æ«º½½Çpid
    PID_control PID_yaw_omega; // Æ«º½½Çpid
    PID_control PID_roll; // ºá¹ö½Çpid
    PID_control PID_tp; // ·ÀÅü²æpid
    Motor_DM_Normal Motor_Joint[4];
    Motor_C620 Motor_Wheel[2];
    Motor_DM_Normal Motor_Yaw;

    Motor_Status_t joint_motor_status[4];
    Motor_Status_t wheel_motor_status[2];

    float aaaa;
} chassis_move_t;

/* Function Declaration ------------------------------------------------------*/

/* Function ------------------------------------------------------------------*/

extern uint8_t cap[8];
extern chassis_move_t chassis_move;
#endif //CHASSISL_TASK_H