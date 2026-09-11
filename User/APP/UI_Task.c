/**
  ******************************************************************************
  * @file           : UI_Task.c
  * @author         : Chen Haoran
  * @brief          : None
  * @attention      : None
  * @date           : 2026/3/30
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "UI_Task.h"
#include "Host_Comm_Task.h"
#include "Can_Comm_Task.h"
#include "cmsis_os.h"
#include "ChassisL_Task.h"

/* Define --------------------------------------------------------------------*/

/* Enum ----------------------------------------------------------------------*/

/* Variable && Struct --------------------------------------------------------*/
UI_draw_t UI_draw;
uint16_t last_allowance_bullet_count=0;
/* Function Declaration ------------------------------------------------------*/
void Referee_Draw_Static_UI(UI_draw_t *UI);

void Referee_Draw_Dynamic_UI(UI_draw_t *UI);

void UI_Feedback_Update(UI_draw_t *UI);
/* Function ------------------------------------------------------------------*/

void UI_Task(void)
{
    osDelay(6000);

    last_allowance_bullet_count=projectile_allowance.projectile_allowance_42mm;

    UI_draw.jingtai_flag = 0;
    UI_draw.dongtai_flag = 0;

    UI_draw.UI_status = false;
    UI_draw.UI_RC = get_remote_control_point();

    while (1)
    {
        UI_Feedback_Update(&UI_draw);

        if (UI_draw.UI_status == false)
        {
            Set_Referee_UI_Change_Flag_Clear();
            Referee_Draw_Static_UI(&UI_draw);
            UI_draw.UI_status = true;
            UI_draw.jingtai_flag++;
        }
        else
        {
            Referee_Draw_Dynamic_UI(&UI_draw);
            UI_draw.dongtai_flag++;
        }

        osDelay(1);
    }
}

void UI_Feedback_Update(UI_draw_t *UI)
{
    if (Host_communication.Key_B.key_state == true)
    {
        UI->UI_status = false;
    }

    UI->robot_ui.motor_fric_left_status = Can_Comm.Can_Motor_Status_Data.Rx_Data.left_fric_motor_status;
    UI->robot_ui.motor_fric_right_status = Can_Comm.Can_Motor_Status_Data.Rx_Data.right_fric_motor_status;
    UI->robot_ui.motor_yaw_status = Can_Comm.Can_Motor_Status_Data.Rx_Data.yaw_motor_status;
    UI->robot_ui.motor_pitch_status = Can_Comm.Can_Motor_Status_Data.Rx_Data.pitch_motor_status;
    UI->robot_ui.motor_trigger_status = Can_Comm.Can_Motor_Status_Data.Rx_Data.trigger_motor_status;

    UI->robot_ui.motor_Joint_1_status = chassis_move.joint_motor_status[0];
    UI->robot_ui.motor_Joint_2_status = chassis_move.joint_motor_status[1];
    UI->robot_ui.motor_Joint_3_status = chassis_move.joint_motor_status[2];
    UI->robot_ui.motor_Joint_4_status = chassis_move.joint_motor_status[3];

    UI->robot_ui.motor_wheel_l_status = chassis_move.wheel_motor_status[0];
    UI->robot_ui.motor_wheel_r_status = chassis_move.wheel_motor_status[1];

    // 更新已发射子弹数量
    if (projectile_allowance.projectile_allowance_42mm < last_allowance_bullet_count)
    {
        UI_draw.already_bullet_count++;
    }

    last_allowance_bullet_count=projectile_allowance.projectile_allowance_42mm;
}

