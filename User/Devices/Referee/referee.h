#ifndef REFEREE_H
#define REFEREE_H

#include "main.h"
#include "protocol.h"
// 2025赛季
// 命令码枚举定义（对应表2-1）
typedef enum
{
    Referee_Command_ID_GAME_STATUS = 0x0001, // 0x0001 比赛状态数据
    Referee_Command_ID_GAME_RESULT, // 0x0002 比赛结果数据
    Referee_Command_ID_GAME_ROBOT_HP, // 0x0003 机器人血量数据
    Referee_Command_ID_EVENT_SELF_DATA = 0x0101, // 0x0101 场地事件数据
    Referee_Command_ID_EVENT_SELF_REFEREE_WARNING = 0x0104, // 0x0104 裁判警告数据
    Referee_Command_ID_EVENT_SELF_DART_STATUS = 0x0105, // 2025新增 飞镖发射数据
    Referee_Command_ID_ROBOT_STATUS = 0x0201, // 0x0201 机器人性能数据
    Referee_Command_ID_ROBOT_POWER_HEAT, // 0x0202 底盘能量数据
    Referee_Command_ID_ROBOT_POSITION, // 0x0203 机器人位置数据
    Referee_Command_ID_ROBOT_BUFF, // 0x0204 增益状态数据
    Referee_Command_ID_ROBOT_DAMAGE = 0x0206, // 0x0206 伤害数据
    Referee_Command_ID_ROBOT_BOOSTER, // 0x0207 实时射击数据
    Referee_Command_ID_ROBOT_REMAINING_AMMO, // 0x0208 允许发弹量
    Referee_Command_ID_ROBOT_RFID, // 0x0209 RFID状态数据
    Referee_Command_ID_ROBOT_DART_COMMAND = 0x020A, // 0x020A 飞镖选手端指令
    Referee_Command_ID_ROBOT_SENTRY_LOCATION, // 0x020B 地面机器人位置
    Referee_Command_ID_ROBOT_RADAR_MARK, // 0x020C 雷达标记进度
    Referee_Command_ID_ROBOT_SENTRY_DECISION, // 0x020D 哨兵自主决策
    Referee_Command_ID_ROBOT_RADAR_DECISION, // 0x020E 雷达自主决策
    Referee_Command_ID_INTERACTION = 0x0301, // 0x0301 机器人交互数据
    Referee_Command_ID_INTERACTION_ROBOT_RECEIVE_CUSTOM_CONTROLLER, // 0x0302 自定义控制器交互
    Referee_Command_ID_INTERACTION_ROBOT_RECEIVE_CLIENT_MINIMAP, // 0x0303 小地图交互
    Referee_Command_ID_INTERACTION_ROBOT_RECEIVE_CLIENT_REMOTE_CONTROL, // 0x0304 键鼠遥控
    Referee_Command_ID_INTERACTION_CLIENT_RECEIVE_RADAR, // 0x0305 雷达数据接收
    Referee_Command_ID_INTERACTION_CLIENT_RECEIVE_CUSTOM_CONTROLLER, // 0x0306 自定义控制器接收
    Referee_Command_ID_INTERACTION_CLIENT_RECEIVE_SENTRY_SEMIAUTOMATIC_MINIMAP, // 0x0307 哨兵半自动小地图
    Referee_Command_ID_INTERACTION_CLIENT_RECEIVE_ROBOT_MINIMAP, // 0x0308 机器人小地图
} Enum_Referee_Command_ID;

/**
 * @brief 通用双方机器人ID
 *
 */
typedef enum
{
    Referee_Data_Robots_ID_NO = 0,
    Referee_Data_Robots_ID_RED_HERO_1,
    Referee_Data_Robots_ID_RED_ENGINEER_2,
    Referee_Data_Robots_ID_RED_INFANTRY_3,
    Referee_Data_Robots_ID_RED_INFANTRY_4,
    Referee_Data_Robots_ID_RED_INFANTRY_5,
    Referee_Data_Robots_ID_RED_AERIAL_6,
    Referee_Data_Robots_ID_RED_SENTRY_7,
    Referee_Data_Robots_ID_RED_DART_8,
    Referee_Data_Robots_ID_RED_RADAR_9,
    Referee_Data_Robots_ID_RED_BASE_10,
    Referee_Data_Robots_ID_RED_OUTPOST_11,
    Referee_Data_Robots_ID_BLUE_HERO_1 = 101,
    Referee_Data_Robots_ID_BLUE_ENGINEER_2,
    Referee_Data_Robots_ID_BLUE_INFANTRY_3,
    Referee_Data_Robots_ID_BLUE_INFANTRY_4,
    Referee_Data_Robots_ID_BLUE_INFANTRY_5,
    Referee_Data_Robots_ID_BLUE_AERIAL_6,
    Referee_Data_Robots_ID_BLUE_SENTRY_7,
    Referee_Data_Robots_ID_BLUE_DART_8,
    Referee_Data_Robots_ID_BLUE_RADAR_9,
    Referee_Data_Robots_ID_BLUE_BASE_10,
    Referee_Data_Robots_ID_BLUE_OUTPOST_11,
} Enum_Referee_Data_Robots_ID;

