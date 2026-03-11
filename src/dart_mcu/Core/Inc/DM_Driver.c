#include "dm_driver.h"
#include "shit_utils.h"
#include <string.h>

// 全局CAN发送函数指针
static CAN_Send_Func_t g_can_send_func = NULL;

// 设置CAN发送函数
void DM_Set_CAN_Send_Function(CAN_Send_Func_t func) { g_can_send_func = func; }

// 内部CAN发送函数
static void dm_can_send(uint8_t CAN_ID, uint32_t stdid, uint8_t data[8]) {
  if (g_can_send_func != NULL) {
    g_can_send_func(CAN_ID, stdid, data);
  }
}

// 打开电机
void DM_motorOpen(uint8_t Motor_ID, uint8_t CAN_ID) {
  uint8_t data[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC};
  dm_can_send(CAN_ID, DMJ_STDID + Motor_ID, data);
}

// 关闭电机
void DM_motorClose(uint8_t Motor_ID, uint8_t CAN_ID) {
  uint8_t data[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD};
  dm_can_send(CAN_ID, DMJ_STDID + Motor_ID, data);
}

// 清除电机错误
void DM_motorErrClear(uint8_t Motor_ID, uint8_t CAN_ID) {
  uint8_t data[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB};
  dm_can_send(CAN_ID, DMJ_STDID + Motor_ID, data);
}

// 设置电机零点
void DM_motorZeroSet(uint8_t Motor_ID, uint8_t CAN_ID) {
  uint8_t data[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE};
  dm_can_send(CAN_ID, DMJ_STDID + Motor_ID, data);
}

// 速度位置控制（核心函数）
void DM_speedpositionControl(uint8_t Motor_ID, uint8_t CAN_ID, float _pos,
                             float _vel) {
  uint8_t data[8];

  // 限制输入范围
  LIMIT(_pos, P_MIN, P_MAX);
  LIMIT(_vel, V_MIN, V_MAX);

  // 位置已经是弧度，直接使用（移除角度转换）
  // _pos = _pos * 12.5f / 720.0f;  // 移除错误的角度转换

  // 转换为CAN数据
  int p_int = float_to_uint(_pos, P_MIN, P_MAX, 16);
  int v_int = float_to_uint(_vel, V_MIN, V_MAX, 12);

  data[0] = (p_int >> 8) & 0xFF;
  data[1] = p_int & 0xFF;
  data[2] = (v_int >> 4) & 0xFF;
  data[3] = ((v_int & 0xF) << 4) | 0x0F; // 低4位补1
  data[4] = 0xFF;                        // 扭矩设为0
  data[5] = 0xFF;
  data[6] = 0xFF;
  data[7] = 0xFF;

  dm_can_send(CAN_ID, DMJ_STDID + Motor_ID, data);
}

// 处理电机反馈数据
void DM_infoHandle(motor_t *mot, uint8_t *Data) {
  Motor_Inf_t mtr;
  float temp_angle = 0;
  int16_t temp_cirnum = 0;

  // 解析电机ID和状态
  mtr.id = Data[0] & 0x0F;
  mtr.state = Data[0] >> 4;

  if (mot->Error_id == 0) {
    mot->Error_id = mtr.state;
  }

  // 解析位置、速度、扭矩
  int p_int = (Data[1] << 8) | Data[2];
  int v_int = (Data[3] << 4) | (Data[4] >> 4);
  int t_int = ((Data[4] & 0xF) << 8) | Data[5];

  mtr.pos = uint_to_float(p_int, P_MIN, P_MAX, 16);
  mtr.vel = uint_to_float(v_int, V_MIN, V_MAX, 12);
  mtr.toq = uint_to_float(t_int, T_MIN, T_MAX, 12);

  // 温度
  mtr.Tmos = (float)Data[6];
  mtr.Tcoil = (float)Data[7];

  // 更新帧计数器
  mot->FeedbackData.FrameCounter++;

  // 更新转速 (rad/s -> rps)
  mot->Realrotationrate = mtr.vel;

  // 更新角度 (rad -> 度)
  // 现在mtr.pos已经是弧度，转换为度
  temp_angle = mtr.pos * 180.0f / 3.14159265f;

  // 处理角度循环
  while (temp_angle >= 360.0f) {
    temp_angle -= 360.0f;
    temp_cirnum += 1;
  }

  while (temp_angle < 0.0f) {
    temp_angle += 360.0f;
    temp_cirnum -= 1;
  }

  mot->Realcirnum = temp_cirnum;
  mot->RealAngle = temp_angle;

  // 更新其他反馈数据
  mot->FeedbackData.RealSpeed =
      mtr.vel * 60.0f / (2.0f * 3.14159265f); // rad/s -> rpm
  mot->FeedbackData.Current = mtr.toq;        // 假设扭矩与电流成正比
}
