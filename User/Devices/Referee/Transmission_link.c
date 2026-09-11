/**
  ******************************************************************************
  * @file           : Transmission_link.c
  * @author         : Chen Haoran
  * @brief          : None
  * @attention      : None
  * @date           : 2026/3/10
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "Transmission_link.h"
/* Define --------------------------------------------------------------------*/

/* Enum ----------------------------------------------------------------------*/

/* Variable && Struct --------------------------------------------------------*/
uint8_t SOF = 0;
uint8_t control_rx_buf[2][RX_Control_User_add]; //图传链路双缓冲区
uint8_t user_rx_buf[2][Edit_User_add]; //自定义双缓冲区
uint8_t Transmission_rx_buf[REFREE_RX_LEN]; //图传链路单缓冲区
uint8_t Referee[REFREE_RX_LEN]; //裁判系统单缓冲区
uint8_t referee_tx_buffer[128];
uint8_t refree_rx_buf[REFREE_RX_LEN];
VT13_RC_ctrl_t VT13_rc_ctrl;
uint8_t UI_Change_Flag[10][10] = {0};
Referee_Data_Interaction_Graphic_Config_t Graphic_Config[10][10];

/* Function Declaration ------------------------------------------------------*/
void Referee_Data_Process(uint16_t Length);

void Tranmission_Data_Process(uint16_t Length);

/* Function ------------------------------------------------------------------*/

void process_key(Key_Pressed *key)
{
    int cur = key->key_state;
    int prev = key->prev_state;

    if (cur == 1)
    {
        if (prev == 0)
        {
            key->press_counter = 1;
            key->long_triggered = false;
        }
        else
        {
            key->press_counter++;
            if (key->press_counter >= LONG_PRESS_THRESHOLD && !key->long_triggered)
            {
                key->long_toggle_state = !key->long_toggle_state;
                key->long_triggered = true;
            }
        }
    }
    else
    {
        if (prev == 1)
        {
            if (!key->long_triggered)
            {
                key->short_toggle_state = !key->short_toggle_state;
            }
            key->press_counter = 0;
        }
    }

    key->prev_state = cur;

    // key->short_toggle_state = key->key_state;
    // key->long_toggle_state = key->key_state;
}

//裁判系统相关函数
void USER_USART1_RxHandler(UART_HandleTypeDef *huart, uint16_t Size)
{
    // 常规链路接收
    Referee_Data_Process(Size);

    // 图传链路图传遥控器接收
    // Tranmission_Data_Process(Size);
}

void Transmission_Init(void) //裁判系统和图传链路串口缓冲区和DMA启动
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, refree_rx_buf, REFREE_RX_LEN); //裁判系统单缓冲区
    // HAL_UARTEx_ReceiveToIdle_DMA(&huart1, Transmission_rx_buf, REFREE_RX_LEN);  //图传链路单缓冲区
}

uint8_t Verify_CRC_8(uint8_t *Message, uint32_t Length)
{
    uint8_t index;
    uint8_t check = 0xff;

    if (Message == NULL)
    {
        return (check);
    }

    while (Length--)
    {
        index = *Message;
        Message++;
        check = crc_8_table[check ^ index];
    }
    return (check);
}

uint16_t Verify_CRC_16(uint8_t *Message, uint32_t Length)
{
    uint8_t index;
    uint16_t check = 0xffff;

    if (Message == NULL)
    {
        return (check);
    }

    while (Length--)
    {
        index = *Message;
        Message++;
        check = ((uint16_t) (check) >> 8) ^ crc_16_table[((uint16_t) (check) ^ (uint16_t) (index)) & 0xff];
    }
    return (check);
}