/**
 * @brief 通用双方机器人ID
 *
 */
typedef enum
{
    Referee_Data_Robots_Client_ID_NO = 0,
    Referee_Data_Robots_Client_ID_RED_HERO_1 = 0x0101,
    Referee_Data_Robots_Client_ID_RED_ENGINEER_2,
    Referee_Data_Robots_Client_ID_RED_INFANTRY_3,
    Referee_Data_Robots_Client_ID_RED_INFANTRY_4,
    Referee_Data_Robots_Client_ID_RED_INFANTRY_5,
    Referee_Data_Robots_Client_ID_RED_AERIAL_6,
    Referee_Data_Robots_Client_ID_BLUE_HERO_1 = 0x0165,
    Referee_Data_Robots_Client_ID_BLUE_ENGINEER_2,
    Referee_Data_Robots_Client_ID_BLUE_INFANTRY_3,
    Referee_Data_Robots_Client_ID_BLUE_INFANTRY_4,
    Referee_Data_Robots_Client_ID_BLUE_INFANTRY_5,
    Referee_Data_Robots_Client_ID_BLUE_AERIAL_6,
    Referee_Data_Robots_Server = 0x8080,
} Enum_Referee_Data_Robots_Client_ID;

typedef enum
{
    PROGRESS_UNSTART = 0,
    PROGRESS_PREPARE = 1,
    PROGRESS_SELFCHECK = 2,
    PROGRESS_5sCOUNTDOWN = 3,
    PROGRESS_BATTLE = 4,
    PROGRESS_CALCULATING = 5,
} game_progress_t;

typedef enum
{
    Referee_Interaction_Command_ID_UI_LAYER_DELETE = 0x0100,
    Referee_Interaction_Command_ID_UI_GRAPHIC_1,
    Referee_Interaction_Command_ID_UI_GRAPHIC_2,
    Referee_Interaction_Command_ID_UI_GRAPHIC_5,
    Referee_Interaction_Command_ID_UI_GRAPHIC_7,
    Referee_Interaction_Command_ID_UI_GRAPHIC_STRING = 0x0110,
    Referee_Interaction_Command_ID_SENTRY = 0x0120,
    Referee_Interaction_Command_ID_RADAR = 0x0121,
} Enum_Referee_Interaction_Command_ID;

typedef enum
{
    Referee_Data_Interaction_Layer_Delete_Operation_NULL = 0,
    Referee_Data_Interaction_Layer_Delete_Operation_CLEAR_ONE,
    Referee_Data_Interaction_Layer_Delete_Operation_CLEAR_ALL,
} Enum_Referee_Data_Interaction_Layer_Delete_Operation;

typedef enum
{
    ARMOR_HURT = 0x00, // 装甲板伤害
} hurt_type_t; // 伤害类型

// 操作类型
typedef enum
{
    Referee_Data_Interaction_Graphic_Operation_NULL = 0,
    Referee_Data_Interaction_Graphic_Operation_ADD,
    Referee_Data_Interaction_Graphic_Operation_CHANGE,
    Referee_Data_Interaction_Graphic_Operation_DELETE,
} Enum_Referee_Data_Interaction_Graphic_Operation;

// 操作图形
typedef enum
{
    Referee_Data_Interaction_Graphic_Type_LINE = 0,
    Referee_Data_Interaction_Graphic_Type_RECTANGLE,
    Referee_Data_Interaction_Graphic_Type_CIRCLE,
    Referee_Data_Interaction_Graphic_Type_OVAL,
    Referee_Data_Interaction_Graphic_Type_ARC,
    Referee_Data_Interaction_Graphic_Type_FLOAT,
    Referee_Data_Interaction_Graphic_Type_INTEGER,
    Referee_Data_Interaction_Graphic_Type_STRING,
} Enum_Referee_Data_Interaction_Graphic_Type;

