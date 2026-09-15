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
float P[40][6] =
{
	{-0.9114f, -17.336f, 4.5823f, 17.454f, 6.1675f, -7.2869f},
    {-1.4125f, -18.315f, 6.2039f, 19.623f, 3.9653f, -8.7543f},
    {-8.7714f, 18.754f, -12.309f, -18.845f, 4.3306f, 14.373f},
    {-2.3138f, 5.4872f, -3.9126f, -5.0018f, 1.2468f, 4.5794f},
    {-3.7196f, -74.88f, 17.691f, 50.724f, 19.991f, -25.904f},
    {-0.2118f, -6.3291f, 0.82563f, 0.87253f, 3.2788f, -1.7858f},
    {-2.1176f, 0.082412f, -37.85f, 4.5016f, 29.005f, 30.37f},
    {0.025268f, -1.0123f, -2.6438f, 1.8442f, -1.3215f, 1.3436f},
    {-46.761f, 26.77f, 68.96f, 57.526f, -69.581f, -49.709f},
    {-5.2271f, 2.9101f, 10.285f, 5.2689f, -9.4969f, -8.932f},
    {-0.9114f, 4.5823f, -17.336f, -7.2869f, 6.1675f, 17.454f},
    {-1.4125f, 6.2039f, -18.315f, -8.7543f, 3.9653f, 19.623f},
    {8.7714f, 12.309f, -18.754f, -14.373f, -4.3306f, 18.845f},
    {2.3138f, 3.9126f, -5.4872f, -4.5794f, -1.2468f, 5.0018f},
    {-2.1176f, -37.85f, 0.082412f, 30.37f, 29.005f, 4.5016f},
    {0.025268f, -2.6438f, -1.0123f, 1.3436f, -1.3215f, 1.8442f},
    {-3.7196f, 17.691f, -74.88f, -25.904f, 19.991f, 50.724f},
    {-0.2118f, 0.82563f, -6.3291f, -1.7858f, 3.2788f, 0.87253f},
    {-46.761f, 68.96f, 26.77f, -49.709f, -69.581f, 57.526f},
    {-5.2271f, 10.285f, 2.9101f, -8.932f, -9.4969f, 5.2689f},
    {19.974f, 38.725f, -75.991f, -123.17f, 83.706f, 58.191f},
    {24.558f, 35.834f, -94.78f, -134.3f, 115.03f, 71.896f},
    {-6.2838f, -76.229f, -19.68f, 134.15f, -72.052f, 31.923f},
    {-1.5012f, -24.271f, -4.5508f, 40.743f, -23.401f, 6.5058f},
    {61.078f, 135.33f, -52.778f, -256.22f, 204.99f, 38.185f},
    {6.8868f, 6.6418f, -6.4843f, -7.3449f, 3.8276f, 6.3082f},
    {12.623f, -45.549f, -76.566f, 44.223f, -164.84f, 35.273f},
    {0.56421f, 3.0834f, 0.53641f, -11.23f, -0.46667f, -8.8167f},
    {-17.042f, -862.17f, 48.374f, 854.23f, 270.18f, -45.067f},
    {1.9139f, -64.232f, -9.383f, 42.744f, 46.631f, 9.854f},
    {19.974f, -75.991f, 38.725f, 58.191f, 83.706f, -123.17f},
    {24.558f, -94.78f, 35.834f, 71.896f, 115.03f, -134.3f},
    {6.2838f, 19.68f, 76.229f, -31.923f, 72.052f, -134.15f},
    {1.5012f, 4.5508f, 24.271f, -6.5058f, 23.401f, -40.743f},
    {12.623f, -76.566f, -45.549f, 35.273f, -164.84f, 44.223f},
    {0.56421f, 0.53641f, 3.0834f, -8.8167f, -0.46667f, -11.23f},
    {61.078f, -52.778f, 135.33f, 38.185f, 204.99f, -256.22f},
    {6.8868f, -6.4843f, 6.6418f, 6.3082f, 3.8276f, -7.3449f},
    {-17.042f, 48.374f, -862.17f, -45.067f, 270.18f, 854.23f},
    {1.9139f, -9.383f, -64.232f, 9.854f, 46.631f, 42.744f}
};
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
 * @brief 由P矩阵通过计算得到4*10的K矩阵
 * @param
 * @return
 */
void Fitting_K_Calc(float (*fitting_k)[10], float (*p)[6], float L_l, float L_r)
{
    static int i = 0;
    static int j = 0;
    for(i=0;i<=3;i++)
    {
        for(j=0;j<=9;j++)
        {
            fitting_k[i][j] = p[i*10+j][0] + p[i*10+j][1]*L_l + p[i*10+j][2]*L_r + p[i*10+j][3]*(L_l*L_l) + p[i*10+j][4]*L_l*L_r + p[i*10+j][5]*(L_r*L_r);
        }
    }
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