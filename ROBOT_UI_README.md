# 串联腿机器人UI使用指南

## 功能说明

已添加到 UI_Task.c 中的功能：
- 11个电机状态指示（通过颜色变化）
- 电机索引 0-10 对应：左摩擦轮、右摩擦轮、yaw、pitch、拨弹盘、左前髋、右前髋、左后髋、右后髋、左轮、右轮

## 电机状态颜色

| 状态 | 颜色 | 枚举值 |
|------|------|--------|
| 关闭 | 白色 | MOTOR_OFF |
| 待机 | 绿色 | MOTOR_IDLE |
| 运行 | 青色 | MOTOR_RUNNING |
| 错误 | 粉红 | MOTOR_ERROR |

## 使用方法

### 1. 设置电机状态

在需要的地方调用：
```c
Robot_UI_Set_Motor_Status(电机索引, 状态);
```

示例：
```c
Robot_UI_Set_Motor_Status(0, MOTOR_RUNNING); // 左摩擦轮运行
Robot_UI_Set_Motor_Status(1, MOTOR_RUNNING); // 右摩擦轮运行
Robot_UI_Set_Motor_Status(3, MOTOR_IDLE);    // pitch待机
```

### 2. 传入pitch角度

在 `Referee_Draw_Dynamic_UI` 函数末尾，当前传入的是 0.0f。
您可以修改为传入实际的pitch角度：

```c
// 绘制串联腿机器人UI
Robot_UI_Draw_Dynamic(UI, 0.0f);
```

改为：
```c
// 绘制串联腿机器人UI（请根据您的陀螺仪数据结构修改）
float pitch_angle = INS.angle.pitch; // 示例
Robot_UI_Draw_Dynamic(UI, pitch_angle);
```

### 3. 电机索引参考

| 索引 | 电机名称 |
|------|----------|
| 0 | 左摩擦轮 |
| 1 | 右摩擦轮 |
| 2 | yaw电机 |
| 3 | pitch电机 |
| 4 | 拨弹盘电机 |
| 5 | 左前髋关节 |
| 6 | 右前髋关节 |
| 7 | 左后髋关节 |
| 8 | 右后髋关节 |
| 9 | 左轮电机 |
| 10 | 右轮电机 |

## 注意事项

1. **pitch角度单位**：弧度（radians）
2. **pitch角度范围**：会自动限制在 -40° ~ +40° 对应的范围内
3. **图层使用**：图层7和图层8，避免与现有UI冲突
4. **更新频率**：在 Referee_Draw_Dynamic_UI 中实时更新