// 颜色
typedef enum
{
    Referee_Data_Interaction_Graphic_Color_MAIN = 0,
    Referee_Data_Interaction_Graphic_Color_YELLOW,
    Referee_Data_Interaction_Graphic_Color_GREEN,
    Referee_Data_Interaction_Graphic_Color_ORANGE,
    Referee_Data_Interaction_Graphic_Color_PURPLE,
    Referee_Data_Interaction_Graphic_Color_PINK,
    Referee_Data_Interaction_Graphic_Color_CYAN,
    Referee_Data_Interaction_Graphic_Color_BLACK,
    Referee_Data_Interaction_Graphic_Color_WHITE,
} Enum_Referee_Data_Interaction_Graphic_Color;

// 基础赛事数据结构体（0x0000-0x00FF）
typedef struct __attribute__((packed)) // 0x0001 比赛状态数据
{
    uint8_t game_type : 4; // 比赛类型（位0-3）
    // 1: 超级对抗赛 2: 高校单项赛 3: AI挑战赛 4: 3V3对抗 5: 步兵对抗
    uint8_t game_progress : 4; // 比赛阶段（位4-7）
    // 0: 未开始 1: 准备 2: 15秒自检 3: 5秒倒计时 4: 比赛 5: 结算
    uint16_t stage_remain_time; // 当前阶段剩余时间（秒）
    uint64_t sync_timestamp; // NTP同步时间戳（UNIX时间）
} ext_game_state_t;

typedef struct __attribute__((packed)) // 0x0002 比赛结果数据
{
    uint8_t winner; // 胜利方标识
    // 0: 平局 1: 红方胜 2: 蓝方胜
} ext_game_result_t;

typedef struct __attribute__((packed)) // 0x0003 机器人血量数据
{
    uint16_t ally_1_robot_HP;
    uint16_t ally_2_robot_HP;
    uint16_t ally_3_robot_HP;
    uint16_t ally_4_robot_HP;
    int16_t damage_difference;
    uint16_t ally_7_robot_HP;
    uint16_t ally_outpost_HP;
    uint16_t ally_base_HP;
    uint16_t enemy_outpost_HP;
    uint16_t enemy_base_HP;
} ext_game_robot_HP_t;

// 场地事件数据（0x0100-0x01FF）
typedef struct __attribute__((packed)) // 0x0101 场地事件数据
{
    uint32_t event_data; // 事件状态位
    // bit0-2: 补给区占领状态（0:未占领 1:已占领）
    // bit3-5: 能量机关激活状态（0:未激活 1:已激活）
    // bit6-7: 中央高地占领状态（1:己方 2:敌方）
    // bit21-22: 中心增益点占领状态（0:未占领 1:己方 2:敌方 3:双方）
} ext_event_data_t;

typedef struct __attribute__((packed)) // 0x0104 裁判警告数据
{
    uint8_t level; // 判罚等级（1:黄牌 2:红牌 3:判负）
    uint8_t offending_robot_id; // 违规机器人ID
    uint8_t count; // 违规次数统计
} ext_referee_warning_t;

typedef struct __attribute__((packed)) // 2025新增 0x0105 飞镖发射数据
{
    uint8_t dart_remaining_time; // 剩余发射时间（秒）
    uint16_t dart_info; // 发射状态信息
    // bit0-2: 最近击中目标（0:未命中 1:前哨站 2:基地固定目标...）
    // bit6-7: 当前选定目标（0:前哨站 1:基地固定目标...）
} ext_dart_info_t;

// 机器人状态数据（0x0200-0x02FF）
typedef struct __attribute__((packed)) // 0x0201 机器人性能数据
{
    uint8_t robot_id;              // 机器人唯一ID
    uint8_t robot_level;           // 机器人等级（1-3级）
    uint16_t current_HP;           // 当前血量
    uint16_t maximum_HP;           // 最大血量
    uint16_t shooter_cooling_value;// 射击冷却值
    uint16_t shooter_heat_limit;   // 射击热量上限
    uint16_t chassis_power_limit;  // 底盘功率上限
    float bullet_speed_limit;      // 弹速上限
    uint8_t gimbal_output : 1;     // 云台供电状态（0:关闭 1:24V）
    uint8_t chassis_output : 1;    // 底盘供电状态
    uint8_t shooter_output : 1;    // 发射机构供电状态
} ext_robot_state_t;

