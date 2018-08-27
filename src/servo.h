#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

void init_servo(uint32_t min, uint32_t max);
void pulse(void);
void steer(uint32_t value);

#endif // SERVO_H
