#include "filter.h"


void Filter_Fourier_Init(Class_Filter_Fourier *filter, float __Value_Constrain_Low, float __Value_Constrain_High,
                         Enum_Filter_Fourier_Type __Filter_Fourier_Type, float __Frequency_Low, float __Frequency_High,
                         float __Sampling_Frequency)
{
    filter->Value_Constrain_Low = __Value_Constrain_Low;
    filter->Value_Constrain_High = __Value_Constrain_High;
    filter->Filter_Fourier_Type = __Filter_Fourier_Type;
    filter->Frequency_Low = __Frequency_Low;
    filter->Frequency_High = __Frequency_High;
    filter->Sampling_Frequency = __Sampling_Frequency;
    filter->Signal_Flag = 0;
    filter->Out = 0.0f;

    float system_function_sum = 0.0f;
    float omega_low = 2.0f * PI * filter->Frequency_Low / filter->Sampling_Frequency;
    float omega_high = 2.0f * PI * filter->Frequency_High / filter->Sampling_Frequency;

    switch (filter->Filter_Fourier_Type)
    {
    case Filter_Fourier_Type_LOWPASS:
        for (int i = 0; i < FILTER_FOURIER_ORDER + 1; i++)
        {
            filter->System_Function[i] = omega_low / PI * Math_Sinc((i - FILTER_FOURIER_ORDER / 2.0f) * omega_low);
        }
        break;
    case Filter_Fourier_Type_HIGHPASS:
        for (int i = 0; i < FILTER_FOURIER_ORDER + 1; i++)
        {
            filter->System_Function[i] = Math_Sinc((i - FILTER_FOURIER_ORDER / 2.0f) * PI) - omega_high / PI *
                                         Math_Sinc((i - FILTER_FOURIER_ORDER / 2.0f) * omega_high);
        }
        break;
    case Filter_Fourier_Type_BANDPASS:
        for (int i = 0; i < FILTER_FOURIER_ORDER + 1; i++)
        {
            filter->System_Function[i] = omega_high / PI * Math_Sinc((i - FILTER_FOURIER_ORDER / 2.0f) * omega_high) -
                                         omega_low / PI * Math_Sinc((i - FILTER_FOURIER_ORDER / 2.0f) * omega_low);
        }
        break;
    case Filter_Fourier_Type_BANDSTOP:
        for (int i = 0; i < FILTER_FOURIER_ORDER + 1; i++)
        {
            filter->System_Function[i] = Math_Sinc((i - FILTER_FOURIER_ORDER / 2.0f) * PI) + omega_low / PI *
                                         Math_Sinc((i - FILTER_FOURIER_ORDER / 2.0f) * omega_low) - omega_high / PI *
                                         Math_Sinc((i - FILTER_FOURIER_ORDER / 2.0f) * omega_high);
        }
        break;
    }

    for (int i = 0; i < FILTER_FOURIER_ORDER + 1; i++)
    {
        system_function_sum += filter->System_Function[i];
    }

    for (int i = 0; i < FILTER_FOURIER_ORDER + 1; i++)
    {
        filter->System_Function[i] /= system_function_sum;
    }
}

void Filter_Fourier_TIM_Calculate_PeriodElapsedCallback(Class_Filter_Fourier *filter)
{
    filter->Out = 0.0f;

    for (int i = 0; i < FILTER_FOURIER_ORDER + 1; i++)
    {
        filter->Out += filter->System_Function[i] * filter->Input_Signal[
            (filter->Signal_Flag + i) % (FILTER_FOURIER_ORDER + 1)];
    }
}

float Filter_Fourier_Get_Out(Class_Filter_Fourier *filter)
{
    return filter->Out;
}

void Filter_Fourier_Set_Now(Class_Filter_Fourier *filter, float __Now)
{
    if (filter->Value_Constrain_Low != 0.0f || filter->Value_Constrain_High != 0.0f)
    {
        Math_Constrain(&__Now, filter->Value_Constrain_Low, filter->Value_Constrain_High);
    }

    filter->Input_Signal[filter->Signal_Flag] = __Now;
    filter->Signal_Flag++;

    if (filter->Signal_Flag == FILTER_FOURIER_ORDER + 1)
    {
        filter->Signal_Flag = 0;
    }
}

void Filter_Kalman_Init(Class_Filter_Kalman *filter, float __Error_Measure, float __Now, float __Error_Estimate)
{
    filter->Error_Measure = __Error_Measure;
    filter->Now = __Now;
    filter->Error_Estimate = __Error_Estimate;
    filter->Kalman_Gain = 0.0f;
    filter->Out = 0.0f;
}

float Filter_Kalman_Get_Out(Class_Filter_Kalman *filter)
{
    return filter->Out;
}

void Filter_Kalman_Set_Now(Class_Filter_Kalman *filter, float __Now)
{
    filter->Now = __Now;
}