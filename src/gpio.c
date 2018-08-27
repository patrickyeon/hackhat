#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include "./gpio.h"
#include "./pindefs.h"

void init_gpio(void){
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, ALL_LEDS);
    gpio_clear(GPIOA, ALL_LEDS);
}

void led_on(uint16_t leds){
    gpio_set(GPIOA, leds);
}

void led_off(uint16_t leds) {
    gpio_clear(GPIOA, leds);
}

