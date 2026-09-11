#include "CAN_receive.h"

#include "ChassisL_Task.h"
#include "fdcan.h"

Struct_CAN_Manage_Object CAN1_Manage_Object = {0};
Struct_CAN_Manage_Object CAN2_Manage_Object = {0};
Struct_CAN_Manage_Object CAN3_Manage_Object = {0};

// CAN通信发送缓冲区
uint8_t CAN1_0x100_Tx_Data[8];
uint8_t CAN1_0x1fe_Tx_Data[8];
uint8_t CAN1_0x1ff_Tx_Data[8];
uint8_t CAN1_0x200_Tx_Data[8];
uint8_t CAN1_0x2fe_Tx_Data[8];
uint8_t CAN1_0x2ff_Tx_Data[8];
uint8_t CAN1_0x3fe_Tx_Data[8];
uint8_t CAN1_0x4fe_Tx_Data[8];

uint8_t CAN2_0x100_Tx_Data[8];
uint8_t CAN2_0x1fe_Tx_Data[8];
uint8_t CAN2_0x1ff_Tx_Data[8];
uint8_t CAN2_0x200_Tx_Data[8];
uint8_t CAN2_0x2fe_Tx_Data[8];
uint8_t CAN2_0x2ff_Tx_Data[8];
uint8_t CAN2_0x3fe_Tx_Data[8];
uint8_t CAN2_0x4fe_Tx_Data[8];

uint8_t CAN3_0x100_Tx_Data[8];
uint8_t CAN3_0x1fe_Tx_Data[8];
uint8_t CAN3_0x1ff_Tx_Data[8];
uint8_t CAN3_0x200_Tx_Data[8];
uint8_t CAN3_0x2fe_Tx_Data[8];
uint8_t CAN3_0x2ff_Tx_Data[8];
uint8_t CAN3_0x3fe_Tx_Data[8];
uint8_t CAN3_0x4fe_Tx_Data[8];

uint8_t CAN_Board_Tx_Data[8];
uint8_t CAN_REFREE_Tx_Data[8];
uint8_t CAN_Super_Cap_Tx_Data[8];


/**
 * @brief 初始化CAN总线
 *
 * @param hcan CAN编号
 * @param Callback_Function 处理回调函数
 */