void Referee_Draw_Static_UI(UI_draw_t *UI)
{
    // 底盘预警左线
    UI->graphic_chassis_warning_left_line = Set_Referee_UI_Line(0, 0, Referee_Data_Interaction_Graphic_Color_ORANGE, 5,
                                                                400, 0, 800, 540);
    // 底盘预警右线
    UI->graphic_chassis_warning_right_line = Set_Referee_UI_Line(0, 1, Referee_Data_Interaction_Graphic_Color_ORANGE, 5,
                                                                 1550, 0, 1150, 540);
    // 发射管瞄准竖线
    UI->graphic_booster_aiming_straight_line = Set_Referee_UI_Line(0, 2, Referee_Data_Interaction_Graphic_Color_MAIN, 3,
                                                                   960, 200, 960, 500);
    // // 射击瞄准横线
    // UI->graphic_booster_near_line = Set_Referee_UI_Line(0, 3, Referee_Data_Interaction_Graphic_Color_MAIN, 2, 975 - 140,
    //                                                     400, 975 + 140, 400);
    // 中间瞄准矩形
    UI->graphic_booster_middle_rectangle = Set_Referee_UI_Rectangle(0, 4, Referee_Data_Interaction_Graphic_Color_MAIN,
                                                                    2, 940, 520, 980, 560);

    Referee_UART_Send_Interaction_UI_Graphic_2(UI->graphic_chassis_warning_left_line,
                                           UI->graphic_chassis_warning_right_line, UI->Tx_buffer);
    // 准备下一帧
    osDelay(34);

    Referee_UART_Send_Interaction_UI_Graphic_2(UI->graphic_booster_aiming_straight_line,
                                           UI->graphic_booster_middle_rectangle, UI->Tx_buffer);
    // 准备下一帧
    osDelay(34);



    // 超级电容矩形
    UI->graphic_super_cap_rectangle = Set_Referee_UI_Rectangle(0, 5, Referee_Data_Interaction_Graphic_Color_GREEN, 2,
                                                               760, 210, 760 + 380, 180);
    Referee_UART_Send_Interaction_UI_Graphic_1(UI->graphic_super_cap_rectangle, UI->Tx_buffer);
    // 准备下一帧
    osDelay(34);

    UI->graphic_chassis_mod_string_zero = Set_Referee_UI_String(1, 0, Referee_Data_Interaction_Graphic_Color_WHITE, 3, 1720, 620, 30, 30);
    Referee_UART_Send_Interaction_UI_Graphic_String(UI->graphic_chassis_mod_string_zero, "idle", UI->Tx_buffer);
    // 准备下一帧
    osDelay(34);

    UI->graphic_chassis_mod_string_foll = Set_Referee_UI_String(1, 1, Referee_Data_Interaction_Graphic_Color_WHITE, 3, 1720, 540, 30, 30);
    Referee_UART_Send_Interaction_UI_Graphic_String(UI->graphic_chassis_mod_string_foll, "foll", UI->Tx_buffer);
    // 准备下一帧
    osDelay(34);

    UI->graphic_chassis_mod_string_spin = Set_Referee_UI_String(1, 2, Referee_Data_Interaction_Graphic_Color_WHITE, 3, 1720, 460, 30, 30);
    Referee_UART_Send_Interaction_UI_Graphic_String(UI->graphic_chassis_mod_string_spin, "spin", UI->Tx_buffer);
    // 准备下一帧
    osDelay(34);

    UI->graphic_gimbal_auto_aim_string_armr = Set_Referee_UI_String(1, 3, Referee_Data_Interaction_Graphic_Color_WHITE, 2, 700, 800, 20, 30);
    Referee_UART_Send_Interaction_UI_Graphic_String(UI->graphic_gimbal_auto_aim_string_armr, "armr", UI->Tx_buffer);
    // 准备下一帧
    osDelay(34);

    UI->graphic_gimbal_auto_aim_string_srun = Set_Referee_UI_String(1, 4, Referee_Data_Interaction_Graphic_Color_WHITE, 2, 950-35, 800, 20, 30);
    Referee_UART_Send_Interaction_UI_Graphic_String(UI->graphic_gimbal_auto_aim_string_srun, "srun", UI->Tx_buffer);
    // 准备下一帧
    osDelay(34);

    UI->graphic_gimbal_auto_aim_string_lrun = Set_Referee_UI_String(1, 5, Referee_Data_Interaction_Graphic_Color_WHITE, 2, 1200-70, 800, 20, 30);
    Referee_UART_Send_Interaction_UI_Graphic_String(UI->graphic_gimbal_auto_aim_string_lrun, "lrun", UI->Tx_buffer);
    // 准备下一帧
    osDelay(34);

    UI->graphic_body_low_line = Set_Referee_UI_Line(2, 0, Referee_Data_Interaction_Graphic_Color_ORANGE, 2, body_line_start_x, body_low+40, body_line_start_x+120, body_low+40);
    UI->graphic_body_mid_line = Set_Referee_UI_Line(2, 1, Referee_Data_Interaction_Graphic_Color_ORANGE, 2, body_line_start_x, body_mid+10, body_line_start_x+120, body_mid+10);
    UI->graphic_body_heigh_line = Set_Referee_UI_Line(2, 2, Referee_Data_Interaction_Graphic_Color_ORANGE, 2, body_line_start_x, body_heigh-30, body_line_start_x+120, body_heigh-30);
    UI->graphic_left_theta_circle = Set_Referee_UI_Circle(2,3,Referee_Data_Interaction_Graphic_Color_ORANGE, 2, theta_l_start_x, theta_l_start_y, 10);
    UI->graphic_right_theta_circle = Set_Referee_UI_Circle(2,4,Referee_Data_Interaction_Graphic_Color_ORANGE, 2, theta_r_start_x, theta_r_start_y, 10);
    // 发送当前五个图形
    Referee_UART_Send_Interaction_UI_Graphic_5(UI->graphic_body_low_line,
                                               UI->graphic_body_mid_line,
                                               UI->graphic_body_heigh_line, UI->graphic_left_theta_circle,
                                               UI->graphic_right_theta_circle, UI->Tx_buffer);
    // 准备下一帧
    osDelay(34);


    // //8米5吊射准线
    // UI->graphic_8m5_line = Set_Referee_UI_Line(2, 5, Referee_Data_Interaction_Graphic_Color_MAIN, 3,
    //                                                                960-70, 345, 960+70, 345);
    // //9米5吊射准线
    // UI->graphic_9m5_line = Set_Referee_UI_Line(2, 6, Referee_Data_Interaction_Graphic_Color_MAIN, 3,
    //                                                                960-70, 310, 960+70, 310);
    //
    // Referee_UART_Send_Interaction_UI_Graphic_2(UI->graphic_8m5_line,
    //                                            UI->graphic_9m5_line, UI->Tx_buffer);
    // // 准备下一帧
    // osDelay(34);

    //5米射击准线
    UI->graphic_5m_line = Set_Referee_UI_Line(2, 7, Referee_Data_Interaction_Graphic_Color_MAIN, 3,
                                                                   960-70, 490, 960+70, 490);

    Referee_UART_Send_Interaction_UI_Graphic_1(UI->graphic_5m_line,UI->Tx_buffer);
    // 准备下一帧
    osDelay(34);



    // 绘制串联腿机器人静态UI
    Robot_UI_Draw_Static(UI);
}

