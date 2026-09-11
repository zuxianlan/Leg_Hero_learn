// Copyright 2025 SMBU-PolarBear-Robotics-Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef NAVIGATION_TASK_PACKET_TYPEDEF_H
#define NAVIGATION_TASK_PACKET_TYPEDEF_H

#include "struct_typedef.h"

typedef enum
{
    SOF_RECEIVE = 0x5A,
    SOF_SEND = 0x5A
} navigation_sof_e;

#define SOF_POSITION 0

typedef enum
{
    ID_DEBUG = 0x01,
    ID_IMU = 0x02,
    ID_ROBOT_STATE_INFO = 0x03,
    ID_EVENT_DATA = 0x04,
    ID_PID_DEBUG = 0x05,
    ID_ALL_ROBOT_HP = 0x06,
    ID_GAME_STATUS = 0x07,
    ID_ROBOT_MOTION = 0x08,
    ID_GROUND_ROBOT_POSITION = 0x09,
    ID_RFID_STATUS = 0x0A,
    ID_ROBOT_STATUS = 0x0B,
    ID_JOINT_STATE = 0x0C,
} navigation_send_ID_e;

typedef enum
{
    ID_ROBOT_CMD = 0x01
} navigation_receive_ID_e;


#define DEBUG_PACKAGE_NUM 10
#define DEBUG_PACKAGE_NAME_LEN 10

typedef struct __attribute__ ((packed))
{
    uint8_t sof; // 数据帧起始字节，固定值为 0x5A
    uint8_t len; // 数据段长度
    uint8_t id; // 数据段id
    uint8_t crc; // 数据帧头的 CRC8 校验
}

HeaderFrame;

/**
 * @brief send data
 * 
 */

// 串口调试数据包
typedef struct __attribute__ ((packed))
{
    HeaderFrame frame_header;
    uint32_t time_stamp;
    struct __attribute__ ((packed))
    {
        uint8_t name[DEBUG_PACKAGE_NAME_LEN];
        uint8_t type;
        float data;
    }
    packages[DEBUG_PACKAGE_NUM];

    uint16_t checksum;
}

SendDebugData;

// IMU 数据包
typedef struct __attribute__ ((packed))
{
    HeaderFrame frame_header;
    uint32_t time_stamp;

    struct __attribute__ ((packed))
    {
        float yaw; // rad
        float pitch; // rad
        float roll; // rad

        float yaw_vel; // rad/s
        float pitch_vel; // rad/s
        float roll_vel; // rad/s

        // float x_accel;  // m/s^2
        // float y_accel;  // m/s^2
        // float z_accel;  // m/s^2
    }
    data;

    uint16_t crc;
}

SendImuData;

// 机器人信息数据包
typedef struct __attribute__ ((packed))
{
    HeaderFrame frame_header;

    uint32_t time_stamp;

    struct __attribute__ ((packed))
    {
        /// @brief 机器人部位类型 2 bytes
        struct __attribute__ ((packed))
        {
            uint16_t chassis
            :
            3;
            uint16_t gimbal
            :
            3;
            uint16_t shoot
            :
            3;
            uint16_t arm
            :
            3;
            uint16_t custom_controller
            :
            3;
            uint16_t reserve
            :
            1;
        }
        type;

        /// @brief 机器人部位状态 1 byte
    /// @note 0: 错误，1: 正常
        struct __attribute__ ((packed))
        {
            uint8_t chassis
            :
            1;
            uint8_t gimbal
            :
            1;
            uint8_t shoot
            :
            1;
            uint8_t arm
            :
            1;
            uint8_t custom_controller
            :
            1;
            uint8_t reserve
            :
            3;
        }
        state;
    }
    data;

    uint16_t crc;
}

SendRobotInfoData;

// 事件数据包
typedef struct __attribute__ ((packed))
{
    HeaderFrame frame_header;
    uint32_t time_stamp;
    struct __attribute__ ((packed))
    {
        HeaderFrame frame_header;
        uint32_t time_stamp;

        uint8_t supply_station_front;
        uint8_t supply_station_internal;
        uint8_t supply_zone;
        uint8_t center_gain_zone;

        uint8_t small_energy;
        uint8_t big_energy;

        uint8_t circular_highland;
        uint8_t trapezoidal_highland_3;
        uint8_t trapezoidal_highland_4;

        uint8_t base_virtual_shield_remaining;
    }
    data;
    uint16_t crc;
}

SendEventData;

// PID调参数据包
typedef struct __attribute__ ((packed))
{
    HeaderFrame frame_header;
    uint32_t time_stamp;
    struct __attribute__ ((packed))
    {
        float fdb;
        float ref;
        float pid_out;
    }
    data;

    uint16_t crc;
}

SendPidDebugData;

// 全场机器人hp信息数据包
typedef struct __attribute__ ((packed))
{
    HeaderFrame frame_header;

    uint32_t time_stamp;

    struct __attribute__ ((packed))
    {
        uint16_t red_1_robot_hp;
        uint16_t red_2_robot_hp;
        uint16_t red_3_robot_hp;
        uint16_t red_4_robot_hp;
        uint16_t red_5_robot_hp;
        uint16_t red_7_robot_hp;
        uint16_t red_outpost_hp;
        uint16_t red_base_hp;
        uint16_t blue_1_robot_hp;
        uint16_t blue_2_robot_hp;
        uint16_t blue_3_robot_hp;
        uint16_t blue_4_robot_hp;
        uint16_t blue_5_robot_hp;
        uint16_t blue_7_robot_hp;
        uint16_t blue_outpost_hp;
        uint16_t blue_base_hp;
    }
    data;

    uint16_t crc;
}

