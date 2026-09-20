#include "chassis_kalman.h"
#include "ChassisL_Task.h"

/**
  * @brief  底盘速度卡尔曼滤波器初始化
  * @param  kf 滤波器实例
  */
void chassis_kalman_init(chassis_Kalman_t *kf)
{
    kf->x_hat[0]   = 0.0f;
    kf->x_hat[1]   = 0.0f;
    kf->x_hat_p[0] = 0.0f;
    kf->x_hat_p[1] = 0.0f;

    kf->StateMinVariance[0] = MIN_VARIANCE;
    kf->StateMinVariance[1] = MIN_VARIANCE;

    /* P = I */
    kf->P[0][0] = 1.0f;  kf->P[0][1] = 0.0f;
    kf->P[1][0] = 0.0f;  kf->P[1][1] = 1.0f;
    kf->P_p[0][0] = 1.0f;  kf->P_p[0][1] = 0.0f;
    kf->P_p[1][0] = 0.0f;  kf->P_p[1][1] = 1.0f;

    /* F = [1 dt; 0 1]，匀加速模型 ZOH 离散化 */
    kf->F[0][0] = 1.0f;  kf->F[0][1] = CHASSIS_KF_DT;
    kf->F[1][0] = 0.0f;  kf->F[1][1] = 1.0f;

    /* H = I，两路测量各自直观一个状态 */
    kf->H[0][0] = 1.0f;  kf->H[0][1] = 0.0f;
    kf->H[1][0] = 0.0f;  kf->H[1][1] = 1.0f;

    /* Q = I */
    kf->Q[0][0] = 1.0f;  kf->Q[0][1] = 0.0f;
    kf->Q[1][0] = 0.0f;  kf->Q[1][1] = 1.0f;

    /* R = 100*I，非常不信任测量 */
    kf->R[0][0] = 100.0f;  kf->R[0][1] = 0.0f;
    kf->R[1][0] = 0.0f;    kf->R[1][1] = 100.0f;

    kf->z[0] = 0.0f;
    kf->z[1] = 0.0f;

    kf->K[0][0] = 0.0f;  kf->K[0][1] = 0.0f;
    kf->K[1][0] = 0.0f;  kf->K[1][1] = 0.0f;

}

void chassis_kalman_update(chassis_Kalman_t *kf)
{
    kf->z[0] = chassis_move.Average_Speed;
    kf->z[1] = chassis_move.chassis_INS_point->MotionAccel_n[1];
    chassis_move.Velocity_filter = kf->x_hat[0];
}