void Referee_Data_Process(uint16_t Length)
{
    // 数据处理过程
    Referee_UART_Data_t *tmp_buffer;

    for (int i = 0; i < Length;)
    {
        tmp_buffer = (Referee_UART_Data_t *) &refree_rx_buf[i];

        // 未通过头校验
        if (tmp_buffer->Frame_Header != 0xA5)
        {
            i++;
            continue;
        }
        // 未通过CRC8校验, 顺一位继续判断
        if (Verify_CRC_8((uint8_t *) tmp_buffer, 4) != tmp_buffer->CRC_8)
        {
            i++;
            continue;
        }
        // 未通过CRC16校验, 跨过当前包继续判断
        if (Verify_CRC_16((uint8_t *) tmp_buffer, 7 + tmp_buffer->Data_Length) != *(uint16_t *) (
                (uint32_t) tmp_buffer + 7 + tmp_buffer->Data_Length))
        {
            i += 9 + tmp_buffer->Data_Length;
            continue;
        }
        // 通过校验但帧不够长
        if (i + 7 + tmp_buffer->Data_Length + 2 > Length)
        {
            break;
        }

        switch (tmp_buffer->Referee_Command_ID)
        {
        case Referee_Command_ID_GAME_STATUS:
        {
            memcpy(&game_state, tmp_buffer->Data, sizeof(ext_game_state_t));
        }
        break;
        case Referee_Command_ID_GAME_RESULT:
        {
            memcpy(&game_result, tmp_buffer->Data, sizeof(ext_game_result_t));
        }
        break;
        case Referee_Command_ID_GAME_ROBOT_HP:
        {
            memcpy(&game_robot_HP, tmp_buffer->Data, sizeof(ext_game_robot_HP_t));
        }
        break;

        case Referee_Command_ID_EVENT_SELF_DATA:
        {
            memcpy(&event_data, tmp_buffer->Data, sizeof(ext_event_data_t));
        }
        break;
        case Referee_Command_ID_EVENT_SELF_REFEREE_WARNING:
        {
            memcpy(&referee_warning, tmp_buffer->Data, sizeof(ext_referee_warning_t));
        }
        break;
        case Referee_Command_ID_EVENT_SELF_DART_STATUS:
        {
            memcpy(&dart_info, tmp_buffer->Data, sizeof(ext_dart_info_t));
        }
        break;
        case Referee_Command_ID_ROBOT_STATUS:
        {
            memcpy(&robot_state, tmp_buffer->Data, sizeof(ext_robot_state_t));
        }
        break;
        case Referee_Command_ID_ROBOT_POWER_HEAT:
        {
            memcpy(&power_heat_data, tmp_buffer->Data, sizeof(ext_power_heat_data_t));
        }
        break;
        case Referee_Command_ID_ROBOT_POSITION:
        {
            memcpy(&robot_pos, tmp_buffer->Data, sizeof(ext_robot_pos_t));
        }
        break;
        case Referee_Command_ID_ROBOT_BUFF:
        {
            memcpy(&buff, tmp_buffer->Data, sizeof(ext_buff_t));
        }
        break;
        case Referee_Command_ID_ROBOT_DAMAGE:
        {
            memcpy(&hurt_data, tmp_buffer->Data, sizeof(ext_hurt_data_t));
        }
        break;
        case Referee_Command_ID_ROBOT_BOOSTER:
        {
            memcpy(&shoot_data, tmp_buffer->Data, sizeof(ext_shoot_data_t));
        }
        break;
        case Referee_Command_ID_ROBOT_REMAINING_AMMO:
        {
            memcpy(&projectile_allowance, tmp_buffer->Data, sizeof(ext_projectile_allowance_t));
        }
        break;
        case Referee_Command_ID_ROBOT_RFID:
        {
            memcpy(&rfid_status, tmp_buffer->Data, sizeof(ext_rfid_status_t));
        }
        break;
        case Referee_Command_ID_ROBOT_SENTRY_LOCATION:
        {
            memcpy(&groundRobotPosition, tmp_buffer->Data, sizeof(ground_robot_position_t));
        }
        break;
        case Referee_Command_ID_ROBOT_SENTRY_DECISION:
        {
            memcpy(&sentryInfo, tmp_buffer->Data, sizeof(sentry_info_t));
        }
        break;
        case Referee_Command_ID_INTERACTION:
        {
            memcpy(&robotInteractionData, tmp_buffer->Data, sizeof(robot_interaction_data_t));
        }
        break;
        case ROBOT_COMMAND_ID:
        {
            memcpy(&robot_command_t, tmp_buffer->Data, sizeof(ext_robot_command_t));
        }
        break;
        default:
        {
            break;
        }
        }

        //缓冲区直接推移
        i += 7 + tmp_buffer->Data_Length + 2;
    }
}

void Sentry_Decision_Transmit(sentry_cmd_t *sentry_info, uint8_t *tx_buffer)
{
    Referee_UART_Data_t *tmp_buffer = (Referee_UART_Data_t *) tx_buffer;

    tmp_buffer->Frame_Header = 0xa5;
    tmp_buffer->Data_Length = sizeof(sentry_cmd_t) - 2;
    tmp_buffer->Sequence = SOF;
    tmp_buffer->CRC_8 = Verify_CRC_8((uint8_t *) tmp_buffer, 4);
    tmp_buffer->Referee_Command_ID = 0x0301;

    sentry_cmd_t *tmp_data = (sentry_cmd_t *) tmp_buffer->Data;
    tmp_data->Header = 0x0120;
    tmp_data->Sender = robot_state.robot_id;
    tmp_data->Receiver = Referee_Data_Robots_Server;

    tmp_data->Confirm_Respawn_Status = sentry_info->Confirm_Respawn_Status;
    tmp_data->Confirm_Exchange_Respawn_Status = sentry_info->Confirm_Exchange_Respawn_Status;
    tmp_data->Request_Exchange_Ammo_Number = sentry_info->Request_Exchange_Ammo_Number;
    tmp_data->Request_Exchange_Ammo_Time = sentry_info->Request_Exchange_Ammo_Time;
    tmp_data->Request_Exchange_HP_Time = sentry_info->Request_Exchange_HP_Time;

    tmp_data->CRC_16 = Verify_CRC_16((uint8_t *) tmp_buffer, 7 + tmp_buffer->Data_Length);

    HAL_UART_Transmit(&huart1, (uint8_t *) tmp_buffer, 7 + sizeof(sentry_cmd_t), 80);
    SOF++;
}

