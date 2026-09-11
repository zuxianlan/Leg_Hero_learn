/**
  ****************************(C) COPYRIGHT 2019 DJI****************************
  * @file       remote_control.c/h
  * @brief      遥控器处理，遥控器是通过类似SBUS的协议传输，利用DMA传输方式节约CPU
  *             资源，利用串口空闲中断来拉起处理函数，同时提供一些掉线重启DMA，串口
  *             的方式保证热插拔的稳定性。
  * @note       该任务是通过串口中断启动，不是freeRTOS任务
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. done
  *  V1.0.0     Nov-11-2019     RM              1. support development board tpye c
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 DJI****************************
  */

#include "remote_control.h"


#include "main.h"
#include "usart.h"
#include "string.h"

//#include "detect_task.h"


//遥控器出错数据上限
#define RC_CHANNAL_ERROR_VALUE 800

extern UART_HandleTypeDef huart5;
extern DMA_HandleTypeDef hdma_uart5_rx;


//取正函数
static int16_t RC_abs(int16_t value);

/**
  * @brief          remote control protocol resolution
  * @param[in]      sbus_buf: raw data point
  * @param[out]     rc_ctrl: remote control data struct point
  * @retval         none
  */
/**
  * @brief          遥控器协议解析
  * @param[in]      sbus_buf: 原生数据指针
  * @param[out]     rc_ctrl: 遥控器数据指
  * @retval         none
  */
static void sbus_to_rc(volatile const uint8_t *sbus_buf, RC_ctrl_t *rc_ctrl);

//remote control data
//遥控器控制变量
RC_ctrl_t rc_ctrl;
//接收原始数据，为18个字节，给了36个字节长度，防止DMA传输越界
//现S_BUS为25个字节
uint8_t sbus_rx_buf[2][SBUS_RX_BUF_NUM];

void RC_Init(UART_HandleTypeDef *huart, uint8_t *DstAddress, uint8_t *SecondMemAddress, uint32_t DataLength)
{
	huart->ReceptionType = HAL_UART_RECEPTION_TOIDLE;

	huart->RxEventType = HAL_UART_RXEVENT_IDLE;

	huart->RxXferSize = DataLength;

	SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);

	__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);

	HAL_DMAEx_MultiBufferStart(huart->hdmarx, (uint32_t) &huart->Instance->RDR, (uint32_t) DstAddress,
	                           (uint32_t) SecondMemAddress, DataLength);
}


/**
  * @brief          remote control init
  * @param[in]      none
  * @retval         none
  */
/**
  * @brief          遥控器初始化
  * @param[in]      none
  * @retval         none
  */
void remote_control_init(void)
{
	RC_Init(&huart5, sbus_rx_buf[0], sbus_rx_buf[1], 50);
}

/**
  * @brief          get remote control data point
  * @param[in]      none
  * @retval         remote control data point
  */
/**
  * @brief          获取遥控器数据指针
  * @param[in]      none
  * @retval         遥控器数据指针
  */
const RC_ctrl_t *get_remote_control_point(void)
{
	return &rc_ctrl;
}

//判断遥控器数据是否出错，
uint8_t RC_data_is_error(void)
{
	//使用了go to语句 方便出错统一处理遥控器变量数据归零
	if (RC_abs(rc_ctrl.rc.ch[0]) > RC_CHANNAL_ERROR_VALUE)
	{
		goto error;
	}
	if (RC_abs(rc_ctrl.rc.ch[1]) > RC_CHANNAL_ERROR_VALUE)
	{
		goto error;
	}
	if (RC_abs(rc_ctrl.rc.ch[2]) > RC_CHANNAL_ERROR_VALUE)
	{
		goto error;
	}
	if (RC_abs(rc_ctrl.rc.ch[3]) > RC_CHANNAL_ERROR_VALUE)
	{
		goto error;
	}
	if (rc_ctrl.rc.s[0] == 0)
	{
		goto error;
	}
	if (rc_ctrl.rc.s[1] == 0)
	{
		goto error;
	}
	return 0;

error:
	rc_ctrl.rc.ch[0] = 0;
	rc_ctrl.rc.ch[1] = 0;
	rc_ctrl.rc.ch[2] = 0;
	rc_ctrl.rc.ch[3] = 0;
	rc_ctrl.rc.ch[4] = 0;
	rc_ctrl.rc.s[0] = RC_SW_DOWN;
	rc_ctrl.rc.s[1] = RC_SW_DOWN;
	rc_ctrl.mouse.x = 0;
	rc_ctrl.mouse.y = 0;
	rc_ctrl.mouse.z = 0;
	rc_ctrl.mouse.press_l = 0;
	rc_ctrl.mouse.press_r = 0;
	rc_ctrl.key.v = 0;
	return 1;
}

