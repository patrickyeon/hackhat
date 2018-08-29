#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

extern struct leds_t {
    bool led0;
    bool led1;
    bool led2;
} leds;

void gpio_init(void);
void leds_dc(uint16_t duty);

#endif // GPIO_H