//图传链路数据处理
void Tranmission_Data_Process(uint16_t Length)
{
    // 数据处理过程
    Struct_Transmission_UART_Data *tmp_buffer;

    // for (int i = 0; i < Length;)
    // {
    //     tmp_buffer = (Struct_Transmission_UART_Data *) &Transmission_rx_buf[i];
    //     // 检测帧头0xA9
    //     if (Transmission_rx_buf[i] == 0xA9 && (i + 1 < Length))
    //     {
    //         // 检查第二字节是否为0x53
    //         if (Transmission_rx_buf[i + 1] == 0x53)
    //         {
    //             // 确认数据长度足够
    //             if (i + 21 > Length)
    //             {
    //                 break; // 数据不足，等待后续
    //             }
    //
    //             memcpy(&VT13_rc_ctrl, &Transmission_rx_buf[i], 21);
    //             i += 21;
    //             continue;
    //         }
    //         else
    //         {
    //             i++;
    //             continue;
    //         }
    //     }
    // }


    for (int i = 0; i < Length;)
    {
        // 循环提取数据

        // 验证第一个包头
        if (Transmission_rx_buf[i] == 0xA9 && (i + 1 < Length))
        {
            // 验证第二个包头
            if (Transmission_rx_buf[i + 1] == 0x53)
            {
                // 确认数据长度足够
                if (i + 21 > Length)
                {
                    break; //数据不足等待后续
                }

                // CRC16校验
                if (verify_CRC16_check_sum(&Transmission_rx_buf[i], 21))
                {
                    memcpy(&VT13_rc_ctrl, &Transmission_rx_buf[i], 21);
                }
            }
        }
        else
        {
            i++;
            continue;
        }
    }
}

/**
 * @brief 设定裁判系统UI清除
 *
 */
void Set_Referee_UI_Change_Flag_Clear()
{
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            UI_Change_Flag[i][j] = 0;
        }
    }
}

/**
 * @brief 设定裁判系统UI清除
 *
 * @param Layer_Num 图层编号, 0~9
 * @param Graphic_Num 图形编号, 0~9
 */
Referee_Data_Interaction_Graphic_Config_t *Set_Referee_UI_Clear(uint8_t Layer_Num, uint8_t Graphic_Num)
{
    Graphic_Config[Layer_Num][Graphic_Num].Index[0] = '0';
    Graphic_Config[Layer_Num][Graphic_Num].Index[1] = '0' + Layer_Num;

    Graphic_Config[Layer_Num][Graphic_Num].Index[2] = '0' + Graphic_Num;

    Graphic_Config[Layer_Num][Graphic_Num].Operation_Enum = Referee_Data_Interaction_Graphic_Operation_DELETE;

    UI_Change_Flag[Layer_Num][Graphic_Num] = 0;

    return (&Graphic_Config[Layer_Num][Graphic_Num]);
}


/**
 * @brief 设定裁判系统UI直线
 *
 * @param Layer_Num 图层编号, 0~9
 * @param Graphic_Num 图形编号, 0~9
 * @param Color 图形颜色
 * @param Line_Width 线宽
 * @param Start_X 起点x
 * @param Start_Y 起点y
 * @param End_X 终点x
 * @param End_Y 终点y
 * @return 对应图层指针
 */
Referee_Data_Interaction_Graphic_Config_t *Set_Referee_UI_Line(uint8_t Layer_Num, uint8_t Graphic_Num,
                                                               Enum_Referee_Data_Interaction_Graphic_Color Color,
                                                               uint32_t Line_Width, uint32_t Start_X, uint32_t Start_Y,
                                                               uint32_t End_X, uint32_t End_Y)
{
    Graphic_Config[Layer_Num][Graphic_Num].Index[0] = '0';
    Graphic_Config[Layer_Num][Graphic_Num].Index[1] = '0' + Layer_Num;
    Graphic_Config[Layer_Num][Graphic_Num].Index[2] = '0' + Graphic_Num;

    if (UI_Change_Flag[Layer_Num][Graphic_Num] == 0)
    {
        Graphic_Config[Layer_Num][Graphic_Num].Operation_Enum = Referee_Data_Interaction_Graphic_Operation_ADD;
        UI_Change_Flag[Layer_Num][Graphic_Num] = 1;
    }
    else
    {
        Graphic_Config[Layer_Num][Graphic_Num].Operation_Enum = Referee_Data_Interaction_Graphic_Operation_CHANGE;
    }

    Graphic_Config[Layer_Num][Graphic_Num].Type_Enum = Referee_Data_Interaction_Graphic_Type_LINE;
    Graphic_Config[Layer_Num][Graphic_Num].Layer_Num = Layer_Num;
    Graphic_Config[Layer_Num][Graphic_Num].Color_Enum = Color;
    Graphic_Config[Layer_Num][Graphic_Num].Line_Width = Line_Width;
    Graphic_Config[Layer_Num][Graphic_Num].Start_X = Start_X;
    Graphic_Config[Layer_Num][Graphic_Num].Start_Y = Start_Y;
    Graphic_Config[Layer_Num][Graphic_Num].Details_D = End_X;
    Graphic_Config[Layer_Num][Graphic_Num].Details_E = End_Y;

    return (&Graphic_Config[Layer_Num][Graphic_Num]);
}

/**
 * @brief 设定裁判系统UI矩形
 *
 * @param Layer_Num 图层编号, 0~9
 * @param Graphic_Num 图形编号, 0~9
 * @param Color 图形颜色
 * @param Line_Width 线宽
 * @param Start_X 起点x
 * @param Start_Y 起点y
 * @param End_X 终点x
 * @param End_Y 终点y
 * @return 对应图层指针
 */
