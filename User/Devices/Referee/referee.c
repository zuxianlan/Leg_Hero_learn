#include "referee.h"
#include "string.h"
#include "stdio.h"
#include "CRC8_CRC16.h"
#include "protocol.h"


frame_header_struct_t referee_receive_header;
frame_header_struct_t referee_send_header;

ext_game_state_t game_state; // 0001
ext_game_result_t game_result; // 0002
ext_game_robot_HP_t game_robot_HP; // 0003

ext_event_data_t event_data; // 0x0101
ext_referee_warning_t referee_warning; // 0x0104
ext_dart_info_t dart_info; // 0x0105

ext_robot_state_t robot_state; // 0x0201
ext_power_heat_data_t power_heat_data; // 0x0202
ext_robot_pos_t robot_pos; // 0x0203
ext_buff_t buff; // 0x0204
ext_hurt_data_t hurt_data; // 0x0206
ext_shoot_data_t shoot_data; // 0x0207
ext_projectile_allowance_t projectile_allowance; // 0x0208
ext_rfid_status_t rfid_status; // 0x0209
ground_robot_position_t groundRobotPosition; // 0x020B
sentry_info_t sentryInfo; // 0x020D

robot_interaction_data_t robotInteractionData; // 0x0301
ext_robot_command_t robot_command_t; // 0x0303

sentry_info_t sentry_info;

sentry_cmd_t sentry_cmd;

void init_referee_struct_data(void)
{
    memset(&referee_receive_header, 0, sizeof(frame_header_struct_t));
    memset(&referee_send_header, 0, sizeof(frame_header_struct_t));

    memset(&game_state, 0, sizeof(ext_game_state_t));
    memset(&game_result, 0, sizeof(ext_game_result_t));
    memset(&game_robot_HP, 0, sizeof(ext_game_robot_HP_t));

    memset(&event_data, 0, sizeof(ext_event_data_t));

    memset(&referee_warning, 0, sizeof(ext_referee_warning_t));
    memset(&dart_info, 0, sizeof(ext_dart_info_t));

    memset(&robot_state, 0, sizeof(ext_robot_state_t));
    memset(&power_heat_data, 0, sizeof(ext_power_heat_data_t));
    memset(&robot_pos, 0, sizeof(ext_robot_pos_t));
    memset(&buff, 0, sizeof(ext_buff_t));

    memset(&hurt_data, 0, sizeof(ext_hurt_data_t));
    memset(&shoot_data, 0, sizeof(ext_shoot_data_t));
    memset(&projectile_allowance, 0, sizeof(ext_projectile_allowance_t));


    memset(&robot_command_t, 0, sizeof(ext_robot_command_t));
}

//获取机器人id
uint8_t get_robot_id(void)
{
    return robot_state.robot_id;
}

// 获取 42mm 发射机构的射击热量
void get_shoot_heat_limit_and_heat(uint16_t *heat_limit, uint16_t *heat)
{
    *heat = power_heat_data.shooter_42mm_heat;
}

//获取机器人状态
ext_robot_state_t *get_robot_status_point(void)
{
    return &robot_state;
}

//获取机器人伤害类型指针
ext_hurt_data_t *get_hurt_data_point(void)
{
    return &hurt_data;
}

//获取机器人发射机构弹速信息
ext_shoot_data_t *get_shoot_data_point(void)
{
    return &shoot_data;
}

//获取机器人命令数据指针
ext_robot_command_t *get_robot_command_point(void)
{
    return &robot_command_t;
}

//获取场地状态指针
ext_event_data_t *get_field_event_point(void)
{
    return &event_data;
}

//获取比赛机器血量指针
ext_game_robot_HP_t *get_game_robot_HP_point(void)
{
    return &game_robot_HP;
}

//获取机器人位置指针
ext_robot_pos_t *get_robot_pos_point(void)
{
    return &robot_pos;
}