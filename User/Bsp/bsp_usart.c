#include "bsp_usart.h"
#include "main.h"
#include "string.h"
#include "remote_control.h"
#include "usart.h"
#include "Transmission_link.h"


extern ext_game_robot_HP_t game_robot_HP_t;
extern ext_game_state_t game_state;

extern DMA_HandleTypeDef hdma_usart10_rx;

uint8_t vision_rx_buf[2][VISION_RX_LEN_2];
vision_rxfifo_t vision_rxfifo = {0};

void vision_rx_decode(uint8_t * test_code);

void USER_USART10_RxHandler(UART_HandleTypeDef *huart, uint16_t Size);

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart == &huart5)
    {
        USER_USART5_RxHandler(huart, Size);
    }
    else if (huart == &huart10)
    {
        USER_USART10_RxHandler(huart, Size);
    }
    else if (huart == &huart1)
    {
        USER_USART1_RxHandler(huart, Size);
    }
}

void Radar_Init(UART_HandleTypeDef *huart, uint8_t *DstAddress, uint8_t *SecondMemAddress, uint32_t DataLength)
{
    huart->ReceptionType = HAL_UART_RECEPTION_TOIDLE;

    huart->RxEventType = HAL_UART_RXEVENT_IDLE;

    huart->RxXferSize = DataLength;

    SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);

    __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);

    HAL_DMAEx_MultiBufferStart(huart->hdmarx, (uint32_t) & huart->Instance->RDR, (uint32_t) DstAddress,
                               (uint32_t) SecondMemAddress, DataLength);
}

void RADAR_Init(void)
{
    Radar_Init(&huart10, vision_rx_buf[0], vision_rx_buf[1], VISION_RX_LEN_2);
}

void USER_USART10_RxHandler(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (((((DMA_Stream_TypeDef *) huart->hdmarx->Instance)->CR) & DMA_SxCR_CT) == RESET)
    {
        __HAL_DMA_DISABLE(huart->hdmarx);

        ((DMA_Stream_TypeDef *) huart->hdmarx->Instance)->CR |= DMA_SxCR_CT;

        __HAL_DMA_SET_COUNTER(huart->hdmarx, VISION_RX_LEN_2);

        if (Size == VISION_RX_LEN)
        {
            vision_rx_decode(vision_rx_buf[0]);
        }
    }
    else
    {
        __HAL_DMA_DISABLE(huart->hdmarx);

        ((DMA_Stream_TypeDef *) huart->hdmarx->Instance)->CR &= ~(DMA_SxCR_CT);

        __HAL_DMA_SET_COUNTER(huart->hdmarx, VISION_RX_LEN_2);

        if (Size == VISION_RX_LEN)
        {
            vision_rx_decode(vision_rx_buf[1]);
        }
    }
    __HAL_DMA_ENABLE(huart->hdmarx);
}


void vision_rx_decode(uint8_t *test_code)
{
    if ((uint8_t) test_code[0] == 0xA5)
    {
        memset(&vision_rxfifo, 0, sizeof(vision_rxfifo));

        memcpy(&vision_rxfifo, test_code, sizeof(vision_rxfifo));

        // vision_rxfifo.header = test_code[0];
        // vision_rxfifo.vx = *(float*)(&test_code[1]);
        // vision_rxfifo.vy = *(float*)(&test_code[5]);
        // vision_rxfifo.ang_z = *(float*)(&test_code[9]);
        // vision_rxfifo.spin_flag = *(uint8_t*)(&test_code[13]);

        // error_usarttime = 0;
    }
}

void REFREE_Init(void)
{
    // Refree_Init(&huart1,refree_rx_buf[0],refree_rx_buf[1],REFREE_RX_LEN_2);
    // HAL_UARTEx_ReceiveToIdle_DMA(&huart1, refree_rx_buf, REFREE_RX_LEN);
}

uint8_t UART_Send_Data(UART_HandleTypeDef *huart, uint8_t *Data, uint16_t Length)
{
    return (HAL_UART_Transmit_DMA(huart, Data, Length));
}