Referee_Data_Interaction_Graphic_Config_t *Set_Referee_UI_Rectangle(uint8_t Layer_Num, uint8_t Graphic_Num,
                                                                    Enum_Referee_Data_Interaction_Graphic_Color Color,
                                                                    uint32_t Line_Width, uint32_t Start_X,
                                                                    uint32_t Start_Y, uint32_t End_X, uint32_t End_Y)
{
    Graphic_Config[Layer_Num][Graphic_Num].Index[0] = '0';
    Graphic_Config[Layer_Num][Graphic_Num].Index[1] = '0' + Layer_Num;
    Graphic_Config[Layer_Num][Graphic_Num].Index[2] = '0' + Graphic_Num;

    if (UI_Change_Flag[Layer_Num][Graphic_Num] == 0)
    {
        Graphic_Config[Layer_Num][Graphic_Num].Operation_Enum = Referee_Data_Interaction_Graphic_Operation_ADD;
        UI_Change_Flag[Layer_Num][Graphic_Num] = 1;
    }
    else
    {
        Graphic_Config[Layer_Num][Graphic_Num].Operation_Enum = Referee_Data_Interaction_Graphic_Operation_CHANGE;
    }

    Graphic_Config[Layer_Num][Graphic_Num].Type_Enum = Referee_Data_Interaction_Graphic_Type_RECTANGLE;
    Graphic_Config[Layer_Num][Graphic_Num].Layer_Num = Layer_Num;
    Graphic_Config[Layer_Num][Graphic_Num].Color_Enum = Color;
    Graphic_Config[Layer_Num][Graphic_Num].Line_Width = Line_Width;
    Graphic_Config[Layer_Num][Graphic_Num].Start_X = Start_X;
    Graphic_Config[Layer_Num][Graphic_Num].Start_Y = Start_Y;
    Graphic_Config[Layer_Num][Graphic_Num].Details_D = End_X;
    Graphic_Config[Layer_Num][Graphic_Num].Details_E = End_Y;

    return (&Graphic_Config[Layer_Num][Graphic_Num]);
}

/**
 * @brief 设定裁判系统UI圆形
 *
 * @param Layer_Num 图层编号, 0~9
 * @param Graphic_Num 图形编号, 0~9
 * @param Color 图形颜色
 * @param Line_Width 线宽
 * @param Center_X 圆心x
 * @param Center_Y 圆心y
 * @param Radius 半径
 * @return 对应图层指针
 */
Referee_Data_Interaction_Graphic_Config_t *Set_Referee_UI_Circle(uint8_t Layer_Num, uint8_t Graphic_Num,
                                                                 Enum_Referee_Data_Interaction_Graphic_Color Color,
                                                                 uint32_t Line_Width, uint32_t Center_X,
                                                                 uint32_t Center_Y, uint32_t Radius)
{
    Graphic_Config[Layer_Num][Graphic_Num].Index[0] = '0';
    Graphic_Config[Layer_Num][Graphic_Num].Index[1] = '0' + Layer_Num;
    Graphic_Config[Layer_Num][Graphic_Num].Index[2] = '0' + Graphic_Num;

    if (UI_Change_Flag[Layer_Num][Graphic_Num] == 0)
    {
        Graphic_Config[Layer_Num][Graphic_Num].Operation_Enum = Referee_Data_Interaction_Graphic_Operation_ADD;
        UI_Change_Flag[Layer_Num][Graphic_Num] = 1;
    }
    else
    {
        Graphic_Config[Layer_Num][Graphic_Num].Operation_Enum = Referee_Data_Interaction_Graphic_Operation_CHANGE;
    }

    Graphic_Config[Layer_Num][Graphic_Num].Type_Enum = Referee_Data_Interaction_Graphic_Type_CIRCLE;
    Graphic_Config[Layer_Num][Graphic_Num].Layer_Num = Layer_Num;
    Graphic_Config[Layer_Num][Graphic_Num].Color_Enum = Color;
    Graphic_Config[Layer_Num][Graphic_Num].Line_Width = Line_Width;
    Graphic_Config[Layer_Num][Graphic_Num].Start_X = Center_X;
    Graphic_Config[Layer_Num][Graphic_Num].Start_Y = Center_Y;
    Graphic_Config[Layer_Num][Graphic_Num].Details_C = Radius;

    return (&Graphic_Config[Layer_Num][Graphic_Num]);
}

/**
 * @brief 设定裁判系统UI椭圆形
 *
 * @param Layer_Num 图层编号, 0~9
 * @param Graphic_Num 图形编号, 0~9
 * @param Color 图形颜色
 * @param Line_Width 线宽
 * @param Center_X 圆心x
 * @param Center_Y 圆心y
 * @param Length_X x半轴长度
 * @param Length_Y y半轴长度
 * @return 对应图层指针
 */
Referee_Data_Interaction_Graphic_Config_t *Set_Referee_UI_Oval(uint8_t Layer_Num, uint8_t Graphic_Num,
                                                               Enum_Referee_Data_Interaction_Graphic_Color Color,
                                                               uint32_t Line_Width, uint32_t Center_X,
                                                               uint32_t Center_Y, uint32_t Length_X, uint32_t Length_Y)
{
    Graphic_Config[Layer_Num][Graphic_Num].Index[0] = '0';
    Graphic_Config[Layer_Num][Graphic_Num].Index[1] = '0' + Layer_Num;
    Graphic_Config[Layer_Num][Graphic_Num].Index[2] = '0' + Graphic_Num;

    if (UI_Change_Flag[Layer_Num][Graphic_Num] == 0)
    {
        Graphic_Config[Layer_Num][Graphic_Num].Operation_Enum = Referee_Data_Interaction_Graphic_Operation_ADD;
        UI_Change_Flag[Layer_Num][Graphic_Num] = 1;
    }
    else
    {
        Graphic_Config[Layer_Num][Graphic_Num].Operation_Enum = Referee_Data_Interaction_Graphic_Operation_CHANGE;
    }

    Graphic_Config[Layer_Num][Graphic_Num].Type_Enum = Referee_Data_Interaction_Graphic_Type_OVAL;
    Graphic_Config[Layer_Num][Graphic_Num].Layer_Num = Layer_Num;
    Graphic_Config[Layer_Num][Graphic_Num].Color_Enum = Color;
    Graphic_Config[Layer_Num][Graphic_Num].Line_Width = Line_Width;
    Graphic_Config[Layer_Num][Graphic_Num].Start_X = Center_X;
    Graphic_Config[Layer_Num][Graphic_Num].Start_Y = Center_Y;
    Graphic_Config[Layer_Num][Graphic_Num].Details_D = Length_X;
    Graphic_Config[Layer_Num][Graphic_Num].Details_E = Length_Y;

    return (&Graphic_Config[Layer_Num][Graphic_Num]);
}

