/**
 * @file can_comm.h
 * @author yuanluochen
 * @brief 多设备通信模块，主要用于控制板之间的通信，使用can总线实现，基于数组实现
 * @version 0.1
 * @date 2023-09-17
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef CAN_PACKEET_CONNECTION_H
#define CAN_PACKEET_CONNECTION_H

#include "CAN_receive.h"

//最大传输数据量 unit byte
#define CAN_COMM_MAX_BUFSIZE 60
//校验数据量 unit byte, 保存帧头帧尾 + + 数据长度位 + crc校验和
#define CAN_COMM_OFFSET_BUFSIZE 4

//帧头
#define CAN_COMM_HEADER 0X73
//帧尾
#define CAN_COMM_TAIL 0x65

//can设备单次发送最大值
#define CAN_COMM_SINGLE_TRANSMIT_MAX_SIZE 8
//can通信队列容量
#define CAN_COMM_QUEUE_CAPACITY 11

typedef enum
{
    No_Control = 0,
    Remote_Control,
    Key_Mouse_Control,
} robot_control_status_t;//机器人控制状态

typedef enum
{
    TRIGGER_NORMAL = 0,
    TRIGGER_GONGDAN,
    TRIGGER_HUIBO,
} trigger_status_t;

typedef struct __attribute__ ((packed))
{
    //遥控器控制数据
    uint16_t ch_1;
    uint16_t ch_2;
    uint16_t ch_3;
    uint16_t sw;
}Can_Remote_Rx_Data_t;

typedef struct __attribute__ ((packed))
{
    //遥控器控制数据
    uint16_t ch_1;
    uint16_t ch_2;
    uint16_t ch_3;
    uint16_t sw;
}Can_VT13_Rx_Data_t;

typedef struct __attribute__ ((packed))
{
    uint16_t fric_speed_set_average;
    uint16_t key;
    uint16_t flag;
    uint8_t trigger_status;
    uint8_t robot_move_flag :4;
    uint8_t robot_control_status :4;
}Can_Control_Rx_Data_t;

typedef struct __attribute__((packed))
{
    uint8_t left_fric_motor_status :4;
    uint8_t right_fric_motor_status :4;
    uint8_t yaw_motor_status;
    uint8_t pitch_motor_status;
    uint8_t trigger_motor_status;
    float pitch_angle;

}Can_Motor_Status_Rx_Data_t;


typedef struct __attribute__ ((packed))
{
    uint16_t robot_id;
    uint16_t shoot_heat;
    uint16_t shoot_speed;
    uint16_t shoot_heat_limit;
}Can_Referee_Tx_Data_t;

typedef struct
{
    // 绑定的CAN
    Struct_CAN_Manage_Object *Can_Manage_Object;
    // 接收ID
    uint16_t CAN_Rx_ID;
    // 发送ID
    uint16_t CAN_Tx_ID;
    // 发送缓存区
    uint8_t *Tx_Data;

    //对外接口
    Can_Remote_Rx_Data_t Rx_Data;
} Can_Remote_Data_t;

typedef struct
{
    // 绑定的CAN
    Struct_CAN_Manage_Object *Can_Manage_Object;
    // 接收ID
    uint16_t CAN_Rx_ID;
    // 发送ID
    uint16_t CAN_Tx_ID;
    // 发送缓存区
    uint8_t *Tx_Data;

    //对外接口

    //发送内容
    Can_Control_Rx_Data_t Rx_Data;
} Can_Control_Data_t;

typedef struct
{
    // 绑定的CAN
    Struct_CAN_Manage_Object *Can_Manage_Object;
    // 接收ID
    uint16_t CAN_Rx_ID;
    // 发送ID
    uint16_t CAN_Tx_ID;
    // 发送缓存区
    uint8_t *Tx_Data;

    //对外接口

    //发送内容
    Can_Motor_Status_Rx_Data_t Rx_Data;
} Can_Motor_Status_Data_t;


typedef struct
{
    // 绑定的CAN
    Struct_CAN_Manage_Object *Can_Manage_Object;
    // 接收ID
    uint16_t CAN_Rx_ID;
    // 发送ID
    uint16_t CAN_Tx_ID;
    // 发送缓存区
    uint8_t *Tx_Data;

    //对外接口

    //发送内容
    Can_VT13_Rx_Data_t Rx_Data;
} Can_VT13_Data_t;

typedef struct
{
    // 绑定的CAN
    Struct_CAN_Manage_Object *Can_Manage_Object;
    // 接收ID
    uint16_t CAN_Rx_ID;
    // 发送ID
    uint16_t CAN_Tx_ID;
    // 发送缓存区
    uint8_t *Tx_Data;

    //对外接口
    uint16_t robot_id;
    //发送内容
    Can_Referee_Tx_Data_t Can_Referee_Tx_Data;
} Can_Referee_Data_t;

void Can_Receive_Solution(Can_Remote_Data_t * can_comm, uint8_t * Rx_Data);
void Can_Control_Data_Solution(Can_Control_Data_t * can_comm, uint8_t * Rx_Data);
void Can_Motor_Status_Data_Solution(Can_Motor_Status_Data_t *can_comm, uint8_t *Rx_Data);
void Can_VT13_Data_Solution(Can_VT13_Data_t * can_comm, uint8_t * Rx_Data);
void Can_Referee_Data_Init(Can_Referee_Data_t *Can_Referee_Data, FDCAN_HandleTypeDef *hdfcan, uint16_t __CAN_Tx_ID);
void Can_Referee_Data_Out_Put(Can_Referee_Data_t *Can_Referee_Data);

#endif // !CAN_PACKEET_CONNECTION_H