void Referee_Draw_Dynamic_UI(UI_draw_t *UI)
{
    // 底盘功率限制整数
    UI->graphic_chassis_power_integer = Set_Referee_UI_Integer(4, 0, Referee_Data_Interaction_Graphic_Color_PURPLE, 5,
                                                               1700, 750, 40, robot_state.chassis_power_limit);

    // 云台YAW弧线
    static float arc_begin = 0, arc_end = 0;
    arc_end = (-Host_communication.Tx_data.motor_yaw_angle + 5.05f ) + PI / 12.0f ;
    arc_begin = (-Host_communication.Tx_data.motor_yaw_angle + 5.05f ) - PI / 12.0f ;
    arc_end = (normalizeAngleToPi_Robust(arc_end) + PI) * 180 / PI;
    arc_begin = (normalizeAngleToPi_Robust(arc_begin) + PI) * 180 / PI;
    UI->graphic_gimbal_yaw_arc = Set_Referee_UI_Arc(4, 1, Referee_Data_Interaction_Graphic_Color_PINK, 7, 960, 540,
                                                    (int) arc_begin, (int) arc_end, 90, 90);

    // 超级电容能量显示矩形
    UI->graphic_super_cap_energy_rectangle = Set_Referee_UI_Rectangle(
        4, 2, Referee_Data_Interaction_Graphic_Color_GREEN, 20, 765, 195,
        765 + (int)(13.214f*chassis_move.Super_Cap_Rx.cap_volt), 195);

    // 超级电容电压显示浮点
    UI->graphic_super_cap_vol_float = Set_Referee_UI_Float(4, 3, Referee_Data_Interaction_Graphic_Color_GREEN, 3, 765+270,
                                                           155, 30, chassis_move.Super_Cap_Rx.cap_volt);

    // 自瞄范围矩形
    if (UI->vision_flag == 0)
    {
        UI->graphic_auto_aim_range_rectangle = Set_Referee_UI_Rectangle(4, 4, Referee_Data_Interaction_Graphic_Color_WHITE,
                                                                        2, 700, 340, 1200, 750);
    }
    else if (UI->vision_flag == 1)
    {
        UI->graphic_auto_aim_range_rectangle = Set_Referee_UI_Rectangle(4, 4, Referee_Data_Interaction_Graphic_Color_GREEN,
                                                                        2, 700, 340, 1200, 750);
    }
    else if (UI->vision_flag == 2)
    {
        UI->graphic_auto_aim_range_rectangle = Set_Referee_UI_Rectangle(4, 4, Referee_Data_Interaction_Graphic_Color_PURPLE,
                                                                        2, 700, 340, 1200, 750);
    }

    Referee_UART_Send_Interaction_UI_Graphic_5(UI->graphic_chassis_power_integer, UI->graphic_gimbal_yaw_arc,
                                               UI->graphic_super_cap_energy_rectangle, UI->graphic_super_cap_vol_float,
                                               UI->graphic_auto_aim_range_rectangle, UI->Tx_buffer);
    // 准备下一帧
    osDelay(34);


    // Referee_UART_Send_Interaction_UI_Graphic_2(UI->graphic_chassis_power_integer,
    //                                        UI->graphic_gimbal_yaw_arc,UI->Tx_buffer);
    // // 准备下一帧
    // osDelay(34);
    //
    // Referee_UART_Send_Interaction_UI_Graphic_2(UI->graphic_super_cap_energy_rectangle,
    //                                        UI->graphic_super_cap_vol_float,UI->Tx_buffer);
    // // 准备下一帧
    // osDelay(34);


    //底盘模式：根据力矩不同，选择不同的矩形，小数字表示半角参考位置
    if (Host_communication.Rx_data.wheel_torque[0] == 0 &&
        Host_communication.Rx_data.wheel_torque[1] == 0)
    {
        UI->graphic_chassis_mod_rectangle = Set_Referee_UI_Rectangle(3,0, Referee_Data_Interaction_Graphic_Color_YELLOW,4,1700, 640, 1700+150, 640-70);
    }
    else
    {
        if (Host_communication.Rx_data.spin_flag == 0)
        {
            UI->graphic_chassis_mod_rectangle = Set_Referee_UI_Rectangle(3,0, Referee_Data_Interaction_Graphic_Color_YELLOW,4,1700, 560, 1700+150, 560-70);
        }
        else if (Host_communication.Rx_data.spin_flag == 1)
        {
            UI->graphic_chassis_mod_rectangle = Set_Referee_UI_Rectangle(3,0, Referee_Data_Interaction_Graphic_Color_YELLOW,4,1700, 480, 1700+150, 480-70);
        }
    }

    //摩擦轮标志：根据标志选择不同的摩擦轮颜色
    if (UI->fric_flag == 1)
    {
        UI->graphic_auto_aim_mod_rectangle = Set_Referee_UI_Rectangle(3, 1, Referee_Data_Interaction_Graphic_Color_YELLOW, 4, 680, 815, 680+110, 815-45);
    }
    else
    {
        UI->graphic_auto_aim_mod_rectangle = Set_Referee_UI_Rectangle(3, 1, Referee_Data_Interaction_Graphic_Color_WHITE, 4, 680, 815, 680+110, 815-45);
    }
    
    Referee_UART_Send_Interaction_UI_Graphic_2(UI->graphic_chassis_mod_rectangle,
                                               UI->graphic_auto_aim_mod_rectangle,UI->Tx_buffer);
    // 准备下一帧
    osDelay(34);

    //拨弹盘供弹状态
    UI->graphic_trigger_status = Set_Referee_UI_String(5, 0, Referee_Data_Interaction_Graphic_Color_CYAN, 3, 1650, 400, 20, 30);

    if (UI->trigger_status == TRIGGER_NORMAL)
    {
        Referee_UART_Send_Interaction_UI_Graphic_String(UI->graphic_trigger_status, "trig_normal", UI->Tx_buffer);
    }
    else if (UI->trigger_status == TRIGGER_GONGDAN)
    {
        Referee_UART_Send_Interaction_UI_Graphic_String(UI->graphic_trigger_status, "trig_gongdan", UI->Tx_buffer);
    }
    else if (UI->trigger_status == TRIGGER_HUIBO)
    {
        Referee_UART_Send_Interaction_UI_Graphic_String(UI->graphic_trigger_status, "trig_huibo", UI->Tx_buffer);
    }
    // 准备下一帧
    osDelay(34);



    //机器人控制状态
    UI->graphic_robot_control_status = Set_Referee_UI_String(5, 1, Referee_Data_Interaction_Graphic_Color_ORANGE, 3, 1650, 360, 20, 30);

    if (Can_Comm.Can_Control_Data.Rx_Data.robot_control_status==No_Control)
    {
        Referee_UART_Send_Interaction_UI_Graphic_String(UI->graphic_robot_control_status, "no_control", UI->Tx_buffer);
    }
    else if (Can_Comm.Can_Control_Data.Rx_Data.robot_control_status==Remote_Control)
    {
        Referee_UART_Send_Interaction_UI_Graphic_String(UI->graphic_robot_control_status, "rc_control", UI->Tx_buffer);
    }
    else if (Can_Comm.Can_Control_Data.Rx_Data.robot_control_status==Key_Mouse_Control)
    {
        Referee_UART_Send_Interaction_UI_Graphic_String(UI->graphic_robot_control_status, "key_control", UI->Tx_buffer);
    }
    // 准备下一帧
    osDelay(34);


    //右上角显示摩擦轮转速设定平均值
    int32_t fric_speed_set_average = UI->fric_speed_set_average;
    UI->graphic_fric_speed_set_average=Set_Referee_UI_Integer(5, 2, Referee_Data_Interaction_Graphic_Color_CYAN, 3, 1700, 850, 40, fric_speed_set_average);
    Referee_UART_Send_Interaction_UI_Graphic_1(UI->graphic_fric_speed_set_average, UI->Tx_buffer);
    // 准备下一帧
    osDelay(34);


    //pitch浮点数数据显示
    UI->graphic_pitch_data=Set_Referee_UI_Float(5, 3, Referee_Data_Interaction_Graphic_Color_CYAN, 3, 1500, 780, 30, UI_draw.pitch_angle);
    Referee_UART_Send_Interaction_UI_Graphic_1(UI->graphic_pitch_data, UI->Tx_buffer);
    // 准备下一帧
    osDelay(34);


    // 腿长显示
    float tmp_l = body_low + ((chassis_move.left_leg.L0-0.08f) / 0.35) * (body_heigh+30 - body_low);
    float tmp_r = body_low + ((chassis_move.right_leg.L0-0.082f) / 0.35) * (body_heigh+30 - body_low);
    UI->graphic_left_leg_L_rectangle = Set_Referee_UI_Rectangle(3, 2, Referee_Data_Interaction_Graphic_Color_GREEN, 15, body_line_start_x+25, body_low+8,body_line_start_x + 35, (int)tmp_l-8);
    UI->graphic_right_leg_L_rectangle = Set_Referee_UI_Rectangle(3, 3, Referee_Data_Interaction_Graphic_Color_GREEN, 15, body_line_start_x + 80, body_low+8,body_line_start_x + 90, (int)tmp_r-8);

    Referee_UART_Send_Interaction_UI_Graphic_2(UI->graphic_left_leg_L_rectangle,
                                                   UI->graphic_right_leg_L_rectangle,UI->Tx_buffer);
    // 准备下一帧
    osDelay(34);

    // 绘制串联腿机器人UI
    Robot_UI_Draw_Dynamic(UI);
}

