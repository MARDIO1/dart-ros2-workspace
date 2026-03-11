#ifndef __UTILS_H__
#define __UTILS_H__

#include <math.h>
#include <stdint.h>

// 数值限制宏（修复：添加赋值操作）
#define LIMIT(x, min, max)                                                     \
  do {                                                                         \
    if ((x) > (max))                                                           \
      (x) = (max);                                                             \
    else if ((x) < (min))                                                      \
      (x) = (min);                                                             \
  } while (0)

// 数据类型转换函数
static inline int float_to_uint(float x, float x_min, float x_max, int bits) {
  float span = x_max - x_min;
  float offset = x_min;

  // 归一化到[0, 1]
  float normalized = (x - offset) / span;

  // 限制范围
  if (normalized < 0.0f)
    normalized = 0.0f;
  if (normalized > 1.0f)
    normalized = 1.0f;

  // 转换为整数
  return (int)(normalized * ((1 << bits) - 1));
}

static inline float uint_to_float(int x_int, float x_min, float x_max,
                                  int bits) {
  float span = x_max - x_min;
  float offset = x_min;

  // 转换为浮点数
  float normalized = (float)x_int / ((1 << bits) - 1);

  // 反归一化
  return normalized * span + offset;
}

#endif // __UTILS_H__