//void slove_RC_lost(void)
//{
//    RC_restart(SBUS_RX_BUF_NUM);
//}
//void slove_data_error(void)
//{
//    RC_restart(SBUS_RX_BUF_NUM);
//}


void USER_USART5_RxHandler(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (((((DMA_Stream_TypeDef *) huart->hdmarx->Instance)->CR) & DMA_SxCR_CT) == RESET)
	{
		__HAL_DMA_DISABLE(huart->hdmarx);

		((DMA_Stream_TypeDef *) huart->hdmarx->Instance)->CR |= DMA_SxCR_CT;

		__HAL_DMA_SET_COUNTER(huart->hdmarx, SBUS_RX_BUF_NUM);

		if (Size == RC_FRAME_LENGTH)
		{
			sbus_to_rc(sbus_rx_buf[0], &rc_ctrl);
		}
	}
	else
	{
		__HAL_DMA_DISABLE(huart->hdmarx);

		((DMA_Stream_TypeDef *) huart->hdmarx->Instance)->CR &= ~(DMA_SxCR_CT);

		__HAL_DMA_SET_COUNTER(huart->hdmarx, SBUS_RX_BUF_NUM);

		if (Size == RC_FRAME_LENGTH)
		{
			sbus_to_rc(sbus_rx_buf[1], &rc_ctrl);
		}
	}
	__HAL_DMA_ENABLE(huart->hdmarx);
}


//取正函数
static int16_t RC_abs(int16_t value)
{
	if (value > 0)
	{
		return value;
	}
	else
	{
		return -value;
	}
}

/**
  * @brief          remote control protocol resolution
  * @param[in]      sbus_buf: raw data point
  * @param[out]     rc_ctrl: remote control data struct point
  * @retval         none
  */
/**
  * @brief          遥控器协议解析
  * @param[in]      sbus_buf: 原生数据指针
  * @param[out]     rc_ctrl: 遥控器数据指
  * @retval         none
  */
