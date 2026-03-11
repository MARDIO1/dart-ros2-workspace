#ifndef __MOTOR_TYPES_H__
#define __MOTOR_TYPES_H__

#include <stdint.h>

// 电机类型枚举
typedef enum {
    MOTOR_DM4310,
} motor_type_t;

// 电机反馈数据结构
typedef struct {
    uint32_t FrameCounter;      // 帧计数器
    float RealSpeed;            // 实际速度 (rpm)
    float Current;              // 电流
    float Voltage;              // 电压
    float Mechanical_Angle;     // 机械角度
    float Temperature;          // 温度
} Motor_Feedback_Data_t;

// 电机控制结构
typedef struct {
    motor_type_t type;          // 电机类型
    uint32_t id;                // 电机ID
    uint8_t Error_id;           // 错误ID
    uint8_t Mode;               // 控制模式
    
    uint8_t Reductionratio;     // 减速比
    int16_t Realcirnum;         // 实际圈数
    float RealAngle;            // 实际角度 (°)
    float Realrotationrate;     // 实际转速 (rps)
    
    Motor_Feedback_Data_t FeedbackData; // 反馈数据
} motor_t;

#endif // __MOTOR_TYPES_H__
