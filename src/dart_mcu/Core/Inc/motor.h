//
// Created by cheny on 24-9-11.
//

#ifndef DART_MCU_MOTOR_H
#define DART_MCU_MOTOR_H

#include "FreeRTOS.h"
#include "air_pump.h"
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

    void create(uint8_t motor_id, uint8_t can_id) {
        // set up struct and open CAN
        initDriver();
        motor_id_ = motor_id;
        can_id_ = can_id;
        info_.type = MOTOR_DM4310;
        info_.id = motor_id;
        info_.Reductionratio = MOTORDM4310_Reductionratio;
        target_vel=1.0f;
        target_pos_rad=0.0f;
    }

    void open() { DM_motorOpen(motor_id_, can_id_); }

    void close() { DM_motorClose(motor_id_, can_id_); }

    void speedPositionControl(float pos, float vel) {/*RAD 另一边是电压*/DM_speedpositionControl(motor_id_, can_id_, pos, vel);}
    
    void decodeCanMsg(const uint8_t *rxData) {DM_infoHandle(&info_, const_cast<uint8_t *>(rxData));}

    void setpos(float pos_rad) { target_pos_rad = pos_rad; }

    void updatemove() { speedPositionControl(target_pos_rad, target_vel); }

    float getAngleDeg() const { return info_.RealAngle; }

    float getAngleRad() const { return info_.RealAngle * 3.14159265f / 180.0f; }

      private : static void
                initDriver() {
      static bool inited = false;
      if (!inited) {
        DM_Set_CAN_Send_Function(my_can_send);
        inited = true;
      }
    }

    static void my_can_send(uint8_t CAN_ID, uint32_t stdid, uint8_t data[8]);
    /* per-instance driver information */
    uint8_t motor_id_ = 0;
    uint8_t can_id_ = 1;
    motor_t info_;
    float target_pos_rad;
    float target_vel;
    
};

extern motor_rm MotorTriggerLS; // 扳机丝杆电机
extern motor_rm MotorYawLS;     // 偏航丝杆电机
extern motor_rm MotorLoad[2];   // 装填电机

// DM wrapper instance (optional)
extern motor_dm MotorWindmill;

} // namespace motor

#endif // DART_MCU_MOTOR_H
