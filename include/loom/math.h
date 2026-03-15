#ifndef LOOM_MATH_H
#define LOOM_MATH_H 1

#define abs(x)                                                                \
  ({                                                                          \
    auto _x = (x);                                                            \
    _x < 0 ? -_x : _x;                                                        \
  })

#define add        __builtin_add_overflow
#define cast(x, y) __builtin_add_overflow (x, 0, y)
#define mul        __builtin_mul_overflow

#endif