#ifndef __DM_DRIVER_H__
#define __DM_DRIVER_H__

#include "shit_motor_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// 宏定义
#define DMJ_STDID 0x00                // MIT
#define DMP_STDID 0X100
#define DMS_STDID 0X200
#define MOTORDM4310_Reductionratio 10 // DM4310减速比

// 控制参数范围
#define P_MIN -12.5f // 最小位置 (rad)
#define P_MAX 12.5f  // 最大位置 (rad)
#define V_MIN -30.0f // 最小速度 (rad/s)
#define V_MAX 30.0f  // 最大速度 (rad/s)
#define T_MIN -10.0f // 最小扭矩 (N·m)
#define T_MAX 10.0f  // 最大扭矩 (N·m)

// 电机信息结构
typedef struct {
  int id;      // 电机ID
  int state;   // 状态
  float pos;   // 位置 (rad)
  float vel;   // 速度 (rad/s)
  float toq;   // 扭矩 (N·m)
  float Tmos;  // MOS温度 (°C)
  float Tcoil; // 线圈温度 (°C)
} Motor_Inf_t;

// CAN发送回调函数类型（需要用户实现）
typedef void (*CAN_Send_Func_t)(uint8_t CAN_ID, uint32_t stdid,
                                uint8_t data[8]);

// 设置CAN发送函数（必须在初始化时调用）
void DM_Set_CAN_Send_Function(CAN_Send_Func_t func);

// 控制函数声明
void DM_motorOpen(uint8_t Motor_ID, uint8_t CAN_ID);
void DM_motorClose(uint8_t Motor_ID, uint8_t CAN_ID);
void DM_speedpositionControl(uint8_t Motor_ID, uint8_t CAN_ID, float _pos,
                             float _vel);
void DM_motorErrClear(uint8_t Motor_ID, uint8_t CAN_ID);
void DM_motorZeroSet(uint8_t Motor_ID, uint8_t CAN_ID);

// 反馈处理函数
void DM_infoHandle(motor_t *mot, uint8_t *Data);

#ifdef __cplusplus
}
#endif

#endif // __DM_DRIVER_H__
