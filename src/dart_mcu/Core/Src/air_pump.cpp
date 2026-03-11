#include "air_pump.h"

void air_pump::begin(GPIO_TypeDef *port, uint16_t pin, bool active_high,
                     bool default_on) {
  port_ = port;
  pin_ = pin;
  active_high_ = active_high;
  set(default_on);
}

void air_pump::on() { set(true); }

void air_pump::off() { set(false); }

void air_pump::set(bool on) {
  if (port_ == nullptr || pin_ == 0) {
    state_on_ = on;
    return;
  }

  state_on_ = on;
  const GPIO_PinState level =
      (on == active_high_) ? GPIO_PIN_SET : GPIO_PIN_RESET;
  HAL_GPIO_WritePin(port_, pin_, level);
}

void air_pump::toggle() { set(!state_on_); }

bool air_pump::isOn() const { return state_on_; }
