/**
  ******************************************************************************
  * @file           : ChassisL_Task.c
  * @author         : gagami
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/6
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "ChassisL_Task.h"
#include "fdcan.h"
#include "CAN_receive.h"
#include "Host_Comm_Task.h"
#include "VMC&LQR_Calc.h"
#include "motor_dji.h"
#include "motor_dm.h"
#include "Can_Comm_Task.h"

/* Define --------------------------------------------------------------------*/

/* Enum ----------------------------------------------------------------------*/

/* Variable && Struct --------------------------------------------------------*/
chassis_move_t chassis_move;
chassis_mode_e chassis_mode;
vmc_leg_t left_vmc_leg;
vmc_leg_t right_vmc_leg;
const uint32_t chassis_time = 1;
extern float mpc_out;

/* Function Declaration ------------------------------------------------------*/
static void chassis_init(chassis_move_t * chassis_move_init);
static void chassis_set_control(chassis_move_t *chassis);
void Chassis_Feedback_Update(chassis_move_t *chassis, vmc_leg_t *vmcl, vmc_leg_t *vmcr);
static void chassis_control_loop(chassis_move_t *chassis, vmc_leg_t *vmcl, vmc_leg_t *vmcr);
static void chassis_lqr_calc_to_motor(chassis_move_t *chassis, vmc_leg_t *vmcl, vmc_leg_t *vmcr);
static void chassis_output_to_motor(chassis_move_t * chassis);
void Chassis_Motor_Status_PeriodElapsedCallback(chassis_move_t *chassis);
void chassis_motor_keep_alive(chassis_move_t *chassis);
static float Max_Output(float num,float max);

uint8_t cap[8] = {0};
float Fitting_K[4][10] = {}; // 10 维 LQR 增益矩阵（4输入×10状态，由 P[40][6] 系数表按左右腿长实时重建）
int a=0;
/* Function ------------------------------------------------------------------*/
/**
 * @brief 底盘初始化
 *
 * @param
 * @return
 */
void ChassisL_Task(void)
{
    while (INS.ins_flag == 0)
    {
        osDelay(1);
    }
    // osDelay(5000);
    chassis_init(&chassis_move);

    while (1)
    {
        //目标设置
        chassis_set_control(&chassis_move);
        //底盘数据更新
        Chassis_Feedback_Update(&chassis_move, &left_vmc_leg, &right_vmc_leg);
        //PID计算
        chassis_control_loop(&chassis_move, &left_vmc_leg, &right_vmc_leg);

        chassis_lqr_calc_to_motor(&chassis_move, &left_vmc_leg, &right_vmc_leg);
        chassis_output_to_motor(&chassis_move);
        Chassis_Motor_Status_PeriodElapsedCallback(&chassis_move);
        CAP_AddTxPacket(&chassis_move.Super_Cap_Tx, cap);
        //轮电机，超电数据发送
        TIM_CAN_PeriodElapsedCallback();

        osDelay(1);
    }
}

/**
 * @brief 左腿初始化
 *
 * @param
 * @return
 */
