#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

#include "./pindefs.h"
#include "./servo.h"

#define MIN(A,B) (A < B ? A : B)

static uint32_t min_pulse, max_pulse, tpulse;

void servo_init(uint32_t min, uint32_t max) {
    min_pulse = min;
    max_pulse = max;
    tpulse = (min + max) / 2;
    // we're claiming TIM2, I don't care.
    rcc_periph_clock_enable(RCC_TIM2);
    nvic_enable_irq(NVIC_TIM2_IRQ);
    rcc_periph_reset_pulse(RST_TIM2);
    // as far as I can tell, the prescaler doesn't do shit.
    //timer_set_prescaler(TIM2, 47); // 1us per count
    timer_disable_preload(TIM2);
    timer_one_shot_mode(TIM2);
    timer_direction_up(TIM2);

    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SERVO);
    gpio_clear(GPIOA, SERVO);
}

void servo_steer(uint32_t value) {
    // value in range 0..999
    uint32_t range = max_pulse - min_pulse;
    tpulse = min_pulse + (range * MIN(value, 999)) / 999;
}

void servo_pulse(void) {
    gpio_set(GPIOA, SERVO);
    timer_set_counter(TIM2, 0);
    timer_set_oc_value(TIM2, TIM_OC1, tpulse * 48);
    timer_enable_counter(TIM2);
    timer_enable_irq(TIM2, TIM_DIER_CC1IE);
}

void tim2_isr(void) {
    if (timer_get_flag(TIM2, TIM_SR_CC1IF)) {
        timer_clear_flag(TIM2, TIM_SR_CC1IF);
        timer_disable_counter(TIM2);
        gpio_clear(GPIOA, SERVO);
    }
}