typedef struct __attribute__((packed)) // 0x0202 底盘能量数据
{
    uint16_t reserved;
    uint16_t reserved_2;
    float reserved_3;
    uint16_t buffer_energy; // 底盘缓冲能量（焦耳）
    uint16_t shooter_17mm_heat; // 第1个17mm发射机构热量
    uint16_t shooter_42mm_heat; // 42mm发射机构热量
} ext_power_heat_data_t;

typedef struct __attribute__((packed)) // 0x0203 机器人位置数据
{
    float x; // X坐标（米）
    float y; // Y坐标（米）
    float angle; // 朝向角度（正北0度）
} ext_robot_pos_t;

typedef struct __attribute__((packed)) // 0x0204 增益状态数据
{
    uint8_t recovery_buff; // 回血增益（%）bit0-4:能量阈值指示
    uint8_t cooling_buff; // 冷却倍率（直接值）
    uint8_t defence_buff; // 防御增益（%）
    uint8_t vulnerability_buff; // 负防御增益（%）
    uint16_t attack_buff; // 攻击增益（%）
    uint8_t remaining_energy; // 剩余能量
} ext_buff_t;

typedef struct __attribute__((packed)) // 0x0206 伤害数据
{
    uint8_t armor_id : 4; // 受损装甲模块ID（0-15）
    uint8_t damage_type : 4; // 伤害类型（0:装甲攻击 1:模块掉线等）
} ext_hurt_data_t;

typedef struct __attribute__((packed)) // 0x0207 实时射击数据
{
    uint8_t bullet_type; // 弹丸类型（1:17mm 2:42mm）
    uint8_t shooter_id; // 发射机构ID（1-3）
    uint8_t fire_rate; // 射击频率（Hz）
    float initial_speed; // 初始速度（m/s）
} ext_shoot_data_t;

typedef struct __attribute__((packed)) // 0x0208 允许发弹量（2025新增）
{
    uint16_t projectile_allowance_17mm;
    uint16_t projectile_allowance_42mm;
    uint16_t remaining_gold_coin;
    uint16_t projectile_allowance_fortress;
} ext_projectile_allowance_t;

typedef struct __attribute__((packed)) // 0x0209 RFID状态数据
{
    uint32_t rfid_status;
    uint8_t rfid_status_2;
} ext_rfid_status_t;

typedef struct __attribute__((packed)) // 0x020A 飞镖选手端指令
{
    uint8_t launch_status; // 发射状态（0:关闭 1:开启中 2:已开启）
    uint8_t reserved;
    uint16_t target_change_time; // 目标切换剩余时间（秒）
    uint16_t latest_launch_time; // 最近发射指令时间（秒）
} ext_dart_command_t;

typedef struct __attribute__((packed)) // 0x020B 地面机器人位置
{
    float hero_x; // 英雄机器人X坐标
    float hero_y; // 英雄机器人Y坐标
    float engineer_x; // 工程机器人X坐标
    float engineer_y; // 工程机器人Y坐标
    float standard_3_x; // 3号步兵机器人X坐标
    float standard_3_y; // 3号步兵机器人Y坐标
    float standard_4_x; // 4号步兵机器人X坐标
    float standard_4_y; // 4号步兵机器人Y坐标
    float reserved; // 保留字段
    float reserved_2;
} ground_robot_position_t;

typedef struct __attribute__((packed)) // 0x020C 雷达标记进度
{
    uint8_t mark_progress; // 雷达标记进度（0-100%）
} ext_radar_mark_t;

typedef struct __attribute__((packed)) // 0x020D 哨兵自主决策
{
    uint32_t sentry_info;
    uint16_t sentry_info_2;
    uint64_t sentry_info_3;
} sentry_info_t;

typedef struct __attribute__((packed)) // 0x020E 雷达自主决策
{
    uint8_t radar_info; // 雷达决策信息
    // bit0-1: 双倍易伤机会（0-2）
    // bit2: 是否触发双倍易伤
} radar_decision_t;

// 交互控制指令（0x0300-0x03FF）
typedef struct __attribute__((packed)) // 0x0301 通用交互数据
{
    uint16_t data_cmd_id; // 子内容ID（0x0100-0x02FF）
    uint16_t sender_id; // 发送方机器人ID
    uint16_t receiver_id; // 接收方机器人ID
    uint8_t user_data[112]; // 用户数据段（最大112字节）
} robot_interaction_data_t;

