/**
  ******************************************************************************
  * @file           : Transmission_link.h
  * @author         : Chen Haoran
  * @brief          : None
  * @attention      : None
  * @date           : 2026/3/10
  ******************************************************************************
  */
#ifndef CTRLBOARD_H7_IMU_TRANSMISSION_LINK_H
#define CTRLBOARD_H7_IMU_TRANSMISSION_LINK_H
/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "bsp_usart.h"
#include "crc8_crc16/CRC8_CRC16.h"
#include "referee.h"
#include "stdbool.h"
/* Define --------------------------------------------------------------------*/
#define RX_Control_User 39u
#define RX_Control_User_add  78u
#define Edit_User 39u
#define Edit_User_add  78u
#define MAX_SLOPE 200               //斜率限制的最大变化量
#define REFREE_RX_LEN 512u

#define RC_CH_VALUE_MIN         ((uint16_t)364)
#define RC_CH_VALUE_OFFSET      ((uint16_t)1024)
#define RC_CH_VALUE_MAX         ((uint16_t)1684)

#define KEY_PRESSED_OFFSET_W            ((uint16_t)1 << 0)
#define KEY_PRESSED_OFFSET_S            ((uint16_t)1 << 1)
#define KEY_PRESSED_OFFSET_A            ((uint16_t)1 << 2)
#define KEY_PRESSED_OFFSET_D            ((uint16_t)1 << 3)
#define KEY_PRESSED_OFFSET_SHIFT        ((uint16_t)1 << 4)
#define KEY_PRESSED_OFFSET_CTRL         ((uint16_t)1 << 5)
#define KEY_PRESSED_OFFSET_Q            ((uint16_t)1 << 6)
#define KEY_PRESSED_OFFSET_E            ((uint16_t)1 << 7)
#define KEY_PRESSED_OFFSET_R            ((uint16_t)1 << 8)
#define KEY_PRESSED_OFFSET_F            ((uint16_t)1 << 9)
#define KEY_PRESSED_OFFSET_G            ((uint16_t)1 << 10)
#define KEY_PRESSED_OFFSET_Z            ((uint16_t)1 << 11)
#define KEY_PRESSED_OFFSET_X            ((uint16_t)1 << 12)
#define KEY_PRESSED_OFFSET_C            ((uint16_t)1 << 13)
#define KEY_PRESSED_OFFSET_V            ((uint16_t)1 << 14)
#define KEY_PRESSED_OFFSET_B            ((uint16_t)1 << 15)

#define LONG_PRESS_THRESHOLD 200

/* Enum ----------------------------------------------------------------------*/

/* Variable && Struct --------------------------------------------------------*/
typedef enum
{
    Control_Tool = 0x0302, //自定义控制器
    Key_Mouse = 0x0304 //原始键鼠数据
} Enum_Transmission_Command_ID;

typedef struct __attribute__((packed))
{
    //帧头
    uint8_t Frame_Header;
    uint16_t Data_Length;
    uint8_t Sequence;
    uint8_t CRC_8;
    //命令码
    Enum_Transmission_Command_ID Transmission_Command_ID;

    uint8_t Data[121];
} Struct_Transmission_UART_Data;

#pragma pack(push, 1)
typedef struct
{
    //帧头
    uint8_t soft_1; //0xA9
    uint8_t soft_2; //0x53
    //遥控器四个通道
    float ch_0;
    float ch_1;
    float ch_2;
    float ch_3;
    //挡位切换开关
    uint8_t mode_sw;
    //返航/暂停按键
    uint8_t pause;
    //Fn自定义按键
    uint8_t fn_1;
    //拍照切换按键
    uint8_t fn_2;
    //拨轮
    float wheel;
    //拍照/录像按键
    uint8_t trigger;
    //鼠标输出量
    float mouse_x;
    float mouse_y;
    float mouse_z;
    //鼠标按键
    uint8_t mouse_left;
    uint8_t mouse_right;
    uint8_t mouse_middle;
    //键盘返回帧
    uint16_t key;
    //校验
    uint16_t crc16;
} VT13_RC_ctrl_t;
#pragma pack(pop)

typedef struct
{
    bool key_state;
    bool prev_state;
    int press_counter;
    bool long_triggered;
    bool short_toggle_state;
    bool long_toggle_state;
    bool last_long_toggle_state;
}Key_Pressed;

/* Function Declaration ------------------------------------------------------*/
void USER_USART1_RxHandler(UART_HandleTypeDef *huart, uint16_t Size);

void Transmission_Init(void);

void Referee_UART_Send_Interaction_UI_Graphic_1(Referee_Data_Interaction_Graphic_Config_t *Graphic_1,
                                                uint8_t *Tx_buffer);

void Referee_UART_Send_Interaction_UI_Graphic_2(Referee_Data_Interaction_Graphic_Config_t *Graphic_1,
                                                Referee_Data_Interaction_Graphic_Config_t *Graphic_2,
                                                uint8_t *Tx_buffer);