/**
 * @brief 设定裁判系统UI圆弧形
 *
 * @param Layer_Num 图层编号, 0~9
 * @param Graphic_Num 图形编号, 0~9
 * @param Color 图形颜色
 * @param Line_Width 线宽
 * @param Center_X 圆心x
 * @param Center_Y 圆心y
 * @param Angle_Start 起始角度
 * @param Angle_End 终止角度
 * @param Length_X x半轴长度
 * @param Length_Y y半轴长度
 * @return 对应图层指针
 */
Referee_Data_Interaction_Graphic_Config_t *Set_Referee_UI_Arc(uint8_t Layer_Num, uint8_t Graphic_Num,
                                                              Enum_Referee_Data_Interaction_Graphic_Color Color,
                                                              uint32_t Line_Width, uint32_t Center_X, uint32_t Center_Y,
                                                              uint32_t Angle_Start, uint32_t Angle_End,
                                                              uint32_t Length_X, uint32_t Length_Y)
{
    Graphic_Config[Layer_Num][Graphic_Num].Index[0] = '0';
    Graphic_Config[Layer_Num][Graphic_Num].Index[1] = '0' + Layer_Num;
    Graphic_Config[Layer_Num][Graphic_Num].Index[2] = '0' + Graphic_Num;

    if (UI_Change_Flag[Layer_Num][Graphic_Num] == 0)
    {
        Graphic_Config[Layer_Num][Graphic_Num].Operation_Enum = Referee_Data_Interaction_Graphic_Operation_ADD;
        UI_Change_Flag[Layer_Num][Graphic_Num] = 1;
    }
    else
    {
        Graphic_Config[Layer_Num][Graphic_Num].Operation_Enum = Referee_Data_Interaction_Graphic_Operation_CHANGE;
    }

    Graphic_Config[Layer_Num][Graphic_Num].Type_Enum = Referee_Data_Interaction_Graphic_Type_ARC;
    Graphic_Config[Layer_Num][Graphic_Num].Layer_Num = Layer_Num;
    Graphic_Config[Layer_Num][Graphic_Num].Color_Enum = Color;
    Graphic_Config[Layer_Num][Graphic_Num].Line_Width = Line_Width;
    Graphic_Config[Layer_Num][Graphic_Num].Start_X = Center_X;
    Graphic_Config[Layer_Num][Graphic_Num].Start_Y = Center_Y;
    Graphic_Config[Layer_Num][Graphic_Num].Details_A = Angle_Start;
    Graphic_Config[Layer_Num][Graphic_Num].Details_B = Angle_End;
    Graphic_Config[Layer_Num][Graphic_Num].Details_D = Length_X;
    Graphic_Config[Layer_Num][Graphic_Num].Details_E = Length_Y;

    return (&Graphic_Config[Layer_Num][Graphic_Num]);
}

/**
 * @brief 设定裁判系统UI浮点数
 *
 * @param Layer_Num 图层编号, 0~9
 * @param Graphic_Num 图形编号, 0~9
 * @param Color 图形颜色
 * @param Line_Width 线宽
 * @param Start_X 起点x
 * @param Start_Y 起点y
 * @param Font_Width 字体大小
 * @param Float 数值
 * @return 对应图层指针
 */
Referee_Data_Interaction_Graphic_Config_t *Set_Referee_UI_Float(uint8_t Layer_Num, uint8_t Graphic_Num,
                                                                Enum_Referee_Data_Interaction_Graphic_Color Color,
                                                                uint32_t Line_Width, uint32_t Start_X, uint32_t Start_Y,
                                                                uint32_t Font_Width, float Float)
{
    Graphic_Config[Layer_Num][Graphic_Num].Index[0] = '0';
    Graphic_Config[Layer_Num][Graphic_Num].Index[1] = '0' + Layer_Num;
    Graphic_Config[Layer_Num][Graphic_Num].Index[2] = '0' + Graphic_Num;

    if (UI_Change_Flag[Layer_Num][Graphic_Num] == 0)
    {
        Graphic_Config[Layer_Num][Graphic_Num].Operation_Enum = Referee_Data_Interaction_Graphic_Operation_ADD;
        UI_Change_Flag[Layer_Num][Graphic_Num] = 1;
    }
    else
    {
        Graphic_Config[Layer_Num][Graphic_Num].Operation_Enum = Referee_Data_Interaction_Graphic_Operation_CHANGE;
    }

    Graphic_Config[Layer_Num][Graphic_Num].Type_Enum = Referee_Data_Interaction_Graphic_Type_FLOAT;
    Graphic_Config[Layer_Num][Graphic_Num].Layer_Num = Layer_Num;
    Graphic_Config[Layer_Num][Graphic_Num].Color_Enum = Color;
    Graphic_Config[Layer_Num][Graphic_Num].Line_Width = Line_Width;
    Graphic_Config[Layer_Num][Graphic_Num].Start_X = Start_X;
    Graphic_Config[Layer_Num][Graphic_Num].Start_Y = Start_Y;
    Graphic_Config[Layer_Num][Graphic_Num].Details_A = Font_Width;
    int32_t *tmp_pointer = (int32_t *) ((uint32_t) &Graphic_Config[Layer_Num][Graphic_Num] + 11);
    *tmp_pointer = (int32_t) (Float * 1000.0f);

    return (&Graphic_Config[Layer_Num][Graphic_Num]);
}

