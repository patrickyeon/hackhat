#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

void servo_init(uint32_t min, uint32_t max);
void servo_pulse(void);
void servo_steer(uint32_t value);

#endif // SERVO_H