void CAN_Init(FDCAN_HandleTypeDef *hfdcan, CAN_Call_Back Callback_Function)
{
    if (hfdcan->Instance == FDCAN1)
    {
        CAN1_Manage_Object.CAN_Handler = hfdcan;
        CAN1_Manage_Object.Callback_Function = Callback_Function;

        FDCAN_FilterTypeDef fdcan_filter;

        // 配置滤波器0 - 接收所有标准ID
        fdcan_filter.IdType = FDCAN_STANDARD_ID; //标准ID
        fdcan_filter.FilterIndex = 0; //滤波器索引
        fdcan_filter.FilterType = FDCAN_FILTER_MASK;
        fdcan_filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; //过滤器0关联到FIFO0
        fdcan_filter.FilterID1 = 0x000; // ID = 0x000
        fdcan_filter.FilterID2 = 0x000; //

        HAL_FDCAN_ConfigFilter(&hfdcan1, &fdcan_filter);

        HAL_FDCAN_ConfigGlobalFilter(&hfdcan1,FDCAN_REJECT,FDCAN_REJECT,FDCAN_REJECT_REMOTE,FDCAN_REJECT_REMOTE);

        // 设置FIFO0水印为1 - 每收到1个消息就触发中断
        HAL_FDCAN_ConfigFifoWatermark(&hfdcan1, FDCAN_CFG_RX_FIFO0, 1);

        // 激活FIFO0新消息中断
        HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
        HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);

        HAL_FDCAN_Start(&hfdcan1);

        // HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE | // FIFO0新消息中断
        //                                          FDCAN_IT_RX_FIFO0_WATERMARK | // FIFO0水印中断
        //                                          FDCAN_IT_TX_COMPLETE | // 发送完成中断
        //                                          FDCAN_IT_TX_FIFO_EMPTY | // 发送FIFO空中断
        //                                          FDCAN_IT_BUS_OFF | // 总线关闭中断
        //                                          FDCAN_IT_ARB_PROTOCOL_ERROR | // 仲裁协议错误
        //                                          FDCAN_IT_DATA_PROTOCOL_ERROR | // 数据协议错误
        //                                          FDCAN_IT_ERROR_PASSIVE | // 被动错误
        //                                          FDCAN_IT_ERROR_WARNING, // 错误警告
        //                                0);
    }

    else if (hfdcan->Instance == FDCAN2)
    {
        CAN2_Manage_Object.CAN_Handler = hfdcan;
        CAN2_Manage_Object.Callback_Function = Callback_Function;

        FDCAN_FilterTypeDef sFilterConfig;
        sFilterConfig.IdType = FDCAN_STANDARD_ID; //扩展ID不接收
        sFilterConfig.FilterIndex = 0;
        sFilterConfig.FilterType = FDCAN_FILTER_MASK;
        sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
        sFilterConfig.FilterID1 = 0x00000000;
        sFilterConfig.FilterID2 = 0x00000000;

        HAL_FDCAN_ConfigFilter(&hfdcan2, &sFilterConfig);
        HAL_FDCAN_ConfigGlobalFilter(&hfdcan2, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);
        HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
        HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);
        HAL_FDCAN_Start(&hfdcan2);
    }

    else if (hfdcan->Instance == FDCAN3)
    {
        CAN3_Manage_Object.CAN_Handler = hfdcan;
        CAN3_Manage_Object.Callback_Function = Callback_Function;

        FDCAN_FilterTypeDef fdcan_filter;

        // 配置滤波器0 - 接收所有标准ID
        fdcan_filter.IdType = FDCAN_STANDARD_ID; //标准ID
        fdcan_filter.FilterIndex = 0; //滤波器索引
        fdcan_filter.FilterType = FDCAN_FILTER_MASK;
        fdcan_filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; //过滤器0关联到FIFO0
        fdcan_filter.FilterID1 = 0x000; // ID = 0x000
        fdcan_filter.FilterID2 = 0x000; //

        HAL_FDCAN_ConfigFilter(&hfdcan3, &fdcan_filter);

        HAL_FDCAN_ConfigGlobalFilter(&hfdcan3,FDCAN_REJECT,FDCAN_REJECT,FDCAN_REJECT_REMOTE,FDCAN_REJECT_REMOTE);

        // 设置FIFO0水印为1 - 每收到1个消息就触发中断
        HAL_FDCAN_ConfigFifoWatermark(&hfdcan3, FDCAN_CFG_RX_FIFO0, 1);

        // 激活FIFO0新消息中断
        HAL_FDCAN_ActivateNotification(&hfdcan3, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
        HAL_FDCAN_ActivateNotification(&hfdcan3, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);

        HAL_FDCAN_Start(&hfdcan3);

        // HAL_FDCAN_ActivateNotification(&hfdcan3, FDCAN_IT_RX_FIFO0_NEW_MESSAGE | // FIFO0新消息中断
        //                                          FDCAN_IT_RX_FIFO0_WATERMARK | // FIFO0水印中断
        //                                          FDCAN_IT_TX_COMPLETE | // 发送完成中断
        //                                          FDCAN_IT_TX_FIFO_EMPTY | // 发送FIFO空中断
        //                                          FDCAN_IT_BUS_OFF | // 总线关闭中断
        //                                          FDCAN_IT_ARB_PROTOCOL_ERROR | // 仲裁协议错误
        //                                          FDCAN_IT_DATA_PROTOCOL_ERROR | // 数据协议错误
        //                                          FDCAN_IT_ERROR_PASSIVE | // 被动错误
        //                                          FDCAN_IT_ERROR_WARNING, // 错误警告
        //                                0);

        // CAN3_Manage_Object.CAN_Handler = hfdcan;
        // CAN3_Manage_Object.Callback_Function = Callback_Function;
        //
        // FDCAN_FilterTypeDef sFilterConfig;
        // sFilterConfig.IdType = FDCAN_STANDARD_ID; //扩展ID不接收
        // sFilterConfig.FilterIndex = 0;
        // sFilterConfig.FilterType = FDCAN_FILTER_MASK;
        // sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
        // sFilterConfig.FilterID1 = 0x00000000;
        // sFilterConfig.FilterID2 = 0x00000000;
        //
        // HAL_FDCAN_ConfigFilter(&hfdcan3, &sFilterConfig);
        // HAL_FDCAN_ConfigGlobalFilter(&hfdcan3, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);
        // HAL_FDCAN_ActivateNotification(&hfdcan3, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
        // HAL_FDCAN_ActivateNotification(&hfdcan3, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);
        // HAL_FDCAN_Start(&hfdcan3);
    }
}

