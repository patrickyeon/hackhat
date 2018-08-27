#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

#include "./gpio.h"
#include "./pindefs.h"

struct leds_t leds;

void init_gpio(void){
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, ALL_LEDS);
    gpio_clear(GPIOA, ALL_LEDS);

    rcc_periph_clock_enable(RCC_TIM3);
    nvic_enable_irq(NVIC_TIM3_IRQ);
    timer_disable_preload(TIM3);
    timer_direction_up(TIM3);
    timer_continuous_mode(TIM3);
    timer_set_oc_value(TIM3, TIM_OC2, 999);
    leds_dc(500);
    timer_enable_counter(TIM3);
    timer_enable_irq(TIM3, TIM_DIER_CC1IE);
    timer_enable_irq(TIM3, TIM_DIER_CC2IE);
}

void leds_dc(uint16_t duty) {
    timer_set_oc_value(TIM3, TIM_OC1, duty <= 999 ? duty : 999);
}

void tim3_isr(void) {
    if (timer_get_flag(TIM3, TIM_SR_CC1IF)) {
        timer_clear_flag(TIM3, TIM_SR_CC1IF);
        gpio_clear(GPIOA, ALL_LEDS);
    } else if (timer_get_flag(TIM3, TIM_SR_CC2IF)) {
        timer_clear_flag(TIM3, TIM_SR_CC2IF);
        if (leds.led0) {
            gpio_set(GPIOA, LED0);
        }
        if (leds.led1) {
            gpio_set(GPIOA, LED1);
        }
        if (leds.led2) {
            gpio_set(GPIOA, LED2);
        }
        timer_set_counter(TIM3, 0);
    }
}

