#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

void init_gpio(void);
void led_on(uint16_t leds);
void led_off(uint16_t leds);

#endif // GPIO_H
