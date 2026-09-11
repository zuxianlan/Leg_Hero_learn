/**
  ******************************************************************************
  * @file           : VMC&LQR_Calc.h
  * @author         : gagami
  * @brief          : None
  * @attention      : None
  * @date           : 2025/8/7
  ******************************************************************************
  */
#ifndef VMC_LQR_CALC_H
#define VMC_LQR_CALC_H

/* Includes ------------------------------------------------------------------*/
#include "struct_typedef.h"
#include "INS_Task.h"
/* Define --------------------------------------------------------------------*/

/* Enum ----------------------------------------------------------------------*/

/* Variable && Struct --------------------------------------------------------*/

typedef struct
{
    const INS_t *INS_point;

    float L0, phi0; //C点的极坐标

    float d_phi0; //现在C点角度phi0的变换率
    float last_phi0; //上一次C点角度，用于计算角度phi0的变换率d_phi0

    float phi2, phi3;
    float phi1, phi4;
    float d_phi1, d_phi4;

    float torque_set[2]; //VMC解算出电机给定力矩

    float F0;
    float Tp;
    float F02;

    float theta;
    float d_theta; //theta的一阶导数
    float last_d_theta;
    float dd_theta; //theta的二阶导数

    float d_L0; //L0的一阶导数
    float dd_L0; //L0的二阶导数
    float last_L0;
    float last_d_L0;

    float FN; //支持力
} vmc_leg_t;

/* Function Declaration ------------------------------------------------------*/
void VMC_Calc_1(vmc_leg_t *vmc, float dt);

void VMC_Calc_2(vmc_leg_t *vmc);

uint8_t ground_detection(vmc_leg_t *vmc, const INS_t *ins);

/* Function ------------------------------------------------------------------*/


#endif //VMC_LQR_CALC_H