Enum_Referee_Data_Interaction_Graphic_Color get_motor_color(Motor_Status_t status) {
    switch(status) {
        case Motor_Status_OFFLINE:     return Referee_Data_Interaction_Graphic_Color_BLACK;
        case Motor_Status_NORMAL:    return Referee_Data_Interaction_Graphic_Color_GREEN;
        case Motor_Status_LIGHT_STALL: return Referee_Data_Interaction_Graphic_Color_CYAN;
        case Motor_Status_HEAVY_STALL: return Referee_Data_Interaction_Graphic_Color_PINK;
        default:            return Referee_Data_Interaction_Graphic_Color_BLACK;
    }
}

void Robot_UI_Draw_Static(UI_draw_t *UI) {

    const uint16_t center_x = ROBOT_UI_CENTER_X;
    const uint16_t center_y = ROBOT_UI_CENTER_Y;

    //机器人车身
    UI->robot_ui.chassis_frame = Set_Referee_UI_Rectangle(7, 0, Referee_Data_Interaction_Graphic_Color_CYAN, 3, center_x-85, center_y, center_x+85, center_y+50);
    //机器人pitch连杆
    UI->robot_ui.pitch_link = Set_Referee_UI_Line(7, 1, Referee_Data_Interaction_Graphic_Color_CYAN, 6, center_x, center_y+70, center_x-40, center_y+120);
    //机器人pitch转轴
    UI->robot_ui.pitch_rotation_point = Set_Referee_UI_Circle(7, 2, Referee_Data_Interaction_Graphic_Color_CYAN, 2, center_x-40, center_y+120, 7);
    //机器人左腿转轴
    UI->robot_ui.left_leg_rotation_point = Set_Referee_UI_Circle(7, 3, Referee_Data_Interaction_Graphic_Color_CYAN, 2, center_x-70, center_y-35, 5);
    //机器人右腿转轴
    UI->robot_ui.right_leg_rotation_point = Set_Referee_UI_Circle(7, 4, Referee_Data_Interaction_Graphic_Color_CYAN, 2, center_x, center_y-35, 5);

    Referee_UART_Send_Interaction_UI_Graphic_5(UI->robot_ui.chassis_frame,UI->robot_ui.pitch_link,UI->robot_ui.pitch_rotation_point,UI->robot_ui.left_leg_rotation_point,UI->robot_ui.right_leg_rotation_point, UI->Tx_buffer);
    // 准备下一帧
    osDelay(34);
}