typedef struct __attribute__((packed)) // 0x0302 自定义控制器交互
{
    // 控制器专属交互数据
    uint8_t custom_data[30];
} custom_controller_data_t;

typedef struct __attribute__((packed)) // 0x0303 小地图交互数据
{
    float target_position_x;
    float target_position_y;
    uint8_t commd_keyboard;
    uint8_t target_robot_ID;
    uint16_t cmd_source;
} ext_robot_command_t;

typedef struct __attribute__((packed)) // 0x0304 键鼠遥控数据
{
    int16_t mouse_x;
    int16_t mouse_y;
    int16_t mouse_Z;
    int8_t left_button_down;
    int8_t right_button_down;
    uint16_t keyboard_value;
    uint16_t reserved;
} remote_control_t;

// 其他交互数据结构体（0x0305-0x0308）
typedef struct __attribute__((packed)) // 0x0305 雷达数据接收
{
    uint16_t hero_position_x;
    uint16_t hero_position_y;
    uint16_t engineer_position_x;
    uint16_t engineer_position_y;
    uint16_t infantry_3_position_x;
    uint16_t infantry_3_position_y;
    uint16_t infantry_4_position_x;
    uint16_t infantry_4_position_y;
    uint16_t infantry_5_position_x;
    uint16_t infantry_5_position_y;
    uint16_t sentry_position_x;
    uint16_t sentry_position_y;
} radar_data_t;

typedef struct __attribute__((packed)) // 0x0306 自定义控制器接收
{
    uint16_t key_value;
    uint16_t x_position : 12;
    uint16_t mouse_left : 4;
    uint16_t y_position : 12;
    uint16_t mouse_right : 4;
    uint16_t reserved;
} custom_client_data_t;

typedef struct __attribute__((packed)) // 0x0307 哨兵半自动小地图
{
    uint8_t intention;
    uint16_t start_position_x;
    uint16_t start_position_y;
    int8_t delta_x[49];
    int8_t delta_y[49];
    uint16_t sender_id;
} map_data_t;

typedef struct __attribute__((packed)) // 0x0308 机器人小地图
{
    uint16_t sender_id;
    uint16_t receiver_id;
    uint8_t user_data[30];
} custom_info_t;

typedef struct __attribute__((packed)) // 子id 0x0120哨兵自主决策信息
{
    uint16_t Header;
    uint16_t Sender;
    uint16_t Receiver;
    uint32_t Confirm_Respawn_Status : 1;
    uint32_t Confirm_Exchange_Respawn_Status : 1;
    uint32_t Request_Exchange_Ammo_Number : 11;
    uint32_t Request_Exchange_Ammo_Time : 4;
    uint32_t Request_Exchange_HP_Time : 4;
    uint32_t Reserved : 11;
    uint16_t CRC_16;
} sentry_cmd_t;

typedef struct __attribute__((packed)) // 图形配置结构体
{
    uint8_t Index[3];
    uint32_t Operation_Enum : 3;
    uint32_t Type_Enum : 3;
    uint32_t Layer_Num : 4;
    uint32_t Color_Enum : 4;
    uint32_t Details_A : 9;
    uint32_t Details_B : 9;
    uint32_t Line_Width : 10;
    uint32_t Start_X : 11;
    uint32_t Start_Y : 11;
    uint32_t Details_C : 10;
    uint32_t Details_D : 11;
    uint32_t Details_E : 11;
} Referee_Data_Interaction_Graphic_Config_t;

// UI发送相关
typedef struct __attribute__((packed))
{
    uint16_t Header;
    Enum_Referee_Data_Robots_ID Sender;
    uint8_t Reserved;
    Enum_Referee_Data_Robots_Client_ID Receiver;
    Enum_Referee_Data_Interaction_Layer_Delete_Operation Operation;
    uint8_t Delete_Serial;
    uint16_t CRC_16;
} Referee_Tx_Data_Interaction_Layer_Delete_t;

typedef struct __attribute__((packed))
{
    uint16_t Header;
    Enum_Referee_Data_Robots_ID Sender;
    uint8_t Reserved;
    Enum_Referee_Data_Robots_Client_ID Receiver;
    Referee_Data_Interaction_Graphic_Config_t Graphic[1];
    uint16_t CRC_16;
} Referee_Tx_Data_Interaction_Graphic_1_t;

