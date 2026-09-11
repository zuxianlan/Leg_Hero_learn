#ifndef __CAP_H
#define __CAP_H
#include "main.h"

typedef struct __attribute__ ((packed))
{
    uint8_t err
    :
    4; //错误帧
    uint8_t status
    :
    3; //状态帧
    uint8_t flag
    :
    1; //使能标志；1：使能；0：失能
    float cap_volt; //电容电压
    float chassis_power; //底盘功率
    float cap_target_power; //电容设定功率（裁判功率上限）
}

cap_rx_data_t; //接收电容返回数据

typedef struct __attribute__ ((packed))
{
    uint16_t cap_set_power; //电容设定功率（裁判功率上限）
    uint8_t cap_flag
    :
    1; //电容设定功率（裁判功率上限）
}

cap_tx_data_t; //发送电容控制数据

void CAP_GetRxPacket(cap_rx_data_t * rx, uint8_t * rx_data);
void CAP_AddTxPacket(cap_tx_data_t * tx, uint8_t * tx_data);
#endif //__CAP_H