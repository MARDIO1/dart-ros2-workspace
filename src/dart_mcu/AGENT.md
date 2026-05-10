# AGENT.md

## 4310 约定
- `motor::MotorDM4310` 是当前 4310 的统一入口。
- 角度校准优先走 `setStandardAngleDeg()` / `setStandardAngleRad()`。
- 下发位置优先走 `setpos()` / `setposDeg()`，再用 `updatemove()` 发包。
- 保护态只 `close()`，退出保护态要 `open()`。
- 反馈累计角度看 `getAngleWithCircle()`，原始角度看 `getRealAngleDeg()`。

## 外设 / TIM
- `APB1` 定时器时钟：`42 MHz`
- `APB2` 定时器时钟：`84 MHz`
- `TIM2`：`PA3 / TIM2_CH4`，蜂鸣器
- `TIM3`：`PB0 / TIM3_CH3`，WS2812
- `TIM4`、`TIM5`：`trigger_servo` 舵机 / 发射相关
- `TIM6`：基本定时器，当前用于 `SoundEffectManager::timer_callback`
- `TIM8`：`CH1/CH2` 用于 `velocity_meter`，`CH3` 预留/按硬件映射使用
- `TIM12`：`PH6 / TIM12_CH1`，蜂鸣器 PWM
- `TIM1`：当前废弃

## 引脚映射
- `A  PI0`
- `B  PH12` `TIM5_CH3` `trigger_servo[1]`
- `C  PH11` `TIM5_CH2` `trigger_servo[4][5]`
- `D  PH10` `TIM5_CH1` `trigger_servo[7]`
- `E  PD15` `TIM4_CH4` `trigger_servo[3]`
- `F  PD14` `TIM4_CH3` `trigger_servo[2]`
- `G  PD13` `GPIOOutput`
- `H  PD12` `TIM4_CH1` `trigger[0]`
- `Z  PI2` `GPIOOutput`
- `Y  PI7` `TIM8_CH3`
- `X  PI6` `TIM8_CH2` 测速
- `W  PI5` `TIM8_CH1` 测速
- `V  PA3` `TIM2_CH4` 连蜂鸣器
- `U  PA2` `GPIOInput` 微动开关状态
- `T  PA1` `GPIOInput` 连信号（橘线）和地（红线）读取 `YAW` 状态
- `S  PA0` `GPIOInput` 唤醒引脚

## 代码提示
- `main.cpp` 里只对匹配的 `MotorDM4310` 帧做解析，不要把未知 CAN 帧都喂给它。
- `state_machine.cpp` 里保护态要成对处理 `MotorDM4310.close()` / `open()`。