static void chassis_init(chassis_move_t *chassis_move_init)
{
    //遥控器数据指针获取
    chassis_move_init->chassis_RC = get_remote_control_point();

    //陀螺仪指针
    chassis_move_init->chassis_INS_point = get_INS_point();

    /* 前
     * 0 2
     * 1 3
     */
    Motor_DM_Normal_Init(&chassis_move_init->Motor_Joint[3], &hfdcan1, 0x11, 0x01, Motor_DM_Control_Method_NORMAL_MIT,
                         1 * PI, 45.0f, 54.0f, 3.0f);
    Motor_DM_Normal_Init(&chassis_move_init->Motor_Joint[2], &hfdcan1, 0x22, 0x02, Motor_DM_Control_Method_NORMAL_MIT,
                         1 * PI, 45.0f, 54.0f, 3.0f);
    Motor_DM_Normal_Init(&chassis_move_init->Motor_Joint[1], &hfdcan1, 0x33, 0x03, Motor_DM_Control_Method_NORMAL_MIT,
                         1 * PI, 45.0f, 54.0f, 3.0f);
    Motor_DM_Normal_Init(&chassis_move_init->Motor_Joint[0], &hfdcan1, 0x44, 0x04, Motor_DM_Control_Method_NORMAL_MIT,
                         1 * PI, 45.0f, 54.0f, 3.0f);
    Motor_DM_Normal_Init(&chassis_move_init->Motor_Yaw, &hfdcan3, 0x55, 0x05, Motor_DM_Control_Method_NORMAL_MIT, 1 * PI,30.0f, 10.0f, 3.0f);

    Motor_C620_Init(&chassis_move_init->Motor_Wheel[0], &hfdcan2, CAN_Motor_ID_0x202, Motor_DJI_Control_Method_CURRENT,
                    Motor_DJI_Power_Limit_Status_DISABLE, 20.0f);
    chassis_move_init->Motor_Wheel[0].Gearbox_Rate = 15.17f;

    Motor_C620_Init(&chassis_move_init->Motor_Wheel[1], &hfdcan2, CAN_Motor_ID_0x201, Motor_DJI_Control_Method_CURRENT,
                    Motor_DJI_Power_Limit_Status_DISABLE, 20.0f);
    chassis_move_init->Motor_Wheel[1].Gearbox_Rate = 15.17f;

    // 底盘 PID 初始化
    PID_Init(&chassis_move_init->PID_legL, 50.0f, 10.0f, 20.0f, 0.0f, 10.0f, 150.0f, 0.001f, 0.0f, 0.0f, 0.0f, 0.0f, PID_D_First_ENABLE);
    PID_Init(&chassis_move_init->PID_legR, 50.0f, 10.0f, 20.0f, 0.0f, 10.0f, 150.0f, 0.001f, 0.0f, 0.0f, 0.0f, 0.0f, PID_D_First_ENABLE);
    PID_Init(&chassis_move_init->PID_roll, 5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 90.0f, 0.001f, 0.0f, 0.0f, 0.0f, 0.0f, PID_D_First_ENABLE);
    PID_Init(&chassis_move_init->PID_tp, 70.0f, 0.0f, 0.0f, 0.0f, 0.0f, 5.0f, 0.01f, 0.0f, 0.0f, 0.0f,0.0f, PID_D_First_ENABLE);

    PID_Init(&chassis_move_init->PID_buffer, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 10.0f, 0.1f, 0.0f, 0.0f, 0.0f,0.0f, PID_D_First_DISABLE);

    Motor_DM_Normal_CAN_Send_Enable(&chassis_move_init->Motor_Joint[0]);
    Motor_DM_Normal_CAN_Send_Enable(&chassis_move_init->Motor_Joint[1]);
    Motor_DM_Normal_CAN_Send_Enable(&chassis_move_init->Motor_Joint[2]);
    Motor_DM_Normal_CAN_Send_Enable(&chassis_move_init->Motor_Joint[3]);


}

/**
 * @brief 设置底盘目标值
 * @param
 * @return
 */
static void chassis_set_control(chassis_move_t *chassis)
{
    if (chassis == NULL)  {return;}
    // 设置 theta 误差目标
    chassis->Target_Theta = 0.0f;

}

//反馈更新
void Chassis_Feedback_Update(chassis_move_t *chassis, vmc_leg_t *vmcl, vmc_leg_t *vmcr)
{
    chassis->left_leg.phi1 = PI + chassis->Motor_Joint[1].Rx_Data.Now_Angle;
    chassis->left_leg.phi4 = chassis->Motor_Joint[0].Rx_Data.Now_Angle;
    chassis->left_leg.d_phi1 = chassis->Motor_Joint[1].Rx_Data.Now_Omega;
    chassis->left_leg.d_phi4 = chassis->Motor_Joint[0].Rx_Data.Now_Omega;

    chassis->right_leg.phi1 = PI - chassis->Motor_Joint[2].Rx_Data.Now_Angle;
    chassis->right_leg.phi4 = -chassis->Motor_Joint[3].Rx_Data.Now_Angle;
    chassis->right_leg.d_phi1 = -chassis->Motor_Joint[2].Rx_Data.Now_Omega;
    chassis->right_leg.d_phi4 = -chassis->Motor_Joint[3].Rx_Data.Now_Omega;

    chassis->pitch = -chassis->chassis_INS_point->Pitch;
    chassis->d_pitch = -chassis->chassis_INS_point->Gyro[0];
    chassis->theta_err = vmcl->theta - vmcr->theta;

    VMC_Calc_1(&chassis->left_leg, (float)chassis_time/1000.0f);
    VMC_Calc_1(&chassis->right_leg, (float)chassis_time/1000.0f);

    chassis->err[0] = chassis->X_filter - chassis->Target_X;
    chassis->err[1] = chassis->Velocity_filter - chassis->Target_Velocity;
    chassis->err[2] = 0;
    chassis->err[3] = chassis->chassis_INS_point->Gyro[2] - chassis->Target_Omega;
    chassis->err[4] = Max_Output((vmcl->theta - chassis->Target_Theta), 0.5f);
    chassis->err[5] = vmcl->d_theta - 0;
    chassis->err[6] = Max_Output((vmcr->theta - chassis->Target_Theta), 0.5f);
    chassis->err[7] = vmcr->d_theta - 0;
    chassis->err[8] = Max_Output((chassis->pitch - 0), 1.0f);
    chassis->err[9] = chassis->d_pitch - 0;

}


