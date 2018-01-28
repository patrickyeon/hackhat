#ifndef PINDEFS_H
#define PINDEFS_H

#include <libopencm3/stm32/gpio.h>

#define LED0 GPIO4
#define LED1 GPIO3
#define LED2 GPIO2
#define ALL_LEDS (LED0 | LED1 | LED2)

#define SCL GPIO9
#define SDA GPIO10

#define USBDP GPIO12
#define USBDM GPIO11

#define SERVO GPIO1

#endif // PINDEFS_H