SendAllRobotHpData;

// 比赛信息数据包
// struct __attribute__((packed))
// {
//   HeaderFrame frame_header;
//   uint32_t time_stamp;
//
//   struct __attribute__((packed))
//   {
//     uint8_t game_progress;
//     uint16_t stage_remain_time;
//   } data;
//
//   uint16_t crc;
// }SendGameStatusData;

// 机器人运动数据包
typedef struct __attribute__ ((packed))
{
    HeaderFrame frame_header;
    uint32_t time_stamp;

    struct __attribute__ ((packed))
    {
        struct __attribute__ ((packed))
        {
            float vx;
            float vy;
            float wz;
        }
        speed_vector;
    }
    data;
    uint16_t crc;
}

SendRobotMotionData;

// 地面机器人位置数据包
typedef struct __attribute__ ((packed))
{
    HeaderFrame frame_header;
    uint32_t time_stamp;
    struct __attribute__ ((packed))
    {
        float hero_x;
        float hero_y;

        float engineer_x;
        float engineer_y;

        float standard_3_x;
        float standard_3_y;

        float standard_4_x;
        float standard_4_y;

        float standard_5_x;
        float standard_5_y;
    }
    data;
    uint16_t crc;
}

SendGroundRobotPosition;

// RFID 状态数据包
typedef struct __attribute__ ((packed))
{
    HeaderFrame frame_header;
    uint32_t time_stamp;

    struct __attribute__ ((packed))
    {
        bool base_gain_point; // 己方基地增益点
        bool circular_highland_gain_point; // 己方环形高地增益点
        bool enemy_circular_highland_gain_point; // 对方环形高地增益点
        bool friendly_r3_b3_gain_point; // 己方 R3/B3 梯形高地增益点
        bool enemy_r3_b3_gain_point; // 对方 R3/B3 梯形高地增益点
        bool friendly_r4_b4_gain_point; // 己方 R4/B4 梯形高地增益点
        bool enemy_r4_b4_gain_point; // 对方 R4/B4 梯形高地增益点
        bool energy_mechanism_gain_point; // 己方能量机关激活点
        bool friendly_fly_ramp_front_gain_point; // 己方飞坡增益点（靠近己方一侧飞坡前）
        bool friendly_fly_ramp_back_gain_point; // 己方飞坡增益点（靠近己方一侧飞坡后）
        bool enemy_fly_ramp_front_gain_point; // 对方飞坡增益点（靠近对方一侧飞坡前）
        bool enemy_fly_ramp_back_gain_point; // 对方飞坡增益点（靠近对方一侧飞坡后）
        bool friendly_outpost_gain_point; // 己方前哨站增益点
        bool friendly_healing_point; // 己方补血点（检测到任一均视为激活）
        bool friendly_sentry_patrol_area; // 己方哨兵巡逻区
        bool enemy_sentry_patrol_area; // 对方哨兵巡逻区
        bool friendly_big_resource_island; // 己方大资源岛增益点
        bool enemy_big_resource_island; // 对方大资源岛增益点
        bool friendly_exchange_area; // 己方兑换区
        bool center_gain_point; // 中心增益点 RFID 卡状态（仅 RMUL 适用），1
        // 为已检测到
    }
    data;
    uint16_t crc;
}

SendRfidStatus;

// 机器人状态数据包
typedef struct __attribute__ ((packed))
{
    HeaderFrame frame_header;
    uint32_t time_stamp;
    struct __attribute__ ((packed))
    {
        uint8_t robot_id;
        uint8_t robot_level;
        uint16_t current_up;
        uint16_t maximum_hp;
        uint16_t shooter_barrel_cooling_value;
        uint16_t shooter_barrel_heat_limit;

        uint16_t shooter_17mm_1_barrel_heat;

        float robot_pos_x;
        float robot_pos_y;
        float robot_pos_angle;

        uint8_t armor_id;
        uint8_t hp_deduction_reason;

        uint16_t projectile_allowance_17mm_1;
        uint16_t remaining_gold_coin;
    }
    data;
    uint16_t crc;
}

SendRobotStatus;

// 云台状态数据包
typedef struct __attribute__ ((packed))
{
    HeaderFrame frame_header;
    uint32_t time_stamp;
    struct __attribute__ ((packed))
    {
        float pitch;
        float yaw;
    }
    data;
    uint16_t crc;
}

SendJointState;

/**
 * @brief navigation task receive data to computer
 * 
 */

typedef struct __attribute__ ((packed))
{
    HeaderFrame frame_header;

    uint32_t time_stamp;

    struct __attribute__ ((packed))
    {
        struct __attribute__ ((packed))
        {
            float vx;
            float vy;
            float wz;
        }
        speed_vector;

        struct __attribute__ ((packed))
        {
            float roll;
            float pitch;
            float yaw;
            float leg_lenth;
        }
        chassis;

        struct __attribute__ ((packed))
        {
            float pitch;
            float yaw;
        }
        gimbal;

        struct __attribute__ ((packed))
        {
            uint8_t fire;
            uint8_t fric_on;
        }
        shoot;
    }
    data;

    uint16_t checksum;
}

ReceiveRobotCmdData;


#endif  // STANDARD_ROBOT_PP_ROS2__PACKET_TYPEDEF_HPP_