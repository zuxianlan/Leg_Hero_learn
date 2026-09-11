#include "Mathh.h"


/**
 * @brief 限幅函数
 *
 * @tparam Type 类型
 * @param x 传入数据
 * @param Min 最小值
 * @param Max 最大值
 */
float Math_Constrain(float *x, float Min, float Max)
{
    if (*x < Min)
    {
        *x = Min;
    }
    else if (*x > Max)
    {
        *x = Max;
    }
    return *x;
}

/**
 * @brief 反转16位数据的字节序
 *
 * @param Source 源数据指针
 * @param Destination 目标数据指针，如果为 NULL 则不进行拷贝
 * @return uint16_t 反转后的16位数据
 */
uint16_t Math_Endian_Reverse_16(void *Source, void *Destination)
{
    uint8_t *temp_address_8 = (uint8_t *) Source;
    uint16_t temp_address_16;
    temp_address_16 = (uint16_t) (temp_address_8[0] << 8 | temp_address_8[1]);

    if (Destination != NULL)
    {
        uint8_t *temp_destination = (uint8_t *) Destination;
        temp_destination[0] = temp_address_8[1];
        temp_destination[1] = temp_address_8[0];
    }

    return temp_address_16;
}

/**
 * @brief 将整数范围映射到浮点数范围
 *
 * @param x 输入的整数值
 * @param Int_Min 整数范围的最小值
 * @param Int_Max 整数范围的最大值
 * @param Float_Min 浮点数范围的最小值
 * @param Float_Max 浮点数范围的最大值
 * @return float 映射后的浮点数值
 */
float Math_Int_To_Float(int32_t x, int32_t Int_Min, int32_t Int_Max, float Float_Min, float Float_Max)
{
    float tmp = (float) (x - Int_Min) / (float) (Int_Max - Int_Min);
    float out = tmp * (Float_Max - Float_Min) + Float_Min;
    return out;
}

/**
 * @brief 将浮点数映射到整型
 *
 * @param x 浮点数
 * @param Float_Min 浮点数最小值
 * @param Float_Max 浮点数最大值
 * @param Int_Min 整型最小值
 * @param Int_Max 整型最大值
 * @return int32_t 整型
 */
int32_t Math_Float_To_Int(float x, float Float_Min, float Float_Max, int32_t Int_Min, int32_t Int_Max)
{
    float tmp = (x - Float_Min) / (Float_Max - Float_Min);
    int32_t out = tmp * (float) (Int_Max - Int_Min) + Int_Min;
    return (out);
}

float Float_Math_Abs(float x)
{
    return ((x > 0) ? x : -x);
}

/**
 * @brief sinc函数的实现
 *
 * @param x 输入
 * @return float 输出
 */
float Math_Sinc(float x)
{
    // 分母为0则按极限求法
    if (Float_Math_Abs(x) <= 2.0f * FLT_EPSILON)
    {
        return (1.0f);
    }

    return (arm_sin_f32(x) / x);
}

float Math_Modulus_Normalization(float x, float modulus)
{
    float tmp;

    tmp = fmod(x + modulus / 2.0f, modulus);

    if (tmp < 0.0f)
    {
        tmp += modulus;
    }

    return (tmp - modulus / 2.0f);
}

void normalizeAngle(float *angle)
{
    float normalized = fmod(*angle, 2.0 * M_PI);

    // 确保余数在 [0, 2π)
    if (normalized < 0)
    {
        normalized += 2.0 * M_PI;
    }

    // 如果大于等于 π，减去 2π 得到 [-π, π)
    if (normalized >= M_PI)
    {
        normalized -= 2.0 * M_PI;
    }

    *angle = normalized;
}

float normalizeAngleToPi_Robust(float angle)
{
    // 利用atan2的周期性，直接返回 [-π, π] 范围内的角度
    return atan2(sin(angle), cos(angle));
}

