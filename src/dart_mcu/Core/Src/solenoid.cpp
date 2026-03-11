#include "solenoid.h"

void solenoid::begin(GPIO_TypeDef *port, uint16_t pin, bool active_high,
                     bool default_on) {
  port_ = port;
  pin_ = pin;
  active_high_ = active_high;
  set(default_on);
}

void solenoid::on() { set(true); }

void solenoid::off() { set(false); }

void solenoid::set(bool on) {
  if (port_ == nullptr || pin_ == 0) {
    state_on_ = on;
    return;
  }

  state_on_ = on;
  const GPIO_PinState level =
      (on == active_high_) ? GPIO_PIN_SET : GPIO_PIN_RESET;
  HAL_GPIO_WritePin(port_, pin_, level);
}

void solenoid::toggle() { set(!state_on_); }

bool solenoid::isOn() const { return state_on_; }