/**
 * @brief 设定裁判系统UI整型数
 *
 * @param Layer_Num 图层编号, 0~9
 * @param Graphic_Num 图形编号, 0~9
 * @param Color 图形颜色
 * @param Line_Width 线宽
 * @param Start_X 起点x
 * @param Start_Y 起点y
 * @param Font_Width 字体大小
 * @param Integer 数值
 * @return 对应图层指针
 */
Referee_Data_Interaction_Graphic_Config_t *Set_Referee_UI_Integer(uint8_t Layer_Num, uint8_t Graphic_Num,
                                                                  Enum_Referee_Data_Interaction_Graphic_Color Color,
                                                                  uint32_t Line_Width, uint32_t Start_X,
                                                                  uint32_t Start_Y, uint32_t Font_Width,
                                                                  int32_t Integer)
{
    Graphic_Config[Layer_Num][Graphic_Num].Index[0] = '0';
    Graphic_Config[Layer_Num][Graphic_Num].Index[1] = '0' + Layer_Num;
    Graphic_Config[Layer_Num][Graphic_Num].Index[2] = '0' + Graphic_Num;

    if (UI_Change_Flag[Layer_Num][Graphic_Num] == 0)
    {
        Graphic_Config[Layer_Num][Graphic_Num].Operation_Enum = Referee_Data_Interaction_Graphic_Operation_ADD;
        UI_Change_Flag[Layer_Num][Graphic_Num] = 1;
    }
    else
    {
        Graphic_Config[Layer_Num][Graphic_Num].Operation_Enum = Referee_Data_Interaction_Graphic_Operation_CHANGE;
    }

    Graphic_Config[Layer_Num][Graphic_Num].Type_Enum = Referee_Data_Interaction_Graphic_Type_INTEGER;
    Graphic_Config[Layer_Num][Graphic_Num].Layer_Num = Layer_Num;
    Graphic_Config[Layer_Num][Graphic_Num].Color_Enum = Color;
    Graphic_Config[Layer_Num][Graphic_Num].Line_Width = Line_Width;
    Graphic_Config[Layer_Num][Graphic_Num].Start_X = Start_X;
    Graphic_Config[Layer_Num][Graphic_Num].Start_Y = Start_Y;
    Graphic_Config[Layer_Num][Graphic_Num].Details_A = Font_Width;
    int32_t *tmp_pointer = (int32_t *) ((uint32_t) &Graphic_Config[Layer_Num][Graphic_Num] + 11);
    *tmp_pointer = (int32_t) (Integer);

    return (&Graphic_Config[Layer_Num][Graphic_Num]);
}

/**
 * @brief 设定裁判系统UI字符串
 *
 * @param Layer_Num 图层编号, 0~9
 * @param Graphic_Num 图形编号, 0~9
 * @param Color 图形颜色
 * @param Line_Width 线宽
 * @param Start_X 起点x
 * @param Start_Y 起点y
 * @param Font_Width 字体大小
 * @param String_Length 字符串长度
 * @return
 */
Referee_Data_Interaction_Graphic_Config_t *Set_Referee_UI_String(uint8_t Layer_Num, uint8_t Graphic_Num,
                                                                 Enum_Referee_Data_Interaction_Graphic_Color Color,
                                                                 uint32_t Line_Width, uint32_t Start_X,
                                                                 uint32_t Start_Y, uint32_t Font_Width,
                                                                 uint32_t String_Length)
{
    Graphic_Config[Layer_Num][Graphic_Num].Index[0] = '0';
    Graphic_Config[Layer_Num][Graphic_Num].Index[1] = '0' + Layer_Num;
    Graphic_Config[Layer_Num][Graphic_Num].Index[2] = '0' + Graphic_Num;

    if (UI_Change_Flag[Layer_Num][Graphic_Num] == 0)
    {
        Graphic_Config[Layer_Num][Graphic_Num].Operation_Enum = Referee_Data_Interaction_Graphic_Operation_ADD;
        UI_Change_Flag[Layer_Num][Graphic_Num] = 1;
    }
    else
    {
        Graphic_Config[Layer_Num][Graphic_Num].Operation_Enum = Referee_Data_Interaction_Graphic_Operation_CHANGE;
    }

    Graphic_Config[Layer_Num][Graphic_Num].Type_Enum = Referee_Data_Interaction_Graphic_Type_STRING;
    Graphic_Config[Layer_Num][Graphic_Num].Layer_Num = Layer_Num;
    Graphic_Config[Layer_Num][Graphic_Num].Color_Enum = Color;
    Graphic_Config[Layer_Num][Graphic_Num].Line_Width = Line_Width;
    Graphic_Config[Layer_Num][Graphic_Num].Start_X = Start_X;
    Graphic_Config[Layer_Num][Graphic_Num].Start_Y = Start_Y;
    Graphic_Config[Layer_Num][Graphic_Num].Details_A = Font_Width;
    Graphic_Config[Layer_Num][Graphic_Num].Details_B = String_Length;

    return (&Graphic_Config[Layer_Num][Graphic_Num]);
}