void Robot_UI_Draw_Dynamic(UI_draw_t *UI) {

    const uint16_t center_x = ROBOT_UI_CENTER_X;
    const uint16_t center_y = ROBOT_UI_CENTER_Y;

    const int16_t pivot_x = 150;
    const int16_t pivot_y = 770;
    const int16_t gun_length = 100;
    const int16_t left_fric_offset = 70;
    const int16_t right_fric_offset = 90;

    static uint32_t gun_end_x=0;
    static uint32_t gun_end_y=0;
    static uint32_t fric_left_x=0;
    static uint32_t fric_left_y=0;
    static uint32_t fric_right_x=0;
    static uint32_t fric_right_y=0;

    float sin_pitch = sinf(UI_draw.pitch_angle);
    float cos_pitch = cosf(UI_draw.pitch_angle);

    // 计算线前端坐标
    gun_end_x = pivot_x + gun_length * cos_pitch;
    gun_end_y = pivot_y + gun_length * sin_pitch;

    // 计算摩擦轮圆心（垂直于枪管方向）
    fric_left_x  = pivot_x + left_fric_offset * cos_pitch;
    fric_left_y  = pivot_y + left_fric_offset * sin_pitch;

    fric_right_x = pivot_x + right_fric_offset * cos_pitch;
    fric_right_y = pivot_y + right_fric_offset * sin_pitch;

    //机器人pitch轴电机
    UI->robot_ui.motor_pitch = Set_Referee_UI_Line(7, 5, get_motor_color(UI->robot_ui.motor_pitch_status), 6, center_x-40, center_y+120, gun_end_x, gun_end_y);
    //机器人左摩擦轮电机
    UI->robot_ui.motor_fric_left = Set_Referee_UI_Circle(7, 6, get_motor_color(UI->robot_ui.motor_fric_left_status), 2, fric_left_x, fric_left_y, 10);
    //机器人右摩擦轮电机
    UI->robot_ui.motor_fric_right = Set_Referee_UI_Circle(7, 7, get_motor_color(UI->robot_ui.motor_fric_right_status), 2, fric_right_x, fric_right_y, 10);


    //机器人髋关节1电机
    UI->robot_ui.motor_Joint_1 = Set_Referee_UI_Line(8, 0, get_motor_color(UI->robot_ui.motor_Joint_1_status), 4, center_x-70, center_y-35, center_x-30, center_y-80);
    //机器人髋关节4电机
    UI->robot_ui.motor_Joint_4 = Set_Referee_UI_Line(8, 1, get_motor_color(UI->robot_ui.motor_Joint_4_status), 4, center_x, center_y-35, center_x+40, center_y-80);
    //机器人髋关节2电机
    UI->robot_ui.motor_Joint_2 = Set_Referee_UI_Line(8, 2, get_motor_color(UI->robot_ui.motor_Joint_2_status), 4,center_x-35, center_y, center_x-70, center_y-35);
    //机器人髋关节3电机
    UI->robot_ui.motor_Joint_3 = Set_Referee_UI_Line(8, 3, get_motor_color(UI->robot_ui.motor_Joint_3_status), 4, center_x+35, center_y, center_x, center_y-35);
    //机器人左轮电机
    UI->robot_ui.motor_wheel_l = Set_Referee_UI_Circle(8, 4, get_motor_color(UI->robot_ui.motor_wheel_l_status), 4, center_x-30, center_y-80, 14);
    //机器人右轮电机
    UI->robot_ui.motor_wheel_r = Set_Referee_UI_Circle(8, 5, get_motor_color(UI->robot_ui.motor_wheel_r_status), 4, center_x+40, center_y-80, 14);


    //机器人拨弹盘电机
    UI->robot_ui.motor_trigger = Set_Referee_UI_Rectangle(8, 6, get_motor_color(UI->robot_ui.motor_trigger_status), 2, center_x-35, center_y, center_x+35, center_y+20);
    //机器人yaw轴电机
    UI->robot_ui.motor_yaw = Set_Referee_UI_Rectangle(8, 7, get_motor_color(UI->robot_ui.motor_yaw_status), 2, center_x-25, center_y+50, center_x+25, center_y+70);


    Referee_UART_Send_Interaction_UI_Graphic_2(UI->robot_ui.motor_fric_left,UI->robot_ui.motor_fric_right, UI->Tx_buffer);
    // 准备下一帧
    osDelay(34);

    Referee_UART_Send_Interaction_UI_Graphic_2(UI->robot_ui.motor_yaw,UI->robot_ui.motor_pitch, UI->Tx_buffer);
    // 准备下一帧
    osDelay(34);

    Referee_UART_Send_Interaction_UI_Graphic_7(UI->robot_ui.motor_trigger,UI->robot_ui.motor_Joint_1,UI->robot_ui.motor_Joint_2,UI->robot_ui.motor_Joint_3,UI->robot_ui.motor_Joint_4,UI->robot_ui.motor_wheel_l,UI->robot_ui.motor_wheel_r, UI->Tx_buffer);
    // 准备下一帧
    osDelay(34);

}
