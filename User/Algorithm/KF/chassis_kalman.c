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

/**
  * @brief  底盘速度卡尔曼滤波器一拍更新
  * @param  kf 滤波器实例
  */
void chassis_kalman_update(chassis_Kalman_t *kf)
{
    /* ---------- 局部临时量：对应卡尔曼滤波公式中的中间矩阵 ---------- */
    float FPT[2][2];        /* F·P            （步骤2 临时）→ 复用为 H·P_p?（步骤3 临时） */
    float S[2][2];          /* S = H·P?·H? + R    新息协方差 */
    float Sinv[2][2];       /* S??                新息协方差的逆 */
    float PHt[2][2];        /* P?·H?              增益分子 */
    float innovation[2];    /* ν = z ? H·x??       新息（测量残差） */
    float IKH[2][2];        /* I ? K·H            协方差更新算子 */
    float det;              /* |S| = S??S?? ? S??S??  用于 2×2 求逆 */

    /* ---------- 测量向量 z ---------- */
    kf->z[0] = chassis_move.Average_Speed;
    kf->z[1] = chassis_move.chassis_INS_point->MotionAccel_n[1];;   /* MotionAccel_n[1] 在下位机侧恒为零（AccelLPF 未初始化），先传 0 */

    /* ================= 步骤1  先验状态估计  x?? = F·x? ================= */
    kf->x_hat_p[0] = kf->F[0][0] * kf->x_hat[0] + kf->F[0][1] * kf->x_hat[1];
    kf->x_hat_p[1] = kf->F[1][0] * kf->x_hat[0] + kf->F[1][1] * kf->x_hat[1];

    /* ================= 步骤2  先验协方差  P? = F·P·F? + Q ================= */
    /* 2a: FPT = F·P */
    FPT[0][0] = kf->F[0][0] * kf->P[0][0] + kf->F[0][1] * kf->P[1][0];
    FPT[0][1] = kf->F[0][0] * kf->P[0][1] + kf->F[0][1] * kf->P[1][1];
    FPT[1][0] = kf->F[1][0] * kf->P[0][0] + kf->F[1][1] * kf->P[1][0];
    FPT[1][1] = kf->F[1][0] * kf->P[0][1] + kf->F[1][1] * kf->P[1][1];

    /* 2b: P_p = FPT·F? + Q    （(F?)[k][j] = F[j][k]） */
    kf->P_p[0][0] = FPT[0][0] * kf->F[0][0] + FPT[0][1] * kf->F[0][1] + kf->Q[0][0];
    kf->P_p[0][1] = FPT[0][0] * kf->F[1][0] + FPT[0][1] * kf->F[1][1] + kf->Q[0][1];
    kf->P_p[1][0] = FPT[1][0] * kf->F[0][0] + FPT[1][1] * kf->F[0][1] + kf->Q[1][0];
    kf->P_p[1][1] = FPT[1][0] * kf->F[1][0] + FPT[1][1] * kf->F[1][1] + kf->Q[1][1];

    /* ================= 步骤3  新息协方差  S = H·P?·H? + R ================= */
    /* 3a: FPT 复用为 H·P? */
    FPT[0][0] = kf->H[0][0] * kf->P_p[0][0] + kf->H[0][1] * kf->P_p[1][0];
    FPT[0][1] = kf->H[0][0] * kf->P_p[0][1] + kf->H[0][1] * kf->P_p[1][1];
    FPT[1][0] = kf->H[1][0] * kf->P_p[0][0] + kf->H[1][1] * kf->P_p[1][0];
    FPT[1][1] = kf->H[1][0] * kf->P_p[0][1] + kf->H[1][1] * kf->P_p[1][1];

    /* 3b: S = (H·P?)·H? + R */
    S[0][0] = FPT[0][0] * kf->H[0][0] + FPT[0][1] * kf->H[0][1] + kf->R[0][0];
    S[0][1] = FPT[0][0] * kf->H[1][0] + FPT[0][1] * kf->H[1][1] + kf->R[0][1];
    S[1][0] = FPT[1][0] * kf->H[0][0] + FPT[1][1] * kf->H[0][1] + kf->R[1][0];
    S[1][1] = FPT[1][0] * kf->H[1][0] + FPT[1][1] * kf->H[1][1] + kf->R[1][1];

    /* ================= 步骤4  卡尔曼增益  K = P?·H?·S?? ================= */
    /* 4a: 2×2 解析求逆  Sinv = S?? = adj(S)/|S| */
    det = S[0][0] * S[1][1] - S[0][1] * S[1][0];
    if (det < 0.0f)
    {
        det = -det;
    }
    if (det < 1.0e-12f)
    {
        det = 1.0e-12f;
    }
    Sinv[0][0] =  S[1][1] / det;
    Sinv[0][1] = -S[0][1] / det;
    Sinv[1][0] = -S[1][0] / det;
    Sinv[1][1] =  S[0][0] / det;

    /* 4b: PHt = P?·H? */
    PHt[0][0] = kf->P_p[0][0] * kf->H[0][0] + kf->P_p[0][1] * kf->H[0][1];
    PHt[0][1] = kf->P_p[0][0] * kf->H[1][0] + kf->P_p[0][1] * kf->H[1][1];
    PHt[1][0] = kf->P_p[1][0] * kf->H[0][0] + kf->P_p[1][1] * kf->H[0][1];
    PHt[1][1] = kf->P_p[1][0] * kf->H[1][0] + kf->P_p[1][1] * kf->H[1][1];

    /* 4c: K = PHt·Sinv   （右乘逆，顺序不能颠倒） */
    kf->K[0][0] = PHt[0][0] * Sinv[0][0] + PHt[0][1] * Sinv[1][0];
    kf->K[0][1] = PHt[0][0] * Sinv[0][1] + PHt[0][1] * Sinv[1][1];
    kf->K[1][0] = PHt[1][0] * Sinv[0][0] + PHt[1][1] * Sinv[1][0];
    kf->K[1][1] = PHt[1][0] * Sinv[0][1] + PHt[1][1] * Sinv[1][1];

    /* ============ 步骤5  后验状态估计  x? = x?? + K·(z ? H·x??) ============ */
    /* 5a: ν = z ? H·x?? */
    innovation[0] = kf->z[0] - (kf->H[0][0] * kf->x_hat_p[0] + kf->H[0][1] * kf->x_hat_p[1]);
    innovation[1] = kf->z[1] - (kf->H[1][0] * kf->x_hat_p[0] + kf->H[1][1] * kf->x_hat_p[1]);

    /* 5b: x? = x?? + K·ν */
    kf->x_hat[0] = kf->x_hat_p[0] + kf->K[0][0] * innovation[0] + kf->K[0][1] * innovation[1];
    kf->x_hat[1] = kf->x_hat_p[1] + kf->K[1][0] * innovation[0] + kf->K[1][1] * innovation[1];

    /* ============ 步骤6  后验协方差  P = (I ? K·H)·P?  + 对角下界 ============ */
    /* 6a: IKH = I ? K·H    （δij 用 (i==j)?1:0） */
    IKH[0][0] = 1.0f - (kf->K[0][0] * kf->H[0][0] + kf->K[0][1] * kf->H[1][0]);
    IKH[0][1] = 0.0f - (kf->K[0][0] * kf->H[0][1] + kf->K[0][1] * kf->H[1][1]);
    IKH[1][0] = 0.0f - (kf->K[1][0] * kf->H[0][0] + kf->K[1][1] * kf->H[1][0]);
    IKH[1][1] = 1.0f - (kf->K[1][0] * kf->H[0][1] + kf->K[1][1] * kf->H[1][1]);

    /* 6b: P = IKH·P? */
    kf->P[0][0] = IKH[0][0] * kf->P_p[0][0] + IKH[0][1] * kf->P_p[1][0];
    kf->P[0][1] = IKH[0][0] * kf->P_p[0][1] + IKH[0][1] * kf->P_p[1][1];
    kf->P[1][0] = IKH[1][0] * kf->P_p[0][0] + IKH[1][1] * kf->P_p[1][0];
    kf->P[1][1] = IKH[1][0] * kf->P_p[0][1] + IKH[1][1] * kf->P_p[1][1];

    /* 6c: 对角元下界钳位，防止过度收敛（对应香橙派 KalmanFilter.cpp:221-227） */
    if (kf->P[0][0] < kf->StateMinVariance[0])
    {
        kf->P[0][0] = kf->StateMinVariance[0];
    }
    if (kf->P[1][1] < kf->StateMinVariance[1])
    {
        kf->P[1][1] = kf->StateMinVariance[1];
    }

    /* ================= 步骤7  输出（必须在最后） ================= */
    chassis_move.Velocity_filter = kf->x_hat[0];
}