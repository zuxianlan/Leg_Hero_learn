/**
  ******************************************************************************
  * @file           : UI_Task.h
  * @author         : Chen Haoran
  * @brief          : None
  * @attention      : None
  * @date           : 2026/3/30
  ******************************************************************************
  */
#ifndef CTRLBOARD_H7_IMU_UI_TASK_H
#define CTRLBOARD_H7_IMU_UI_TASK_H
/* Includes ------------------------------------------------------------------*/
#include "Transmission_link.h"
#include "referee.h"
#include "remote_control.h"
#include "stdbool.h"
#include "ChassisL_Task.h"
/* Define --------------------------------------------------------------------*/
#define ROBOT_UI_CENTER_X 190
#define ROBOT_UI_CENTER_Y 650

#define follow_yaw (6.2f)
#define body_low 10
#define body_mid 80
#define body_heigh 160
#define body_line_start_x 600
#define theta_l_start_x 1250
#define theta_l_start_y 100
#define theta_r_start_x 1350
#define theta_r_start_y 100
/* Enum ----------------------------------------------------------------------*/

/* Variable && Struct --------------------------------------------------------*/
typedef struct {
    Motor_Status_t motor_fric_left_status;
    Motor_Status_t motor_fric_right_status;
    Motor_Status_t motor_yaw_status;
    Motor_Status_t motor_pitch_status;
    Motor_Status_t motor_trigger_status;
    Motor_Status_t motor_Joint_1_status;
    Motor_Status_t motor_Joint_2_status;
    Motor_Status_t motor_Joint_3_status;
    Motor_Status_t motor_Joint_4_status;
    Motor_Status_t motor_wheel_l_status;
    Motor_Status_t motor_wheel_r_status;

    Referee_Data_Interaction_Graphic_Config_t *motor_fric_left;
    Referee_Data_Interaction_Graphic_Config_t *motor_fric_right;
    Referee_Data_Interaction_Graphic_Config_t *motor_yaw;
    Referee_Data_Interaction_Graphic_Config_t *motor_pitch;
    Referee_Data_Interaction_Graphic_Config_t *motor_trigger;
    Referee_Data_Interaction_Graphic_Config_t *motor_Joint_1;
    Referee_Data_Interaction_Graphic_Config_t *motor_Joint_2;
    Referee_Data_Interaction_Graphic_Config_t *motor_Joint_3;
    Referee_Data_Interaction_Graphic_Config_t *motor_Joint_4;
    Referee_Data_Interaction_Graphic_Config_t *motor_wheel_l;
    Referee_Data_Interaction_Graphic_Config_t *motor_wheel_r;

    Referee_Data_Interaction_Graphic_Config_t *chassis_frame;
    Referee_Data_Interaction_Graphic_Config_t *pitch_link;
    Referee_Data_Interaction_Graphic_Config_t *pitch_rotation_point;
    Referee_Data_Interaction_Graphic_Config_t *left_leg_rotation_point;
    Referee_Data_Interaction_Graphic_Config_t *right_leg_rotation_point;

} Robot_UI_t;

/* Type Forward Declaration --------------------------------------------------*/
typedef struct UI_draw_t UI_draw_t;

/* Function Declaration ------------------------------------------------------*/
void Robot_UI_Draw_Static(UI_draw_t *UI);
void Robot_UI_Draw_Dynamic(UI_draw_t *UI);
Enum_Referee_Data_Interaction_Graphic_Color get_motor_color(Motor_Status_t status);

/* Function ------------------------------------------------------------------*/

