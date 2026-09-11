// /**
//   ******************************************************************************
//   * @file           : open_source_4010.c
//   * @author         : gagami
//   * @brief          : 开源的FOC小电机4010和2804的控制变量，均采用电压控制
//   * @attention      : None
//   * @date           : 2025/8/5
//   ******************************************************************************
//   */
//
// /* Includes ------------------------------------------------------------------*/
// #include "open_source_4010.h"
// /* Define --------------------------------------------------------------------*/
//
// /* Enum ----------------------------------------------------------------------*/
//
// /* Struct --------------------------------------------------------------------*/
//
// /* Function Declaration ------------------------------------------------------*/
//
// /* Function ------------------------------------------------------------------*/
//
// /**
//  * @brief 分配CAN发送缓冲区
//  *
//  * @param hcan CAN编号
//  * @param __CAN_Rx_ID CAN ID
//  * @return uint8_t* 缓冲区指针
//  */
// uint8_t *allocate_tx_data_4010(FDCAN_HandleTypeDef *hcan, Enum_Motor_4010 __CAN_Rx_ID)
// {
//     uint8_t *tmp_tx_data_ptr = NULL;
//     if (hcan == &hfdcan1)
//     {
//         switch (__CAN_Rx_ID)
//         {
//         case Motor_ID_0x101:
//             tmp_tx_data_ptr = &(CAN1_0x100_Tx_Data[0]);
//             break;
//         case Motor_ID_0x102:
//             tmp_tx_data_ptr = &(CAN1_0x100_Tx_Data[2]);
//             break;
//         case Motor_ID_0x103:
//             tmp_tx_data_ptr = &(CAN1_0x100_Tx_Data[4]);
//             break;
//         case Motor_ID_0x104:
//             tmp_tx_data_ptr = &(CAN1_0x100_Tx_Data[6]);
//             break;
//         case Motor_ID_0x105:
//             tmp_tx_data_ptr = &(CAN1_0x200_Tx_Data[0]);
//             break;
//         case Motor_ID_0x106:
//             tmp_tx_data_ptr = &(CAN1_0x200_Tx_Data[2]);
//             break;
//         case Motor_ID_0x107:
//             tmp_tx_data_ptr = &(CAN1_0x200_Tx_Data[4]);
//             break;
//         case Motor_ID_0x108:
//             tmp_tx_data_ptr = &(CAN1_0x200_Tx_Data[6]);
//             break;
//         }
//     }
//
// 	else if (hcan == &hfdcan2)
//     {
// 	    switch (__CAN_Rx_ID)
// 	    {
// 	    case Motor_ID_0x101:
// 	        tmp_tx_data_ptr = &(CAN2_0x100_Tx_Data[0]);
// 	        break;
// 	    case Motor_ID_0x102:
// 	        tmp_tx_data_ptr = &(CAN2_0x100_Tx_Data[2]);
// 	        break;
// 	    case Motor_ID_0x103:
// 	        tmp_tx_data_ptr = &(CAN2_0x100_Tx_Data[4]);
// 	        break;
// 	    case Motor_ID_0x104:
// 	        tmp_tx_data_ptr = &(CAN2_0x100_Tx_Data[6]);
// 	        break;
// 	    case Motor_ID_0x105:
// 	        tmp_tx_data_ptr = &(CAN2_0x200_Tx_Data[0]);
// 	        break;
// 	    case Motor_ID_0x106:
// 	        tmp_tx_data_ptr = &(CAN2_0x200_Tx_Data[2]);
// 	        break;
// 	    case Motor_ID_0x107:
// 	        tmp_tx_data_ptr = &(CAN2_0x200_Tx_Data[4]);
// 	        break;
// 	    case Motor_ID_0x108:
// 	        tmp_tx_data_ptr = &(CAN2_0x200_Tx_Data[6]);
// 	        break;
// 	    }
//     }
//
// 	else if (hcan == &hfdcan3)
//     {
// 	    switch (__CAN_Rx_ID)
// 	    {
// 	    case Motor_ID_0x101:
// 	        tmp_tx_data_ptr = &(CAN3_0x100_Tx_Data[0]);
// 	        break;
// 	    case Motor_ID_0x102:
// 	        tmp_tx_data_ptr = &(CAN3_0x100_Tx_Data[2]);
// 	        break;
// 	    case Motor_ID_0x103:
// 	        tmp_tx_data_ptr = &(CAN3_0x100_Tx_Data[4]);
// 	        break;
// 	    case Motor_ID_0x104:
// 	        tmp_tx_data_ptr = &(CAN3_0x100_Tx_Data[6]);
// 	        break;
// 	    case Motor_ID_0x105:
// 	        tmp_tx_data_ptr = &(CAN3_0x200_Tx_Data[0]);
// 	        break;
// 	    case Motor_ID_0x106:
// 	        tmp_tx_data_ptr = &(CAN3_0x200_Tx_Data[2]);
// 	        break;
// 	    case Motor_ID_0x107:
// 	        tmp_tx_data_ptr = &(CAN3_0x200_Tx_Data[4]);
// 	        break;
// 	    case Motor_ID_0x108:
// 	        tmp_tx_data_ptr = &(CAN3_0x200_Tx_Data[6]);
// 	        break;
// 	    }
//     }
//     return tmp_tx_data_ptr;
// }
//
// /**
//  * @brief 电机初始化
//  *
//  * @param motor 电机结构体
//  * @param hcan can通信句柄
//  * @param __CAN_ID 接收的can id、
//  * @param __Control_Method 控制模式
//  * @param __Encoder_Offset 编码器偏移，调零点
//  * @param __Calc_Rev_Volt 电机反电动势计算函数指针
//  * @param __Torque_Ratio 电压力矩转换系数
//  * @param __Voltage_Max 最大控制电压
//  * @return
//  */
// void Motor_Open_Source_4010_Init(Motor_Open_Source_4010 *motor, FDCAN_HandleTypeDef *hcan, Enum_Motor_4010 __CAN_ID, Enum_Control_Method __Control_Method, float __Encoder_Offset, Calc_Rev_Volt_t __Calc_Rev_Volt, float __Torque_Ratio, float __Voltage_Max, int8_t __dir)
// {
// 	if (hcan->Instance == FDCAN1)
// 	{
// 		motor->CAN_Manage_Object = &CAN1_Manage_Object;
// 	}
// 	else if (hcan->Instance == FDCAN2)
// 	{
// 		motor->CAN_Manage_Object = &CAN2_Manage_Object;
// 	}
// 	else if (hcan->Instance == FDCAN3)
// 	{
// 		motor->CAN_Manage_Object = &CAN3_Manage_Object;
// 	}
//
// 	motor->CAN_ID = __CAN_ID;
// 	motor->Control_Method = __Control_Method;
// 	motor->Encoder_Offset = __Encoder_Offset;
// 	motor->Calc_Rev_Volt = __Calc_Rev_Volt;
// 	motor->Torque_Ratio = __Torque_Ratio;
// 	motor->Voltage_Max = __Voltage_Max;
// 	motor->dir = __dir;
// 	motor->CAN_Tx_Data = allocate_tx_data_4010(hcan, __CAN_ID);
// }
//
// /**
//  * @brief 输出到电机
//  *
//  * @param
//  * @return
//  */
// void Motor_Open_Source_4010_Output(Motor_Open_Source_4010 *motor)
// {
// 	motor->CAN_Tx_Data[0] = (int16_t)(motor->Out) >> 8;
// 	motor->CAN_Tx_Data[1] = (int16_t)(motor->Out);
// }
//
// /**
//  * @brief 设定输出
//  *
//  * @param
//  * @return
//  */
// void Motor_Open_Source_4010_Set_Out(Motor_Open_Source_4010 *motor, float __Out)
// {
// 	motor->Out = __Out;
// }
//
// /**
//  * @brief 接收回调函数
//  *
//  * @param
//  * @return
//  */
// void Motor_Open_Source_4010_CAN_RxCpltCallback(Motor_Open_Source_4010 *motor, uint8_t *Rx_Data)
// {
// 	motor->Flag += 1;
//
// 	Struct_Motor_Open_CAN_Rx_Data *tmp_buffer = (Struct_Motor_Open_CAN_Rx_Data *)motor->CAN_Manage_Object->Rx_Buffer.Data;
//
// 	motor->Rx_Data.Now_Angle = motor->dir * (((float)tmp_buffer->Now_Angle / 1000.0f) - motor->Encoder_Offset);
// 	motor->Rx_Data.Now_Omega = motor->dir * ((float)tmp_buffer->Now_Omega / 10.0f * 2.0f * PI / 60.0f);
// }
//
// /**
//  * @brief 定期检查电机是否在线
//  *
//  * @param
//  * @return
//  */
// void Motor_Open_Source_4010_TIM_Alive_PeriodElapsedCallback(Motor_Open_Source_4010 *motor)
// {
// 	if (motor->Flag == motor->Pre_Flag)
// 	{
// 		// motor->CAN_Motor_Status = CAN_Motor_Status_DISABLE;
// 		PID_Set_Integral_Error(&motor->PID_Angle, 0.0f);
// 		PID_Set_Integral_Error(&motor->PID_Omega, 0.0f);
// 		PID_Set_Integral_Error(&motor->PID_Current, 0.0f);
// 	}
// 	else
// 	{
// 		// motor->CAN_Motor_Status = CAN_Motor_Status_ENABLE;
// 	}
// 	motor->Pre_Flag = motor->Flag;
// }
//
// /**
//  * @brief 电机周期计算函数，PID控制器控制电机运转
//  *
//  * @param
//  * @return
//  */
// void Motor_Open_Source_4010_TIM_Calculate_PeriodElapsedCallback(Motor_Open_Source_4010 *motor)
// {
// 	Filter_Fourier_Set_Now(&motor->Filter_Fourier_Omega, motor->Rx_Data.Now_Omega);
// 	Filter_Fourier_TIM_Calculate_PeriodElapsedCallback(&motor->Filter_Fourier_Omega);
//
// 	Motor_Open_Source_4010_TIM_PID_PeriodElapsedCallback(motor);
//
// 	float tmp_value = motor->Target_Voltage + motor->Feedforward_Voltage;
// 	if (motor->Rx_Data.Now_Omega >= 0)
// 	{
// 		motor->Target_Voltage += motor->Calc_Rev_Volt(motor->Rx_Data.Now_Omega);
// 	}
// 	else if (motor->Rx_Data.Now_Omega < 0)
// 	{
// 		motor->Target_Voltage -= motor->Calc_Rev_Volt(motor->Rx_Data.Now_Omega);
// 	}
// 	Math_Constrain(&tmp_value, -motor->Voltage_Max, motor->Voltage_Max);
// 	motor->Out = tmp_value * Open_Source_4010_Current_To_Out * motor->dir;
//
// 	Motor_Open_Source_4010_Output(motor);
//
// }
//
// /**
//  * @brief PID控制器不同模式计算
//  *
//  * @param
//  * @return
//  */
// void Motor_Open_Source_4010_TIM_PID_PeriodElapsedCallback(Motor_Open_Source_4010 *motor)
// {
// 	switch (motor->Control_Method)
// 	{
// 	case Motor_DJI_Control_Method_VOLTAGE:
//
// 		break;
// 	case Motor_DJI_Control_Method_TORQUE:
//
// 		motor->Target_Voltage = motor->Target_Torque / motor->Torque_Ratio;
//
// 		break;
// 	case Motor_DJI_Control_Method_OMEGA:
//
// 		motor->PID_Omega.Target = motor->Target_Omega + motor->Feedforward_Omega;
// 		motor->PID_Omega.Now = motor->Rx_Data.Now_Omega;
// 		PID_TIM_Adjust_PeriodElapsedCallback(&motor->PID_Omega);
//
// 		motor->Target_Voltage = motor->PID_Omega.Out;
//
// 		break;
// 	case Motor_DJI_Control_Method_ANGLE:
//
// 		motor->PID_Angle.Target = motor->Target_Angle;
// 		motor->PID_Angle.Now = motor->Rx_Data.Now_Angle;
// 		PID_TIM_Adjust_PeriodElapsedCallback(&motor->PID_Angle);
//
// 		motor->Target_Omega = motor->PID_Angle.Out;
//
// 		motor->PID_Omega.Target = motor->Target_Omega + motor->Feedforward_Omega;
// 		motor->PID_Omega.Now = motor->Rx_Data.Now_Omega;
// 		PID_TIM_Adjust_PeriodElapsedCallback(&motor->PID_Omega);
//
// 		motor->Target_Voltage = motor->PID_Omega.Out;
//
// 		break;
// 	default:
//
// 		Motor_Open_Source_4010_Set_Out(motor, 0.0f);
//
// 		break;
// 	}
// }
//
// //4010电机反电动势计算函数(输入速度，输出反电动势)
// //测量并拟合出不同电压下对应的电机空载转速，调换自变量和因变量就是本函数
// //由于该测量方法忽略阻力对空载转速的影响，最终抵消反电动势时也会抵消大部分电机本身的阻力
// float Motor_CalcRevVolt4010(float speed)
// {
// 	return 0.00008f * speed * speed * speed - 0.0035f * speed * speed + 0.2322f * speed;
// }
//
// //2804电机反电动势计算函数(输入速度，输出反电动势)，测量方法同上
// float Motor_CalcRevVolt2804(float speed)
// {
// 	return 0.000004f * speed * speed * speed - 0.0003f * speed * speed + 0.0266f * speed;
// }