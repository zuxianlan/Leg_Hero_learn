/**
 * @file alg_filter.h
 * @author yssickjgd (1345578933@qq.com)
 * @brief 滤波器
 * @version 1.1
 * @date 2023-08-29 0.1 23赛季定稿
 * @date 2023-09-25 1.1 可自定义滤波器阶数
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef ALG_FILTER_H
#define ALG_FILTER_H

/* Includes ------------------------------------------------------------------*/

#include "Mathh.h"
#include "main.h"
#include "arm_math.h"

/* Exported macros -----------------------------------------------------------*/

// 采样频率
#define FOURIER_FILTER_DEFAULT_SAMPLING_FREQUENCY (1000.0f)

/* Exported types ------------------------------------------------------------*/

/**
 * @brief 滤波器类型
 *
 */
typedef enum
{
    Filter_Fourier_Type_LOWPASS = 0,
    Filter_Fourier_Type_HIGHPASS,
    Filter_Fourier_Type_BANDPASS,
    Filter_Fourier_Type_BANDSTOP,
} Enum_Filter_Fourier_Type;

/**
 * @brief Reusable, Fourier滤波器算法
 *
 */
#define FILTER_FOURIER_ORDER 10

typedef struct
{
    float Value_Constrain_Low;
    float Value_Constrain_High;
    Enum_Filter_Fourier_Type Filter_Fourier_Type;
    float Frequency_Low;
    float Frequency_High;
    float Sampling_Frequency;
    float System_Function[FILTER_FOURIER_ORDER + 1];
    float Input_Signal[FILTER_FOURIER_ORDER + 1];
    uint8_t Signal_Flag;
    float Out;
} Class_Filter_Fourier;

/**
 * @brief Reusable, Kalman滤波器算法
 *
 */
typedef struct
{
    float Error_Measure;
    float Error_Estimate;
    float Kalman_Gain;
    float Out;
    float Now;
} Class_Filter_Kalman;

/* Exported function declarations --------------------------------------------*/

void Filter_Fourier_Init(Class_Filter_Fourier *filter, float __Value_Constrain_Low, float __Value_Constrain_High,
                         Enum_Filter_Fourier_Type __Filter_Fourier_Type, float __Frequency_Low, float __Frequency_High,
                         float __Sampling_Frequency);

float Filter_Fourier_Get_Out(Class_Filter_Fourier *filter);

void Filter_Fourier_Set_Now(Class_Filter_Fourier *filter, float __Now);

void Filter_Fourier_TIM_Calculate_PeriodElapsedCallback(Class_Filter_Fourier *filter);

void Filter_Kalman_Init(Class_Filter_Kalman *filter, float __Error_Measure, float __Now, float __Error_Estimate);

float Filter_Kalman_Get_Out(Class_Filter_Kalman *filter);

void Filter_Kalman_Set_Now(Class_Filter_Kalman *filter, float __Now);


#endif