//
// Created by cheny on 24-9-11.
//
#ifndef PWM_HPP
#define PWM_HPP

#include "stm32f4xx_hal.h"

class PWM {
public:
    PWM() = default;

    void
    configure(TIM_HandleTypeDef *htim, uint32_t channel, uint32_t timerClock, uint32_t period, uint32_t frequency);

    void setDutyCycle(float dutyCyclePercent);

    void setCompareValue(uint32_t compareValue);

    uint32_t getAutoReload() const;         // 查询当前重载值（即最大 Compare 值）
    uint32_t getCompareValue() const;   // 查询当前 Compare 值
    uint32_t getFrequency() const;      // 查询当前频率

    void start();

    void stop();

private:
    TIM_HandleTypeDef *htim_;
    uint32_t channel_;
    uint32_t timerClock_;

    void updateTimer();
};

#endif // PWM_HPP
