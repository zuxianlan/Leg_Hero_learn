/**
  ******************************************************************************
  * @file           : VMC&LQR_Calc.c
  * @author         : gagami
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/7
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "VMC&LQR_Calc.h"
#include "leg_position.h"
#include "leg_speed.h"
#include "leg_convert.h"


/* Define --------------------------------------------------------------------*/

/* Enum ----------------------------------------------------------------------*/

/* Variable && Struct --------------------------------------------------------*/

/* Function Declaration ------------------------------------------------------*/

/* Function ------------------------------------------------------------------*/

/**
 * @brief 计算theta和dtheta给LQR控制器，同时得到腿长l0
 *
 * @param
 * @return
 */
void VMC_Calc_1(vmc_leg_t *vmc, float dt)
{
    static float pitch, d_pitch, alpha;
    pitch = vmc->INS_point->Pitch;
    d_pitch = vmc->INS_point->Gyro[0];

    // 计算得到腿长l0和phi0
    leg_position(vmc->phi1, vmc->phi4, &vmc->L0, &vmc->phi0);
    leg_speed(vmc->d_phi1, vmc->d_phi4, vmc->phi1, vmc->phi4, &vmc->d_L0, &vmc->d_phi0);

    // 计算夹角alpha，之后用来计算倾角theta
    alpha = vmc->phi0 - PI / 2.0f;

    //vmc->d_phi0 = (vmc->phi0 - vmc->last_phi0) / dt;

    //得到状态变量1 2
    vmc->theta = alpha - pitch;
    vmc->d_theta = (vmc->d_phi0 - d_pitch);

    // 更新数据
    vmc->last_phi0 = vmc->phi0;

    // 计算相关变量
    // vmc->d_L0 = (vmc->L0 - vmc->last_L0) / dt;
    vmc->dd_L0 = (vmc->d_L0 - vmc->last_d_L0) / dt;

    vmc->last_d_L0 = vmc->d_L0;
    vmc->last_L0 = vmc->L0;

    vmc->dd_theta = (vmc->d_theta - vmc->last_d_theta) / dt;
    vmc->last_d_theta = vmc->d_theta;
}

/**
 * @brief 通过LQR以及补偿得到的Tp计算髋关节电机输出力矩
 *
 * @param
 * @return
 */
void VMC_Calc_2(vmc_leg_t *vmc)
{
    leg_convert(vmc->F0, vmc->Tp, vmc->phi1, vmc->phi4, vmc->torque_set);
}

/**
 * @brief 离地检测
 *
 * @param
 * @return
 */
uint8_t ground_detection(vmc_leg_t *vmc, const INS_t *ins)
{
    // 静止平衡等式
    vmc->FN = -vmc->F0 * arm_cos_f32(vmc->theta) - vmc->Tp * arm_sin_f32(vmc->theta) / vmc->L0 - 0.2f;
    // 腿部机构的力+轮子重力, 这里忽略了轮子质量*驱动轮竖直方向运动加速度
    // 牛顿第二定律
    // vmc->FN = vmc->F0*arm_cos_f32(vmc->theta) + vmc->Tp*arm_sin_f32(vmc->theta)/vmc->L0 + 0.6f*(ins->MotionAccel_n[2] - vmc->dd_L0*arm_cos_f32(vmc->theta) + 2.0f*vmc->d_L0*vmc->d_theta*arm_sin_f32(vmc->theta) + vmc->L0*vmc->dd_theta*arm_sin_f32(vmc->theta) + vmc->L0*vmc->d_theta*vmc->d_theta*arm_cos_f32(vmc->theta));

    if (vmc->FN < 5.5f)
    {
        //离地了

        return 1;
    }
    else
    {
        return 0;
    }
}