void FDCAN_Set_Baud(FDCAN_HandleTypeDef *hfdcan, uint8_t mode, uint8_t baud)
{
    uint32_t nom_brp = 0, nom_seg1 = 0, nom_seg2 = 0, nom_sjw = 0;
    uint32_t dat_brp = 0, dat_seg1 = 0, dat_seg2 = 0, dat_sjw = 0;

    /*	nominal_baud = 80M/brp/(1+seg1+seg2)
        sample point = (1+seg1)/(1+seg1+sjw)  */
    if (mode == CAN_CLASS)
    {
        switch (baud)
        {
        case CAN_BR_125K: nom_brp = 4;
            nom_seg1 = 139;
            nom_seg2 = 20;
            nom_sjw = 20;
            break; // sample point 87.5%
        case CAN_BR_200K: nom_brp = 2;
            nom_seg1 = 174;
            nom_seg2 = 25;
            nom_sjw = 25;
            break; // sample point 87.5%
        case CAN_BR_250K: nom_brp = 2;
            nom_seg1 = 139;
            nom_seg2 = 20;
            nom_sjw = 20;
            break; // sample point 87.5%
        case CAN_BR_500K: nom_brp = 1;
            nom_seg1 = 139;
            nom_seg2 = 20;
            nom_sjw = 20;
            break; // sample point 87.5%
        case CAN_BR_1M: nom_brp = 1;
            nom_seg1 = 59;
            nom_seg2 = 20;
            nom_sjw = 20;
            break; // sample point 75%
        }
        dat_brp = 1;
        dat_seg1 = 29;
        dat_seg2 = 10;
        dat_sjw = 10; // 仲裁域默认1M
        hfdcan->Init.FrameFormat = FDCAN_FRAME_CLASSIC;
    }
    /*	data_baud	 = 80M/brp/(1+seg1+seg2)
        sample point = (1+seg1)/(1+seg1+sjw)  */
    if (mode == CAN_FD_BRS)
    {
        switch (baud)
        {
        case CAN_BR_2M: dat_brp = 1;
            dat_seg1 = 29;
            dat_seg2 = 10;
            dat_sjw = 10;
            break; // sample point 75%
        case CAN_BR_2M5: dat_brp = 1;
            dat_seg1 = 25;
            dat_seg2 = 6;
            dat_sjw = 6;
            break; // sample point 81.25%
        case CAN_BR_3M2: dat_brp = 1;
            dat_seg1 = 19;
            dat_seg2 = 5;
            dat_sjw = 5;
            break; // sample point 80%
        case CAN_BR_4M: dat_brp = 1;
            dat_seg1 = 14;
            dat_seg2 = 5;
            dat_sjw = 5;
            break; // sample point 75%
        case CAN_BR_5M: dat_brp = 1;
            dat_seg1 = 13;
            dat_seg2 = 2;
            dat_sjw = 2;
            break; // sample point 87.5%
        }
        nom_brp = 1;
        nom_seg1 = 59;
        nom_seg2 = 20;
        nom_sjw = 20; // 数据域默认1M
        hfdcan->Init.FrameFormat = FDCAN_FRAME_FD_BRS;
    }

    HAL_FDCAN_DeInit(hfdcan);

    hfdcan->Init.NominalPrescaler = nom_brp;
    hfdcan->Init.NominalTimeSeg1 = nom_seg1;
    hfdcan->Init.NominalTimeSeg2 = nom_seg2;
    hfdcan->Init.NominalSyncJumpWidth = nom_sjw;

    hfdcan->Init.DataPrescaler = dat_brp;
    hfdcan->Init.DataTimeSeg1 = dat_seg1;
    hfdcan->Init.DataTimeSeg2 = dat_seg2;
    hfdcan->Init.DataSyncJumpWidth = dat_sjw;

    HAL_FDCAN_Init(hfdcan);
}

