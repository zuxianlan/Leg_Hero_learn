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
#define MIN_LEG_LENGTH 0.15f
#define MAX_LEG_LENGTH 0.28f
/* Enum ----------------------------------------------------------------------*/

/* Struct --------------------------------------------------------------------*/
typedef enum {
    Motor_Status_OFFLINE = 0,
    Motor_Status_NORMAL,
    Motor_Status_LIGHT_STALL,
    Motor_Status_HEAVY_STALL,
} Motor_Status_t;

typedef enum
{
    CHASSIS_ZERO_FORCE,                     // 底盘零力模式
    CHASSIS_NO_MOVE,                        // 底盘保持不动
    CHASSIS_INFANTRY_FOLLOW_GIMBAL_YAW,     // 底盘跟随云台偏航
} chassis_mode_e;

typedef enum
{
    NORMAL, // 底盘正常状态
    OVER_TURN, // 底盘翻倒
    OVER_TURNING, // 底盘正在翻身
    READY_TO_JUMP, // 准备跳跃
    GATHER_STRENGTH, // 跳跃蓄力
    TAKE_OFF_TO_JUMP, // 起跳伸腿
    LANDING_CUSHIONING, // 落地缓冲
    ABOVE_GROUND, // 底盘离地
} chassis_fsm_mode_e;

typedef struct
{
    const RC_ctrl_t *chassis_RC; //底盘使用的遥控器指针, the point to remote control
    const INS_t *chassis_INS_point;

    RC_ctrl_t Chassis_RC;

    cap_rx_data_t Super_Cap_Rx;
    cap_tx_data_t Super_Cap_Tx;

    vmc_leg_t left_leg;
    vmc_leg_t right_leg;

    float pitch;
    float d_pitch;

    PID_control PID_buffer; // 左腿变腿长pid

    PID_control PID_legL; // 左腿变腿长pid
    PID_control PID_legR; // 右腿变腿长pid
    PID_control PID_yaw_angle; // 偏航角pid
    PID_control PID_yaw_omega; // 偏航角pid
    PID_control PID_roll; // 横滚角pid
    PID_control PID_tp; // 防劈叉pid
    Motor_DM_Normal Motor_Joint[4];
    Motor_C620 Motor_Wheel[2];
    Motor_DM_Normal Motor_Yaw;

    Motor_Status_t joint_motor_status[4];
    Motor_Status_t wheel_motor_status[2];

    float Target_Leg_l;
    float Target_Leg_r;
    float Target_Roll;
    float Target_Theta; //目标误差

    float theta_err;// 两腿夹角误差
    float aaaa;
} chassis_move_t;

/* Function Declaration ------------------------------------------------------*/

/* Function ------------------------------------------------------------------*/

extern uint8_t cap[8];
extern chassis_move_t chassis_move;
#endif //CHASSISL_TASK_H