/**
 * @brief 根据底盘模式执行 PID 控制
 * @param
 * @return
 */
static void chassis_control_loop(chassis_move_t *chassis, vmc_leg_t *vmcl, vmc_leg_t *vmcr)
{

    // 设置左腿长度目标
    chassis->PID_legL.Target += chassis->Target_Leg_l;
    Math_Constrain(&chassis->PID_legL.Target, MIN_LEG_LENGTH, MAX_LEG_LENGTH);
    chassis->PID_legL.Now = vmcl->L0;
    PID_TIM_Adjust_PeriodElapsedCallback(&chassis->PID_legL);

    // 设置右腿长度目标
    chassis->PID_legR.Target += chassis->Target_Leg_r;
    Math_Constrain(&chassis->PID_legR.Target, MIN_LEG_LENGTH, MAX_LEG_LENGTH);
    chassis->PID_legR.Now = vmcr->L0;
    PID_TIM_Adjust_PeriodElapsedCallback(&chassis->PID_legR);

    // 设置横滚目标
    chassis->PID_roll.Target = chassis->Target_Roll;
    Math_Constrain(&chassis->PID_roll.Target, -PI/10.0f, PI/10.0f);
    chassis->PID_roll.Now = chassis->chassis_INS_point->Roll;
    //PID_TIM_Adjust_PeriodElapsedCallback(&chassis->PID_roll);

    // 设置 theta 误差目标
    chassis->PID_tp.Target = chassis->Target_Theta;
    Math_Constrain(&chassis->PID_tp.Target, -PI/6.0f, PI/6.0f);
    chassis->PID_tp.Now = chassis->theta_err;
    //PID_TIM_Adjust_PeriodElapsedCallback(&chassis->PID_tp);

}

/**
 * @brief 计算T矩阵给电机
 * @param
 * @return
 */
static void chassis_lqr_calc_to_motor(chassis_move_t *chassis, vmc_leg_t *vmcl, vmc_leg_t *vmcr)
{
    for (int i = 0; i < 4; i++)
    {
        chassis->T[i] = chassis->err[0] * Fitting_K[i][0]
        + Max_Output(chassis->err[1] * Fitting_K[i][1], 16)
        - Max_Output(chassis->err[2] * Fitting_K[i][2], 15.0f)
        + chassis->err[3] * Fitting_K[i][3]
        + chassis->err[4] * Fitting_K[i][4]
        + chassis->err[5] * Fitting_K[i][5] * 0.94f
        + chassis->err[6] * Fitting_K[i][6]
        + chassis->err[7] * Fitting_K[i][7] * 0.94f
        + chassis->err[8] * Fitting_K[i][8]
        + chassis->err[9] * Fitting_K[i][9];
    }
    chassis->T_wl = chassis->T[0];
    chassis->T_wr = chassis->T[1];
    vmcl->Tp = chassis->T[2];
    vmcr->Tp = chassis->T[3];
}
/**
 * @brief 输出到电机
 * @param
 * @return
 */