/**
 * @brief HAL库CAN接收FIFO0中断
 *
 * @param hcan CAN编号
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs)
{
    //选择回调函数
    if (hfdcan->Instance == FDCAN1)
    {
        FDCAN_RxHeaderTypeDef rx_header;
        uint8_t len;
        while (HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFO0) > 0)
        {
            if (HAL_FDCAN_GetRxMessage(hfdcan,FDCAN_RX_FIFO0, &rx_header, CAN1_Manage_Object.Rx_Buffer.Data) == HAL_OK)
            {
                CAN1_Manage_Object.Rx_Buffer.Header.Identifier = rx_header.Identifier;
                if (rx_header.DataLength <= FDCAN_DLC_BYTES_8) len = rx_header.DataLength;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_12) len = 12;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_16) len = 16;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_20) len = 20;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_24) len = 24;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_32) len = 32;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_48) len = 48;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_64) len = 64;
                CAN1_Manage_Object.Callback_Function(&CAN1_Manage_Object.Rx_Buffer);
            }
        }
    }
    else if (hfdcan->Instance == FDCAN2)
    {
        while (HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFO0) > 0)
        {
            if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &CAN2_Manage_Object.Rx_Buffer.Header,
                                          CAN2_Manage_Object.Rx_Buffer.Data) == HAL_OK)
            {
                CAN2_Manage_Object.Callback_Function(&CAN2_Manage_Object.Rx_Buffer);
            }
        }
    }
    else if (hfdcan->Instance == FDCAN3)
    {
        FDCAN_RxHeaderTypeDef rx_header;
        uint8_t len;
        while (HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFO0) > 0)
        {
            if (HAL_FDCAN_GetRxMessage(hfdcan,FDCAN_RX_FIFO0, &rx_header, CAN3_Manage_Object.Rx_Buffer.Data) == HAL_OK)
            {
                CAN3_Manage_Object.Rx_Buffer.Header.Identifier = rx_header.Identifier;
                if (rx_header.DataLength <= FDCAN_DLC_BYTES_8) len = rx_header.DataLength;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_12) len = 12;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_16) len = 16;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_20) len = 20;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_24) len = 24;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_32) len = 32;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_48) len = 48;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_64) len = 64;

                CAN3_Manage_Object.Callback_Function(&CAN3_Manage_Object.Rx_Buffer);
            }
        }

        // HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &CAN3_Manage_Object.Rx_Buffer.Header,
        //                        CAN3_Manage_Object.Rx_Buffer.Data);
        // CAN3_Manage_Object.Callback_Function(&CAN3_Manage_Object.Rx_Buffer);
    }
}

/**
 * @brief HAL库CAN接收FIFO1中断
 *
 * @param hcan CAN编号
 */
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs)
{
    //选择回调函数
    if (hfdcan->Instance == FDCAN1)
    {
        FDCAN_RxHeaderTypeDef rx_header;
        uint8_t len;
        while (HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFO1) > 0)
        {
            if (HAL_FDCAN_GetRxMessage(hfdcan,FDCAN_RX_FIFO1, &rx_header, CAN1_Manage_Object.Rx_Buffer.Data) == HAL_OK)
            {
                CAN1_Manage_Object.Rx_Buffer.Header.Identifier = rx_header.Identifier;
                if (rx_header.DataLength <= FDCAN_DLC_BYTES_8) len = rx_header.DataLength;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_12) len = 12;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_16) len = 16;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_20) len = 20;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_24) len = 24;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_32) len = 32;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_48) len = 48;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_64) len = 64;
                CAN1_Manage_Object.Callback_Function(&CAN1_Manage_Object.Rx_Buffer);

            }
        }
    }
    else if (hfdcan->Instance == FDCAN2)
    {
        while (HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFO1) > 0)
        {
            HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &CAN2_Manage_Object.Rx_Buffer.Header,
                                   CAN2_Manage_Object.Rx_Buffer.Data);
            CAN2_Manage_Object.Callback_Function(&CAN2_Manage_Object.Rx_Buffer);
        }
    }
    else if (hfdcan->Instance == FDCAN3)
    {
        FDCAN_RxHeaderTypeDef rx_header;
        uint8_t len;
        while (HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFO1) > 0)
        {
            if (HAL_FDCAN_GetRxMessage(hfdcan,FDCAN_RX_FIFO1, &rx_header, CAN3_Manage_Object.Rx_Buffer.Data) == HAL_OK)
            {
                CAN3_Manage_Object.Rx_Buffer.Header.Identifier = rx_header.Identifier;
                if (rx_header.DataLength <= FDCAN_DLC_BYTES_8) len = rx_header.DataLength;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_12) len = 12;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_16) len = 16;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_20) len = 20;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_24) len = 24;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_32) len = 32;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_48) len = 48;
                else if (rx_header.DataLength == FDCAN_DLC_BYTES_64) len = 64;
                CAN3_Manage_Object.Callback_Function(&CAN3_Manage_Object.Rx_Buffer);
            }
        }

        // HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &CAN3_Manage_Object.Rx_Buffer.Header,
        //                        CAN3_Manage_Object.Rx_Buffer.Data);
        // CAN3_Manage_Object.Callback_Function(&CAN3_Manage_Object.Rx_Buffer);
    }
}


