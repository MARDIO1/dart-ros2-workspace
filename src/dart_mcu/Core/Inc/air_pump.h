#ifndef DART_MCU_AIR_PUMP_H
#define DART_MCU_AIR_PUMP_H

#include "main.h"

class air_pump {
public:
  air_pump() = default;

  // active_high=true means GPIO high -> pump on.
  void begin(GPIO_TypeDef *port, uint16_t pin, bool active_high = true,
             bool default_on = false);

  void on();
  void off();
  void set(bool on);
  void toggle();
  bool isOn() const;

private:
  GPIO_TypeDef *port_ = nullptr;
  uint16_t pin_ = 0;
  bool active_high_ = true;
  bool state_on_ = false;
};

class solenoid {
public:
  solenoid() = default;

  // active_high=true means GPIO high -> valve on.
  void begin(GPIO_TypeDef *port, uint16_t pin, bool active_high = true,
             bool default_on = false);

  void on();
  void off();
  void set(bool on);
  void toggle();
  bool isOn() const;

private:
  GPIO_TypeDef *port_ = nullptr;
  uint16_t pin_ = 0;
  bool active_high_ = true;
  bool state_on_ = false;
};

namespace pneumatic {
extern air_pump main_air_pump;
extern solenoid main_solenoid[3];

void begin();
}

#endif // DART_MCU_AIR_PUMP_H
