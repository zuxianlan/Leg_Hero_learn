#include "super_cap.h"


/**
 * 从接收到的数据中解析出CAP数据包
 * @param rx 指向接收数据结构体的指针
 * @param rx_data 接收到的数据
 */
void CAP_GetRxPacket(cap_rx_data_t *rx, uint8_t *rx_data)
{
    if (rx == NULL || rx_data == NULL)
    {
        return;
    }
    rx->err = (rx_data[0] >> 4) & 0x0F; // 解析错误码
    rx->status = (rx_data[0] >> 1) & 0x07; // 解析状态
    rx->flag = rx_data[0] & 0x01; // 解析标志位
    rx->cap_volt = (float) (rx_data[1] << 8 | rx_data[2]) / 100; // 解析电容电压
    rx->chassis_power = (float) (rx_data[3] << 8 | rx_data[4]) / 100; // 解析底盘功率
    rx->cap_target_power = (float) (rx_data[5] << 8 | rx_data[6]) / 100; // 解析目标电容功率
}


/**
 * 将CAP数据包添加到发送数据中
 * @param tx 指向发送数据结构体的指针
 * @param tx_data 发送的数据
 */
void CAP_AddTxPacket(cap_tx_data_t *tx, uint8_t *tx_data)
{
    if (tx == NULL || tx_data == NULL)
    {
        return;
    }
    tx_data[0] = tx->cap_set_power >> 8; // 设置电容功率高位
    tx_data[1] = tx->cap_set_power; // 设置电容功率低位
    tx_data[2] = tx->cap_flag; // 设置电容标志位
    /****未使用****/
    // tx_data[4] =
    // tx_data[5] =
    // tx_data[6] =
    // tx_data[7] =
}