typedef struct __attribute__((packed))
{
    uint16_t Header;
    Enum_Referee_Data_Robots_ID Sender;
    uint8_t Reserved;
    Enum_Referee_Data_Robots_Client_ID Receiver;
    Referee_Data_Interaction_Graphic_Config_t Graphic[2];
    uint16_t CRC_16;
} Referee_Tx_Data_Interaction_Graphic_2_t;

typedef struct __attribute__((packed))
{
    uint16_t Header;
    Enum_Referee_Data_Robots_ID Sender;
    uint8_t Reserved;
    Enum_Referee_Data_Robots_Client_ID Receiver;
    Referee_Data_Interaction_Graphic_Config_t Graphic[5];
    uint16_t CRC_16;
} Referee_Tx_Data_Interaction_Graphic_5_t;

typedef struct __attribute__((packed))
{
    uint16_t Header;
    Enum_Referee_Data_Robots_ID Sender;
    uint8_t Reserved;
    Enum_Referee_Data_Robots_Client_ID Receiver;
    Referee_Data_Interaction_Graphic_Config_t Graphic[7];
    uint16_t CRC_16;
} Referee_Tx_Data_Interaction_Graphic_7_t;

typedef struct __attribute__((packed))
{
    uint16_t Header;
    Enum_Referee_Data_Robots_ID Sender;
    uint8_t Reserved;
    Enum_Referee_Data_Robots_Client_ID Receiver;
    Referee_Data_Interaction_Graphic_Config_t Graphic_String;
    uint8_t String[30];
    uint16_t CRC_16;
} Referee_Tx_Data_Interaction_Graphic_String_t;


typedef struct __attribute__((packed))
{
    uint8_t Frame_Header;
    uint16_t Data_Length;
    uint8_t Sequence;
    uint8_t CRC_8;
    Enum_Referee_Command_ID Referee_Command_ID;
    uint8_t Data[121];
} Referee_UART_Data_t;

// 辅助函数声明
void parse_referee_data(uint8_t *raw_data);

void encode_interaction_packet(robot_interaction_data_t *data);

extern ext_game_state_t game_state; // 0001
extern ext_game_result_t game_result; // 0002
extern ext_game_robot_HP_t game_robot_HP; // 0003

extern ext_event_data_t event_data; // 0x0101
extern ext_referee_warning_t referee_warning; // 0x0104
extern ext_dart_info_t dart_info; // 0x0105 (2025增)

extern ext_robot_state_t robot_state; // 0x0201
extern ext_power_heat_data_t power_heat_data; // 0x0202
extern ext_robot_pos_t robot_pos; // 0x0203
extern ext_buff_t buff; // 0x0204
extern ext_hurt_data_t hurt_data; // 0x0206
extern ext_shoot_data_t shoot_data; // 0x0207
extern ext_projectile_allowance_t projectile_allowance; // 0x0208
extern ext_rfid_status_t rfid_status; // 0x0209
extern ground_robot_position_t groundRobotPosition; // 0x020B
extern sentry_info_t sentryInfo; // 0x020D
extern robot_interaction_data_t robotInteractionData; // 0x0301


extern ext_robot_command_t robot_command_t; // 0x0303

extern ext_rfid_status_t rfid_status_t;

extern void init_referee_struct_data(void);

extern void referee_data_solve(uint8_t *frame);

extern void get_chassis_power_and_buffer(fp32 *power, fp32 *buffer);

// 获取机器人id
extern uint8_t get_robot_id(void);

extern sentry_info_t sentry_info;

extern sentry_cmd_t sentry_cmd;

extern void get_shoot_heat_limit_and_heat(uint16_t *heat_limit, uint16_t *heat);

// 获取机器人状态指针
ext_robot_state_t *get_robot_status_point(void);

// 获取伤害类型指针
ext_hurt_data_t *get_hurt_data_point(void);

// 获取发射机构弹速
ext_shoot_data_t *get_shoot_data_point(void);

// 获取机器人命令数据指针
ext_robot_command_t *get_robot_command_point(void);

// 获取场地状态指针
ext_event_data_t *get_field_event_point(void);

// 获取比赛机器血量指针
ext_game_robot_HP_t *get_game_robot_HP_point(void);

// 获取机器人位置指针
ext_robot_pos_t *get_robot_pos_point(void);

#endif
