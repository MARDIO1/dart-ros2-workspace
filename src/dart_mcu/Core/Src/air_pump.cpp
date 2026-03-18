#include "air_pump.h"
#include "stm32f4xx_hal_gpio.h"

namespace {
GPIO_TypeDef *const kAirPumpPort =
  GPIOI;
constexpr uint16_t kAirPumpPin = GPIO_PIN_2;
GPIO_TypeDef *const kSolenoidPort = GPIOH;
constexpr uint16_t kSolenoidPins[3] = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5};
} // namespace  PH4有问题？？ PH5有问题？？？？为什么PH3没有问题？？

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

namespace pneumatic {
air_pump main_air_pump;
solenoid main_solenoid[3];

void begin() {
  main_air_pump.begin(kAirPumpPort, kAirPumpPin, false, false);
  for (uint8_t i = 0; i < 3; ++i) {
    main_solenoid[i].begin(kSolenoidPort, kSolenoidPins[i], true, false);
  }
}
} // namespace pneumatic