uint8_t CAN_Send_Data(FDCAN_HandleTypeDef *hfdcan, uint16_t ID, uint8_t *Data, uint16_t Length)
{
    FDCAN_TxHeaderTypeDef tx_header;

    tx_header.Identifier = ID; //TX_ID
    tx_header.IdType = FDCAN_STANDARD_ID; //标准ID
    tx_header.TxFrameType = FDCAN_DATA_FRAME; //数据帧

    if (Length <= 8)
    {
        tx_header.DataLength = Length;
    }
    else if (Length == 12)
    {
        tx_header.DataLength = FDCAN_DLC_BYTES_12;
    }
    else if (Length == 16)
    {
        tx_header.DataLength = FDCAN_DLC_BYTES_16;
    }
    else if (Length == 20)
    {
        tx_header.DataLength = FDCAN_DLC_BYTES_20;
    }
    else if (Length == 24)
    {
        tx_header.DataLength = FDCAN_DLC_BYTES_24;
    }
    else if (Length == 32)
    {
        tx_header.DataLength = FDCAN_DLC_BYTES_32;
    }
    else if (Length == 48)
    {
        tx_header.DataLength = FDCAN_DLC_BYTES_48;
    }
    else if (Length == 64)
    {
        tx_header.DataLength = FDCAN_DLC_BYTES_64;
    }

    tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    tx_header.BitRateSwitch = FDCAN_BRS_ON;
    tx_header.FDFormat = FDCAN_FD_CAN;
    tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    tx_header.MessageMarker = 0;

    int a = HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &tx_header, Data);
    return a;
}

/**
 * @brief CAN的TIM定时器中断发送回调函数
 *
 */
void TIM_CAN_PeriodElapsedCallback()
{
    static int mod = 0;

    mod++;

    // CAN1电机
    // CAN_Send_Data(&hfdcan1, 0x100, CAN1_0x100_Tx_Data, 8);


    // CAN2电机
    CAN_Send_Data(&hfdcan2, 0x200, CAN2_0x200_Tx_Data, 8);

    if (mod == 50)
    {
        CAN_Send_Data(&hfdcan2, 0x210, cap, 8);
        mod = 0;
    }
}