typedef struct UI_draw_t
{
  bool UI_status;

  uint16_t already_bullet_count;
  float pitch_angle;
  uint8_t vision_flag;
  uint8_t fric_flag;
  uint8_t gimbal_flag;
  uint8_t auto_aim_flag;
  uint8_t trigger_status;
  uint16_t fric_speed_set_average;

  uint8_t Tx_buffer[512];

  const RC_ctrl_t *UI_RC;

  //机器人UI
  Robot_UI_t robot_ui;

  // 静态UI
  // 底盘预警左线
  Referee_Data_Interaction_Graphic_Config_t *graphic_chassis_warning_left_line;
  // 底盘预警右线
  Referee_Data_Interaction_Graphic_Config_t *graphic_chassis_warning_right_line;
  // 发射管瞄准竖线
  Referee_Data_Interaction_Graphic_Config_t *graphic_booster_aiming_straight_line;
  // 射击瞄准横线
  Referee_Data_Interaction_Graphic_Config_t *graphic_booster_near_line;
  // 中间瞄准矩形
  Referee_Data_Interaction_Graphic_Config_t *graphic_booster_middle_rectangle;
  // 云台视觉指示直线
  Referee_Data_Interaction_Graphic_Config_t *graphic_gimbal_vision_line;
  // 裁判系统状态圆形
  Referee_Data_Interaction_Graphic_Config_t *graphic_referee_status_circle;
  // 裁判系统颜色状态圆形
  Referee_Data_Interaction_Graphic_Config_t *graphic_referee_color_status_circle;
  // 裁判系统获得底盘圆形
  Referee_Data_Interaction_Graphic_Config_t *graphic_referee_chassis_status_circle;
  // 裁判系统获得发射圆形
  Referee_Data_Interaction_Graphic_Config_t *graphic_referee_booster_status_circle;
  // 裁判系统颜色字符串
  Referee_Data_Interaction_Graphic_Config_t *graphic_ref_color_chs_bst_lv_string;
  // 超级电容矩形
  Referee_Data_Interaction_Graphic_Config_t *graphic_super_cap_rectangle;
  // 底盘模式字符串
  Referee_Data_Interaction_Graphic_Config_t *graphic_chassis_mod_string_zero;
  Referee_Data_Interaction_Graphic_Config_t *graphic_chassis_mod_string_foll;
  Referee_Data_Interaction_Graphic_Config_t *graphic_chassis_mod_string_spin;
  // 云台自动瞄准模式字符串
  Referee_Data_Interaction_Graphic_Config_t *graphic_gimbal_auto_aim_string_armr;
  Referee_Data_Interaction_Graphic_Config_t *graphic_gimbal_auto_aim_string_srun;
  Referee_Data_Interaction_Graphic_Config_t *graphic_gimbal_auto_aim_string_lrun;

  // 动态UI
  // 底盘功率限制整数
  Referee_Data_Interaction_Graphic_Config_t *graphic_chassis_power_integer;
  // 云台YAW弧线
  Referee_Data_Interaction_Graphic_Config_t *graphic_gimbal_yaw_arc;
  // 超级电容能量矩形
  Referee_Data_Interaction_Graphic_Config_t *graphic_super_cap_energy_rectangle;
  // 超级电容电压浮点
  Referee_Data_Interaction_Graphic_Config_t *graphic_super_cap_vol_float;
  // 自瞄范围矩形
  Referee_Data_Interaction_Graphic_Config_t *graphic_auto_aim_range_rectangle;
  // 底盘模式选择矩形
  Referee_Data_Interaction_Graphic_Config_t *graphic_chassis_mod_rectangle;
  // 自瞄模式选择矩形
  Referee_Data_Interaction_Graphic_Config_t *graphic_auto_aim_mod_rectangle;
  // 摩擦轮转速设定平均值
  Referee_Data_Interaction_Graphic_Config_t *graphic_fric_speed_set_average;
  // 扳机状态
  Referee_Data_Interaction_Graphic_Config_t *graphic_trigger_status;
  // 机器人控制状态
  Referee_Data_Interaction_Graphic_Config_t *graphic_robot_control_status;

    // 腿长标尺直线
    Referee_Data_Interaction_Graphic_Config_t *graphic_body_heigh_line;
    Referee_Data_Interaction_Graphic_Config_t *graphic_body_mid_line;
    Referee_Data_Interaction_Graphic_Config_t *graphic_body_low_line;

    // 腿长显示矩形
    Referee_Data_Interaction_Graphic_Config_t *graphic_left_leg_L_rectangle;
    Referee_Data_Interaction_Graphic_Config_t *graphic_right_leg_L_rectangle;

    // 腿部theta转动中心圆
    Referee_Data_Interaction_Graphic_Config_t *graphic_left_theta_circle;
    Referee_Data_Interaction_Graphic_Config_t *graphic_right_theta_circle;

    //pitch浮点数数据显示
    Referee_Data_Interaction_Graphic_Config_t *graphic_pitch_data;

    //9米5吊射准线
    Referee_Data_Interaction_Graphic_Config_t *graphic_9m5_line;
    //8米5吊射准线
    Referee_Data_Interaction_Graphic_Config_t *graphic_8m5_line;
    //5米射击准线
    Referee_Data_Interaction_Graphic_Config_t *graphic_5m_line;

    uint16_t jingtai_flag;
    uint16_t dongtai_flag;
} UI_draw_t;


extern UI_draw_t UI_draw;


#endif //CTRLBOARD_H7_IMU_UI_TASK_H
