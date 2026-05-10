//
// Created by cheny on 24-9-11.
//

#ifndef DART_MCU_MOTOR_H
#define DART_MCU_MOTOR_H

#include "FreeRTOS.h"
#include "can.h"
#include "inttypes.h"
#include "task.h"
#include "dm_driver.h"  // needed for DM4310 types and functions

// DM driver struct is encapsulated inside `motor_dm` instances

namespace motor {

enum E_MotorState { IDLE, RUNNING, DISCONNECTED, UNDEFINED };

enum E_MotorType { M2006, M3508, GM6020, DM4310 };

void update_can_array(uint8_t *aData, uint8_t id, int16_t output);

// brushless RM series motor implementation
class motor_rm {
public:
  // Current
  int16_t target_current_; // 0-10000

  // Velocity
  int16_t current_velocity_; // RPM
  int16_t max_current_;      // 0-10000

  // Round & Position
  int current_round_;
  int current_angle_;

  // Update Time and Disconnect check
  TickType_t last_update_time_;

  // Type
  E_MotorType motor_type_;

  // Can Interface
  uint8_t motor_id_;

  // Motor State
  E_MotorState motor_state_ = DISCONNECTED;

private:
  int current_angle_last_;
  // Motor Angle Reverse Flag
  bool angle_reverse_;
  bool first_decode_ = false;

  // Motor State Flag
  E_MotorState motor_state_next_;

public:
  void create(int16_t max_current, E_MotorType motor_type, uint8_t motor_id,
              bool angle_reserve = false) {
    max_current_ = max_current;
    angle_reverse_ = angle_reserve;
    target_current_ = 0;
    current_velocity_ = 0;
    current_round_ = 0;
    last_update_time_ = xTaskGetTickCount();
    motor_state_ = DISCONNECTED;
    motor_type_ = motor_type;
    motor_id_ = motor_id;
  }

  // Interface
  void decodeCanMsg(CAN_RxHeaderTypeDef *rxHeader, const uint8_t *rxData);

  bool setNextState(E_MotorState state);

  void resetRound() { current_round_ = 0; }

  bool setCurrent(int16_t target_current);

  int16_t updateCurrent();
};

// DM4310-specific motor wrapper
class motor_dm {
public:
  motor_dm() { initDriver(); }

  void create(uint8_t motor_id, uint8_t can_id = 1) {
    initDriver();
    motor_id_ = motor_id;
    can_id_ = can_id;
    info_.type = MOTOR_DM4310;
    info_.id = motor_id;
    info_.Reductionratio = MOTORDM4310_Reductionratio;
    info_.Error_id = 0;
    info_.Mode = 0;
    info_.Realcirnum = 0;
    info_.RealAngle = 0.0f;
    info_.Realrotationrate = 0.0f;
    info_.current_angle_with_circle_ = 0.0f;
    target_vel_radps = 0.0f;
    target_pos_rad = 0.0f;
    standard_angle_rad_ = 0.0f;
    DM_motorOpen(motor_id_, can_id_);
  }

  void open() { DM_motorOpen(motor_id_, can_id_); }

  void close() { DM_motorClose(motor_id_, can_id_); }

  void clearError() { DM_motorErrClear(motor_id_, can_id_); }

  void zeroSet() { DM_motorZeroSet(motor_id_, can_id_); }

  void speedPositionControl(float pos, float vel) {
    DM_speedpositionControl(motor_id_, can_id_, pos, vel);
  }

  void decodeCanMsg(const uint8_t *rxData) {
    DM_infoHandle(&info_, const_cast<uint8_t *>(rxData));
  }

  void setStandardAngleRad(float standard_angle_rad) {
    standard_angle_rad_ = standard_angle_rad;
  }

  void setStandardAngleDeg(float standard_angle_deg) {
    standard_angle_rad_ =
        standard_angle_deg * 3.14159265358979323846f / 180.0f;
  }

  void setpos(float pos_rad, float vel_radps) {
    target_pos_rad = pos_rad + standard_angle_rad_;
    target_vel_radps = vel_radps;
  }

  void setposDeg(float pos_deg, float vel_radps) {
    setpos(pos_deg * 3.14159265358979323846f / 180.0f, vel_radps);
  }

  float getRealAngleDeg() const { return info_.RealAngle; }

  float getAngleWithCircle() const { return info_.current_angle_with_circle_; }

  float getTargetAngleRad() const { return target_pos_rad; }

  void updatemove() { speedPositionControl(target_pos_rad, target_vel_radps); }

  void setNextState(E_MotorState state) { (void)state; }

  void setCurrent(int16_t target_current) { (void)target_current; }

  int16_t updateCurrent() { return 0; }

  uint32_t feedbackStdId() const { return DMJ_STDID + motor_id_; }

private:
  static void initDriver() {
    static bool inited = false;
    if (!inited) {
      DM_Set_CAN_Send_Function(my_can_send);
      inited = true;
    }
  }

  static void my_can_send(uint8_t CAN_ID, uint32_t stdid, uint8_t data[8]);

  uint8_t motor_id_ = 0;
  uint8_t can_id_ = 1;
  float target_vel_radps = 0.0f;
  float target_pos_rad = 0.0f;
  float standard_angle_rad_ = 0.0f;
  motor_t info_;
};

extern motor_rm MotorTriggerLS; // 扳机丝杆电机
extern motor_rm MotorYawLS;     // 偏航丝杆电机
extern motor_rm MotorLoad[2];   // 装填电机

// DM wrapper instance (optional)
extern motor_dm MotorDM4310;

} // namespace motor

#endif // DART_MCU_MOTOR_H
