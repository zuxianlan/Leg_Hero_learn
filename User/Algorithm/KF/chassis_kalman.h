/**
******************************************************************************
  * @file           : Kalman_filter.h
  * @brief          : 底盘前进速度二维卡尔曼滤波器（机体系里程计 + 加速度计）
  ******************************************************************************
  */
#ifndef CTRLBOARD_H7_IMU_KALMAN_FILTER_H
#define CTRLBOARD_H7_IMU_KALMAN_FILTER_H

typedef struct
{
    /* ---------- 滤波器模型参数 ---------- */
    float x_hat[2];     // 后验状态估计
    float x_hat_p[2];   // 先验状态估计
    float K[2][2];      // 卡尔曼增益矩阵
    float P[2][2];      // 后验误差协方差
    float P_p[2][2];    // 先验误差协方差
    float F[2][2];      // 状态转移矩阵
    float Q[2][2];      // 过程噪声协方差
    float H[2][2];      // 观测矩阵
    float R[2][2];      // 测量噪声协方差
    float z[2];         // 测量向量
    float StateMinVariance[2];           // 协方差对角元下界，防止过度收敛
} chassis_kalman_t;

extern void chassis_kalman_init(chassis_kalman_t *kf);
extern void chassis_kalman_update(chassis_kalman_t *kf);
extern chassis_kalman_t chassis_kalman;
#define CHASSIS_KF_DT  0.001f       // 控制周期 s，与 osDelay(1) 对应
#define MIN_VARIANCE 0.03f
#endif //CTRLBOARD_H7_IMU_KALMAN_FILTER_H