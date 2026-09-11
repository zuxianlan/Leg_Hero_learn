// /**
//   ******************************************************************************
//   * @file           : open_source_4010.h
//   * @author         : gagami
//   * @brief          : 开源的FOC小电机4010和2804的控制变量，均采用电压控制
//   * @attention      : None
//   * @date           : 2025/8/5
//   ******************************************************************************
//   */
//
// #ifndef OPEN_SOURCE_4010_H
// #define OPEN_SOURCE_4010_H
//
// /* Includes ------------------------------------------------------------------*/
//
// #include "fdcan.h"
// #include "CAN_receive.h"
// #include "PID_Control.h"
// #include "INS_task.h"
// #include "filter.h"
//
// /* Define --------------------------------------------------------------------*/
//
// #define MOTOR_ECD_TO_RAD 0.000766990394f //      2 * PI / 8192
// #define Open_Source_4010_Current_To_Out 1000.0f
//
// /* Enum ----------------------------------------------------------------------*/
//
// /**
//  * @brief 开源4010电机的ID枚举类型
//  *
//  */
// typedef enum
// {
//   Motor_ID_0x101 = 1,
//   Motor_ID_0x102,
//   Motor_ID_0x103,
//   Motor_ID_0x104,
//   Motor_ID_0x105,
//   Motor_ID_0x106,
//   Motor_ID_0x107,
//   Motor_ID_0x108,
// }Enum_Motor_4010;
//
// typedef enum
// {
//   CAN_Motor_Status_DISABLE = 0,
//   CAN_Motor_Status_ENABLE,
// }Enum_CAN_Motor_Status;
//
// typedef enum
// {
//   Motor_DJI_Control_Method_VOLTAGE = 0,
//   Motor_DJI_Control_Method_Velocity,
//   Motor_DJI_Control_Method_CURRENT,
//   Motor_DJI_Control_Method_TORQUE,
//   Motor_DJI_Control_Method_OMEGA,
//   Motor_DJI_Control_Method_ANGLE,
// }Enum_Control_Method;
// /* Struct --------------------------------------------------------------------*/
//
// // CAN通信原始数据
// typedef struct __attribute__((__packed__))
// {
//   int32_t Now_Angle;
//   int16_t Now_Omega;
//   int16_t reserved;
// }Struct_Motor_Open_CAN_Rx_Data;
//
// // 接收解算完成的数据
// typedef struct
// {
//   float Now_Angle;
//   float Now_Omega;
// }Struct_Motor_Open_Rx_Data;
//
// // 反电动势计算函数指针
// typedef float (*Calc_Rev_Volt_t)(float speed);
//
// // 电机结构体
// typedef struct
// {
//   Class_Filter_Fourier Filter_Fourier_Omega;
//
//   // PID角度环控制
//   PID_control PID_Angle;
//   // PID角速度环控制
//   PID_control PID_Omega;
//   // PID扭矩环控制
//   PID_control PID_Current;
//
//   // 初始化相关变量
//   Struct_CAN_Manage_Object *CAN_Manage_Object; // 绑定的CAN
//   Enum_Motor_4010 CAN_ID; // 收数据绑定的CAN ID
//   uint8_t *CAN_Tx_Data; // 发送缓存区
//   float dir;
//   float Encoder_Offset; // 中值角度标定
//   float Omega_Max; //最大速度
//   float Voltage_Max; // 最大电压
//   float Torque_Ratio; // 电压力矩转换系数
//
//   // 常量
//   uint16_t Encoder_Num_Per_Round; // 一圈编码器刻度
//   uint16_t Output_Max; // 最大输出电压
//
//   // 内部变量
//   uint32_t Flag; // 当前时刻的电机接收flag
//   uint32_t Pre_Flag; // 前一时刻的电机接收flag
//
//   // 电机对外接口信息
//   Struct_Motor_Open_Rx_Data Rx_Data;   //解算后数据
//
//   // 读变量
//   // Enum_CAN_Motor_Status CAN_Motor_Status; // 电机状态
//
//   // 写变量
//   Enum_Control_Method Control_Method; // 电机控制方式
//   float Target_Angle;         // 目标的角度
//   float Target_Omega;         // 目标的速度
//   float Target_Voltage;       // 目标的的电压
//   float Target_Current;       // 目标的的电流
//   float Target_Torque;        // 目标的力矩
//   float Feedforward_Omega;    // 前馈的速度, rad/s
//   float Feedforward_Voltage;  // 前馈的电压, V
//   float Feedforward_Current;  // 前馈的电流, A
//   float Out;                  // 输出量
//
//   Calc_Rev_Volt_t Calc_Rev_Volt;
//
// }Motor_Open_Source_4010;
//
// /* Function Declaration ------------------------------------------------------*/
// void Motor_Open_Source_4010_Init(Motor_Open_Source_4010 *motor, FDCAN_HandleTypeDef *hcan, Enum_Motor_4010 __CAN_ID, Enum_Control_Method __Control_Method, float __Encoder_Offset, Calc_Rev_Volt_t __Calc_Rev_Volt, float __Torque_Ratio, float __Voltage_Max, int8_t __dir);
// void Motor_Open_Source_4010_Output(Motor_Open_Source_4010 *motor);
// void Motor_Open_Source_4010_Set_Out(Motor_Open_Source_4010 *motor, float __Out);
// void Motor_Open_Source_4010_CAN_RxCpltCallback(Motor_Open_Source_4010 *motor, uint8_t *Rx_Data);
// void Motor_Open_Source_4010_TIM_Alive_PeriodElapsedCallback(Motor_Open_Source_4010 *motor);
// void Motor_Open_Source_4010_TIM_Calculate_PeriodElapsedCallback(Motor_Open_Source_4010 *motor);
// void Motor_Open_Source_4010_TIM_PID_PeriodElapsedCallback(Motor_Open_Source_4010 *motor);
// float Motor_CalcRevVolt4010(float speed);
// float Motor_CalcRevVolt2804(float speed);
//
// #endif //OPEN_SOURCE_4010_H