void Referee_UART_Send_Interaction_UI_Graphic_1(Referee_Data_Interaction_Graphic_Config_t *Graphic_1,
                                                uint8_t *Tx_buffer)
{
    Referee_UART_Data_t *tmp_buffer = (Referee_UART_Data_t *) Tx_buffer;

    // 裁判系统帧头
    tmp_buffer->Frame_Header = 0xA5;
    tmp_buffer->Data_Length = sizeof(Referee_Tx_Data_Interaction_Graphic_1_t) - 2;
    tmp_buffer->Sequence = SOF;
    tmp_buffer->CRC_8 = Verify_CRC_8((uint8_t *) tmp_buffer, 4);
    tmp_buffer->Referee_Command_ID = Referee_Command_ID_INTERACTION;

    // 交互帧头
    Referee_Tx_Data_Interaction_Graphic_1_t *tmp_data = (Referee_Tx_Data_Interaction_Graphic_1_t *) tmp_buffer->Data;
    tmp_data->Header = Referee_Interaction_Command_ID_UI_GRAPHIC_1;
    tmp_data->Sender = robot_state.robot_id;
    tmp_data->Receiver = (Enum_Referee_Data_Robots_Client_ID) ((int) (robot_state.robot_id) + 0x100);

    // UI发一个图形帧内容
    tmp_data->Graphic[0] = *Graphic_1;
    tmp_data->CRC_16 = Verify_CRC_16((uint8_t *) tmp_buffer, 7 + tmp_buffer->Data_Length);

    HAL_UART_Transmit(&huart1, (uint8_t *) tmp_buffer, 7 + sizeof(Referee_Tx_Data_Interaction_Graphic_1_t), 100);

    SOF++;
}

void Referee_UART_Send_Interaction_UI_Graphic_2(Referee_Data_Interaction_Graphic_Config_t *Graphic_1,
                                                Referee_Data_Interaction_Graphic_Config_t *Graphic_2,
                                                uint8_t *Tx_buffer)
{
    Referee_UART_Data_t *tmp_buffer = (Referee_UART_Data_t *) Tx_buffer;

    // 裁判系统帧头
    tmp_buffer->Frame_Header = 0xA5;
    tmp_buffer->Data_Length = sizeof(Referee_Tx_Data_Interaction_Graphic_2_t) - 2;
    tmp_buffer->Sequence = SOF;
    tmp_buffer->CRC_8 = Verify_CRC_8((uint8_t *) tmp_buffer, 4);
    tmp_buffer->Referee_Command_ID = Referee_Command_ID_INTERACTION;

    // 交互帧头
    Referee_Tx_Data_Interaction_Graphic_2_t *tmp_data = (Referee_Tx_Data_Interaction_Graphic_2_t *) tmp_buffer->Data;
    tmp_data->Header = Referee_Interaction_Command_ID_UI_GRAPHIC_2;
    tmp_data->Sender = robot_state.robot_id;
    tmp_data->Receiver = (Enum_Referee_Data_Robots_Client_ID) ((int) (robot_state.robot_id) + 0x100);

    // UI发一个图形帧内容
    tmp_data->Graphic[0] = *Graphic_1;
    tmp_data->Graphic[1] = *Graphic_2;
    tmp_data->CRC_16 = Verify_CRC_16((uint8_t *) tmp_buffer, 7 + tmp_buffer->Data_Length);

    HAL_UART_Transmit(&huart1, (uint8_t *) tmp_buffer, 7 + sizeof(Referee_Tx_Data_Interaction_Graphic_2_t), 100);

    SOF++;
}

void Referee_UART_Send_Interaction_UI_Graphic_5(Referee_Data_Interaction_Graphic_Config_t *Graphic_1,
                                                Referee_Data_Interaction_Graphic_Config_t *Graphic_2,
                                                Referee_Data_Interaction_Graphic_Config_t *Graphic_3,
                                                Referee_Data_Interaction_Graphic_Config_t *Graphic_4,
                                                Referee_Data_Interaction_Graphic_Config_t *Graphic_5,
                                                uint8_t *Tx_buffer)
{
    Referee_UART_Data_t *tmp_buffer = (Referee_UART_Data_t *) Tx_buffer;

    // 裁判系统帧头
    tmp_buffer->Frame_Header = 0xA5;
    tmp_buffer->Data_Length = sizeof(Referee_Tx_Data_Interaction_Graphic_5_t) - 2;
    tmp_buffer->Sequence = SOF;
    tmp_buffer->CRC_8 = Verify_CRC_8((uint8_t *) tmp_buffer, 4);
    tmp_buffer->Referee_Command_ID = Referee_Command_ID_INTERACTION;

    // 交互帧头
    Referee_Tx_Data_Interaction_Graphic_5_t *tmp_data = (Referee_Tx_Data_Interaction_Graphic_5_t *) tmp_buffer->Data;
    tmp_data->Header = Referee_Interaction_Command_ID_UI_GRAPHIC_5;
    tmp_data->Sender = robot_state.robot_id;
    tmp_data->Receiver = (Enum_Referee_Data_Robots_Client_ID) ((int) (robot_state.robot_id) + 0x100);

    // UI发一个图形帧内容
    tmp_data->Graphic[0] = *Graphic_1;
    tmp_data->Graphic[1] = *Graphic_2;
    tmp_data->Graphic[2] = *Graphic_3;
    tmp_data->Graphic[3] = *Graphic_4;
    tmp_data->Graphic[4] = *Graphic_5;
    tmp_data->CRC_16 = Verify_CRC_16((uint8_t *) tmp_buffer, 7 + tmp_buffer->Data_Length);

    HAL_UART_Transmit(&huart1, (uint8_t *) tmp_buffer, 7 + sizeof(Referee_Tx_Data_Interaction_Graphic_5_t), 100);

    SOF++;
}

