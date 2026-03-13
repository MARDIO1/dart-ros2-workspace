//
// Created by cheny on 24-9-11.
//

#include "motor.h"
#include "dm_driver.h"  // DM4310 support

// callback stub required by motor_dm (simple CAN1 transmit)
void motor::motor_dm::my_can_send(uint8_t CAN_ID, uint32_t stdid, uint8_t data[8]) {
    CAN_TxHeaderTypeDef TxHeader;
    uint32_t TxMailbox;
    
    TxHeader.StdId = stdid;
    TxHeader.ExtId = 0;
    TxHeader.IDE = CAN_ID_STD;
    TxHeader.RTR = CAN_RTR_DATA;
    TxHeader.DLC = 8;
    TxHeader.TransmitGlobalTime = DISABLE;
    if (CAN_ID==1)
      HAL_CAN_AddTxMessage(&hcan1, &TxHeader, data, &TxMailbox);
    if(CAN_ID==2)
      HAL_CAN_AddTxMessage(&hcan2, &TxHeader, data, &TxMailbox);
}

  // global DM driver struct removed; motor_dm now owns its own motor_t member (info_)

namespace motor {
// RM motor instances
motor_rm MotorTriggerLS; // 扳机丝杆电机
motor_rm MotorYawLS;     // 偏航丝杆电机
motor_rm MotorLoad[2];   // 装填电机

// DM motor wrapper instance (if needed by user code)
motor_dm MotorDM4310;

void update_can_array(uint8_t *aData, uint8_t id, int16_t output) {
  aData[id * 2] = (uint8_t)(output >> 8);
  aData[id * 2 + 1] = (uint8_t)(output);
}

bool motor_rm::setCurrent(int16_t target_current) {
  if (target_current > max_current_)
    target_current = max_current_;
  else if (target_current < -max_current_)
    target_current = -max_current_;

  if (motor_state_ == IDLE)
    motor_state_ = (RUNNING);
  else if (motor_state_ == DISCONNECTED)
    return false;

  target_current_ = target_current;
  return true;
}

bool motor_rm::setNextState(motor::E_MotorState state) {
  if (motor_state_ != DISCONNECTED) {
    motor_state_next_ = state;
    return true;
  } else
    return false;
}

void motor_rm::decodeCanMsg(CAN_RxHeaderTypeDef *rxHeader,
                            const uint8_t *rxData) {
  // 标准帧
  if (rxHeader->IDE == CAN_ID_STD) {
    // 更新last
    current_angle_last_ = current_angle_;

    if (angle_reverse_)
      current_angle_ = 8191 - ((rxData[0] << 8) | rxData[1]);
    else
      current_angle_ = (rxData[0] << 8) | rxData[1];

    current_velocity_ = (rxData[2] << 8) | rxData[3];

    if (angle_reverse_)
      current_velocity_ = -current_velocity_;

    last_update_time_ = xTaskGetTickCount();
    if (motor_state_ == DISCONNECTED &&
        xTaskGetTickCount() - last_update_time_ < 1000) {
      motor_state_ = (IDLE);
    }

    if (!first_decode_) {
      first_decode_ = true;
      return;
    }

    // 判断是否过零
    if ((current_angle_ < 2048) && (current_angle_last_ > 6144))
      current_round_ += 1;
    else if ((current_angle_ > 6144) && (current_angle_last_ < 2048))
      current_round_ -= 1;
  }
}

/**
 * @brief 更新电机状态，返回该电机的控制电流。
 * @return
 */
int16_t motor_rm::updateCurrent() {
  // Check Disconnect
  if (xTaskGetTickCount() - last_update_time_ > 5000) {
    motor_state_ = (DISCONNECTED);
  } else if (motor_state_ == DISCONNECTED) {
    motor_state_ = (IDLE);
  }
  
  // update next_state
  if (motor_state_next_ != UNDEFINED && motor_state_ != DISCONNECTED) {
    motor_state_ = motor_state_next_;
    motor_state_next_ = UNDEFINED;
  } else if (motor_state_next_ != UNDEFINED && motor_state_ == DISCONNECTED) {
    motor_state_next_ = UNDEFINED;
  }
  
  // Return Current value
  if (motor_state_ != RUNNING) {
    target_current_ = 0;
  }
  
  return angle_reverse_ ? -target_current_ : target_current_;
}
} // namespace motor