void Referee_UART_Send_Interaction_UI_Graphic_5(Referee_Data_Interaction_Graphic_Config_t *Graphic_1,
                                                Referee_Data_Interaction_Graphic_Config_t *Graphic_2,
                                                Referee_Data_Interaction_Graphic_Config_t *Graphic_3,
                                                Referee_Data_Interaction_Graphic_Config_t *Graphic_4,
                                                Referee_Data_Interaction_Graphic_Config_t *Graphic_5,
                                                uint8_t *Tx_buffer);

void Referee_UART_Send_Interaction_UI_Graphic_7(Referee_Data_Interaction_Graphic_Config_t *Graphic_1,
                                                Referee_Data_Interaction_Graphic_Config_t *Graphic_2,
                                                Referee_Data_Interaction_Graphic_Config_t *Graphic_3,
                                                Referee_Data_Interaction_Graphic_Config_t *Graphic_4,
                                                Referee_Data_Interaction_Graphic_Config_t *Graphic_5,
                                                Referee_Data_Interaction_Graphic_Config_t *Graphic_6,
                                                Referee_Data_Interaction_Graphic_Config_t *Graphic_7,
                                                uint8_t *Tx_buffer);

void Referee_UART_Send_Interaction_UI_Graphic_String(Referee_Data_Interaction_Graphic_Config_t *Graphic_String,
                                                     const char *String_Content, uint8_t *Tx_buffer);

void Set_Referee_UI_Change_Flag_Clear();

Referee_Data_Interaction_Graphic_Config_t *Set_Referee_UI_Clear(uint8_t Layer_Num, uint8_t Graphic_Num);

Referee_Data_Interaction_Graphic_Config_t *Set_Referee_UI_Line(uint8_t Layer_Num, uint8_t Graphic_Num,
                                                               Enum_Referee_Data_Interaction_Graphic_Color Color,
                                                               uint32_t Line_Width, uint32_t Start_X, uint32_t Start_Y,
                                                               uint32_t End_X, uint32_t End_Y);

Referee_Data_Interaction_Graphic_Config_t *Set_Referee_UI_Rectangle(uint8_t Layer_Num, uint8_t Graphic_Num,
                                                                    Enum_Referee_Data_Interaction_Graphic_Color Color,
                                                                    uint32_t Line_Width, uint32_t Start_X,
                                                                    uint32_t Start_Y, uint32_t End_X, uint32_t End_Y);

Referee_Data_Interaction_Graphic_Config_t *Set_Referee_UI_Circle(uint8_t Layer_Num, uint8_t Graphic_Num,
                                                                 Enum_Referee_Data_Interaction_Graphic_Color Color,
                                                                 uint32_t Line_Width, uint32_t Center_X,
                                                                 uint32_t Center_Y, uint32_t Radius);

Referee_Data_Interaction_Graphic_Config_t *Set_Referee_UI_Oval(uint8_t Layer_Num, uint8_t Graphic_Num,
                                                               Enum_Referee_Data_Interaction_Graphic_Color Color,
                                                               uint32_t Line_Width, uint32_t Center_X,
                                                               uint32_t Center_Y, uint32_t Length_X, uint32_t Length_Y);

Referee_Data_Interaction_Graphic_Config_t *Set_Referee_UI_Arc(uint8_t Layer_Num, uint8_t Graphic_Num,
                                                              Enum_Referee_Data_Interaction_Graphic_Color Color,
                                                              uint32_t Line_Width, uint32_t Center_X, uint32_t Center_Y,
                                                              uint32_t Angle_Start, uint32_t Angle_End,
                                                              uint32_t Length_X, uint32_t Length_Y);

Referee_Data_Interaction_Graphic_Config_t *Set_Referee_UI_Float(uint8_t Layer_Num, uint8_t Graphic_Num,
                                                                Enum_Referee_Data_Interaction_Graphic_Color Color,
                                                                uint32_t Line_Width, uint32_t Start_X, uint32_t Start_Y,
                                                                uint32_t Font_Width, float Float);

Referee_Data_Interaction_Graphic_Config_t *Set_Referee_UI_Integer(uint8_t Layer_Num, uint8_t Graphic_Num,
                                                                  Enum_Referee_Data_Interaction_Graphic_Color Color,
                                                                  uint32_t Line_Width, uint32_t Start_X,
                                                                  uint32_t Start_Y, uint32_t Font_Width,
                                                                  int32_t Integer);

Referee_Data_Interaction_Graphic_Config_t *Set_Referee_UI_String(uint8_t Layer_Num, uint8_t Graphic_Num,
                                                                 Enum_Referee_Data_Interaction_Graphic_Color Color,
                                                                 uint32_t Line_Width, uint32_t Start_X,
                                                                 uint32_t Start_Y, uint32_t Font_Width,
                                                                 uint32_t String_Length);

const VT13_RC_ctrl_t *get_VT13_rc_control_point(void);
void process_key(Key_Pressed *key);
/* Function ------------------------------------------------------------------*/


#endif //CTRLBOARD_H7_IMU_TRANSMISSION_LINK_H