void Referee_UART_Send_Interaction_UI_Graphic_7(Referee_Data_Interaction_Graphic_Config_t *Graphic_1,
                                                Referee_Data_Interaction_Graphic_Config_t *Graphic_2,
                                                Referee_Data_Interaction_Graphic_Config_t *Graphic_3,
                                                Referee_Data_Interaction_Graphic_Config_t *Graphic_4,
                                                Referee_Data_Interaction_Graphic_Config_t *Graphic_5,
                                                Referee_Data_Interaction_Graphic_Config_t *Graphic_6,
                                                Referee_Data_Interaction_Graphic_Config_t *Graphic_7,
                                                uint8_t *Tx_buffer)
{
    Referee_UART_Data_t *tmp_buffer = (Referee_UART_Data_t *) Tx_buffer;

    // 裁判系统帧头
    tmp_buffer->Frame_Header = 0xA5;
    tmp_buffer->Data_Length = sizeof(Referee_Tx_Data_Interaction_Graphic_7_t) - 2;
    tmp_buffer->Sequence = SOF;
    tmp_buffer->CRC_8 = Verify_CRC_8((uint8_t *) tmp_buffer, 4);
    tmp_buffer->Referee_Command_ID = Referee_Command_ID_INTERACTION;

    // 交互帧头
    Referee_Tx_Data_Interaction_Graphic_7_t *tmp_data = (Referee_Tx_Data_Interaction_Graphic_7_t *) tmp_buffer->Data;
    tmp_data->Header = Referee_Interaction_Command_ID_UI_GRAPHIC_7;
    tmp_data->Sender = robot_state.robot_id;
    tmp_data->Receiver = (Enum_Referee_Data_Robots_Client_ID) ((int) (robot_state.robot_id) + 0x100);

    // UI发一个图形帧内容
    tmp_data->Graphic[0] = *Graphic_1;
    tmp_data->Graphic[1] = *Graphic_2;
    tmp_data->Graphic[2] = *Graphic_3;
    tmp_data->Graphic[3] = *Graphic_4;
    tmp_data->Graphic[4] = *Graphic_5;
    tmp_data->Graphic[5] = *Graphic_6;
    tmp_data->Graphic[6] = *Graphic_7;
    tmp_data->CRC_16 = Verify_CRC_16((uint8_t *) tmp_buffer, 7 + tmp_buffer->Data_Length);

    HAL_UART_Transmit(&huart1, (uint8_t *) tmp_buffer, 7 + sizeof(Referee_Tx_Data_Interaction_Graphic_7_t), 100);

    SOF++;
}

void Referee_UART_Send_Interaction_UI_Graphic_String(Referee_Data_Interaction_Graphic_Config_t *Graphic_String,
                                                     const char *String_Content, uint8_t *Tx_buffer)
{
    Referee_UART_Data_t *tmp_buffer = (Referee_UART_Data_t *) Tx_buffer;

    // 裁判系统帧头
    tmp_buffer->Frame_Header = 0xa5;
    tmp_buffer->Data_Length = sizeof(Referee_Tx_Data_Interaction_Graphic_String_t) - 2;
    tmp_buffer->Sequence = SOF;
    tmp_buffer->CRC_8 = Verify_CRC_8((uint8_t *) tmp_buffer, 4);
    tmp_buffer->Referee_Command_ID = Referee_Command_ID_INTERACTION;

    // 交互帧头
    Referee_Tx_Data_Interaction_Graphic_String_t *tmp_data = (Referee_Tx_Data_Interaction_Graphic_String_t *) tmp_buffer
            ->Data;
    tmp_data->Header = Referee_Interaction_Command_ID_UI_GRAPHIC_STRING;
    tmp_data->Sender = robot_state.robot_id;
    tmp_data->Receiver = (Enum_Referee_Data_Robots_Client_ID) ((int) (robot_state.robot_id) + 0x100);

    // UI发字符串帧内容
    tmp_data->Graphic_String = *Graphic_String;
    bzero(tmp_data->String, 30);
    strcpy((char *) tmp_data->String, String_Content);

    tmp_data->CRC_16 = Verify_CRC_16((uint8_t *) tmp_buffer, 7 + tmp_buffer->Data_Length);

    HAL_UART_Transmit(&huart1, (uint8_t *) tmp_buffer, 7 + sizeof(Referee_Tx_Data_Interaction_Graphic_String_t), 80);

    SOF++;
}

const VT13_RC_ctrl_t *get_VT13_rc_control_point(void)
{
    return &VT13_rc_ctrl;
}