static void chassis_output_to_motor(chassis_move_t *chassis)
{
    static int mod = 0;
    mod++;
    if (mod == 100)
    {
        chassis->PID_buffer.Target = 50.0f;
        chassis->PID_buffer.Now = power_heat_data.buffer_energy;
        PID_TIM_Adjust_PeriodElapsedCallback(&chassis->PID_buffer);
        mod = 0;
    }
    chassis->Super_Cap_Tx.cap_set_power = robot_state.chassis_power_limit * 100 - (int)chassis->PID_buffer.Out * 100;

    if (robot_state.chassis_output == 0 )
    {
        chassis->Super_Cap_Tx.cap_flag = 0;
    }
    else
    {
        chassis->Super_Cap_Tx.cap_flag = 1;
    }

    chassis->Motor_Joint[3].Control_Torque = -Host_communication.Rx_data.torque[3] * 1.0;
    chassis->Motor_Joint[2].Control_Torque = -Host_communication.Rx_data.torque[2] * 1.0;
    chassis->Motor_Joint[1].Control_Torque = -Host_communication.Rx_data.torque[1] * 1.0;
    chassis->Motor_Joint[0].Control_Torque = -Host_communication.Rx_data.torque[0] * 1.0;

    chassis->Motor_Wheel[0].Target_Current = 4.0f * Host_communication.Rx_data.wheel_torque[0];
    chassis->Motor_Wheel[1].Target_Current = 4.0f * Host_communication.Rx_data.wheel_torque[1];

    if (Can_Comm.Can_Control_Data.Rx_Data.robot_control_status==No_Control)
    {
        chassis->Motor_Joint[3].Control_Torque = 0.0f;
        chassis->Motor_Joint[2].Control_Torque = 0.0f;
        chassis->Motor_Joint[1].Control_Torque = 0.0f;
        chassis->Motor_Joint[0].Control_Torque = 0.0f;

        chassis->Motor_Wheel[0].Target_Current = 0.0f;
        chassis->Motor_Wheel[1].Target_Current = 0.0f;
    }

    Motor_DM_Normal_TIM_Send_PeriodElapsedCallback(&chassis->Motor_Joint[0]);
    Motor_DM_Normal_TIM_Send_PeriodElapsedCallback(&chassis->Motor_Joint[1]);
    Motor_DM_Normal_TIM_Send_PeriodElapsedCallback(&chassis->Motor_Joint[2]);
    Motor_DM_Normal_TIM_Send_PeriodElapsedCallback(&chassis->Motor_Joint[3]);
    Motor_C620_TIM_Calculate_PeriodElapsedCallback(&chassis->Motor_Wheel[0]);
    Motor_C620_TIM_Calculate_PeriodElapsedCallback(&chassis->Motor_Wheel[1]);
}


void chassis_motor_keep_alive(chassis_move_t *chassis)
{
    static uint32_t Counter_KeepAlive = 0;

    if (Counter_KeepAlive++ > 100)
    {
        Counter_KeepAlive=0;
        //髋关节电机
        Motor_DM_Normal_TIM_Alive_PeriodElapsedCallback(&chassis->Motor_Joint[0]);
        Motor_DM_Normal_TIM_Alive_PeriodElapsedCallback(&chassis->Motor_Joint[1]);
        Motor_DM_Normal_TIM_Alive_PeriodElapsedCallback(&chassis->Motor_Joint[2]);
        Motor_DM_Normal_TIM_Alive_PeriodElapsedCallback(&chassis->Motor_Joint[3]);

        //轮电机
        Motor_C620_TIM_Alive_PeriodElapsedCallback(&chassis->Motor_Wheel[0]);
        Motor_C620_TIM_Alive_PeriodElapsedCallback(&chassis->Motor_Wheel[1]);
    }
}

void chassis_joint_motor_status_influence_output(chassis_move_t *chassis)
{
    if (chassis == NULL)
    {
        return;
    }

    for (uint8_t i = 0; i < 4; i++)
    {
        if (chassis->Motor_Joint[i].Motor_DM_Status == Motor_DM_Status_DISABLE)
        {
            chassis->joint_motor_status[i] = Motor_Status_OFFLINE;
        }
        else if (chassis->Motor_Joint[i].Motor_DM_Status == Motor_DM_Status_ENABLE)
        {
            chassis->joint_motor_status[i] = Motor_Status_NORMAL;
        }
        else
        {
            chassis->joint_motor_status[i] = Motor_Status_OFFLINE;
        }
    }

}

void chassis_wheel_motor_status_influence_output(chassis_move_t *chassis)
{
    if (chassis == NULL)
    {
        return;
    }

    for (uint8_t i = 0; i < 2; i++)
    {
        if (chassis->Motor_Wheel[i].CAN_Motor_Status == CAN_Motor_Status_DISABLE)
        {
            chassis->wheel_motor_status[i] = Motor_Status_OFFLINE;
        }
        else if (chassis->Motor_Wheel[i].CAN_Motor_Status == CAN_Motor_Status_ENABLE)
        {
            chassis->wheel_motor_status[i] = Motor_Status_NORMAL;
        }
        else
        {
            chassis->wheel_motor_status[i] = Motor_Status_OFFLINE;
        }
    }

}

void Chassis_Motor_Status_PeriodElapsedCallback(chassis_move_t *chassis)
{
    //电机在线状态检测
    chassis_motor_keep_alive(&chassis_move);

    //电机状态裁决
    chassis_joint_motor_status_influence_output(chassis);
    chassis_wheel_motor_status_influence_output(chassis);
}

static float Max_Output(float num,float max)
{
    if(num>=max) return max;
    else if(num<=-max) return -max;
    else return num;
}