static void sbus_to_rc(volatile const uint8_t *sbus_buf, RC_ctrl_t *rc_ctrl)
{
	if (sbus_buf == NULL || rc_ctrl == NULL)
	{
		return;
	}

//    rc_ctrl->rc.ch[0] = (sbus_buf[0] | (sbus_buf[1] << 8)) & 0x07ff;        //!< Channel 0
//    rc_ctrl->rc.ch[1] = ((sbus_buf[1] >> 3) | (sbus_buf[2] << 5)) & 0x07ff; //!< Channel 1
//    rc_ctrl->rc.ch[2] = ((sbus_buf[2] >> 6) | (sbus_buf[3] << 2) |          //!< Channel 2
//                         (sbus_buf[4] << 10)) &0x07ff;
//    rc_ctrl->rc.ch[3] = ((sbus_buf[4] >> 1) | (sbus_buf[5] << 7)) & 0x07ff; //!< Channel 3
//    rc_ctrl->rc.s[0] = ((sbus_buf[5] >> 4) & 0x0003);                  //!< Switch left
//    rc_ctrl->rc.s[1] = ((sbus_buf[5] >> 4) & 0x000C) >> 2;                       //!< Switch right
//    rc_ctrl->mouse.x = sbus_buf[6] | (sbus_buf[7] << 8);                    //!< Mouse X axis
//    rc_ctrl->mouse.y = sbus_buf[8] | (sbus_buf[9] << 8);                    //!< Mouse Y axis
//    rc_ctrl->mouse.z = sbus_buf[10] | (sbus_buf[11] << 8);                  //!< Mouse Z axis
//    rc_ctrl->mouse.press_l = sbus_buf[12];                                  //!< Mouse Left Is Press ?
//    rc_ctrl->mouse.press_r = sbus_buf[13];                                  //!< Mouse Right Is Press ?
//    rc_ctrl->key.v = sbus_buf[14] | (sbus_buf[15] << 8);                    //!< KeyBoard value
//    rc_ctrl->rc.ch[4] = sbus_buf[16] | (sbus_buf[17] << 8);                 //NULL

	if (sbus_buf[0] == 0x0f && sbus_buf[23] == 0x00)
	{
		rc_ctrl->SBUS.CH[1] = (sbus_buf[1] >> 0 | (sbus_buf[2] << 8)) & 0x07ff;
		rc_ctrl->SBUS.CH[2] = (sbus_buf[2] >> 3 | (sbus_buf[3] << 5)) & 0x07ff;
		rc_ctrl->SBUS.CH[3] = ((sbus_buf[3] >> 6) | (sbus_buf[4] << 2) | (sbus_buf[5] << 10)) & 0x07ff;
		rc_ctrl->SBUS.CH[4] = ((sbus_buf[5] >> 1) | (sbus_buf[6] << 7)) & 0x07ff;
		rc_ctrl->SBUS.CH[5] = ((sbus_buf[6] >> 4) | (sbus_buf[7] << 4)) & 0x07ff;
		rc_ctrl->SBUS.CH[6] = ((sbus_buf[7] >> 7) | (sbus_buf[8] << 1) | (sbus_buf[9] << 9)) & 0x07ff;
		rc_ctrl->SBUS.CH[7] = ((sbus_buf[9] >> 2) | (sbus_buf[10] << 6)) & 0x07ff;
		rc_ctrl->SBUS.CH[8] = ((sbus_buf[10] >> 5) | (sbus_buf[11] << 3)) & 0x07ff;
		rc_ctrl->SBUS.CH[9] = ((sbus_buf[12] << 0) | (sbus_buf[13] << 8)) & 0x07ff;
		rc_ctrl->SBUS.CH[10] = ((sbus_buf[13] >> 3) | (sbus_buf[14] << 5)) & 0x07ff;
		rc_ctrl->SBUS.CH[11] = ((sbus_buf[14] >> 6) | (sbus_buf[15] << 2) | (sbus_buf[16] << 10)) & 0x07ff;
		rc_ctrl->SBUS.CH[12] = ((sbus_buf[16] >> 1) | (sbus_buf[17] << 7)) & 0x07ff;
		rc_ctrl->SBUS.CH[13] = ((sbus_buf[17] >> 4) | (sbus_buf[18] << 4)) & 0x07ff;
		rc_ctrl->SBUS.CH[14] = ((sbus_buf[18] >> 7) | (sbus_buf[19] << 1) | (sbus_buf[20] << 9)) & 0x07ff;
		rc_ctrl->SBUS.CH[15] = ((sbus_buf[20] >> 2) | (sbus_buf[21] << 6)) & 0x07ff;
		rc_ctrl->RC.ch[1] = (rc_ctrl->SBUS.CH[1] - 1024) / 783.0f;
		rc_ctrl->RC.ch[2] = (rc_ctrl->SBUS.CH[2] - 1024) / 783.0f;
		rc_ctrl->RC.ch[3] = (rc_ctrl->SBUS.CH[3] - 1024) / 783.0f;
		rc_ctrl->RC.ch[4] = (rc_ctrl->SBUS.CH[4] - 1024) / 783.0f;
		rc_ctrl->RC.rotory_sw[1] = (rc_ctrl->SBUS.CH[5] - 240) / 1567.0f;
		rc_ctrl->RC.rotory_sw[2] = (rc_ctrl->SBUS.CH[6] - 240) / 1567.0f;
		switch (rc_ctrl->SBUS.CH[7])
		{
		case 240:
			rc_ctrl->RC.sw[1] = 1;
			break;

		case 1807:
			rc_ctrl->RC.sw[1] = 2;
			break;
		}
		switch (rc_ctrl->SBUS.CH[8])
		{
		case 240:
			rc_ctrl->RC.sw[2] = 1;
			break;

		case 1807:
			rc_ctrl->RC.sw[2] = 2;
			break;
		}
		switch (rc_ctrl->SBUS.CH[9])
		{
		case 240:
			rc_ctrl->RC.sw[3] = 1;
			break;

		case 1807:
			rc_ctrl->RC.sw[3] = 2;
			break;
		case 1024:
			rc_ctrl->RC.sw[3] = 3;
			break;
		}
		switch (rc_ctrl->SBUS.CH[10])
		{
		case 240:
			rc_ctrl->RC.sw[4] = 1;
			break;

		case 1807:
			rc_ctrl->RC.sw[4] = 2;
			break;
		}
	}

	rc_ctrl->rc.ch[0] -= RC_CH_VALUE_OFFSET;
	rc_ctrl->rc.ch[1] -= RC_CH_VALUE_OFFSET;
	rc_ctrl->rc.ch[2] -= RC_CH_VALUE_OFFSET;
	rc_ctrl->rc.ch[3] -= RC_CH_VALUE_OFFSET;
	rc_ctrl->rc.ch[4] -= RC_CH_VALUE_OFFSET;
}

