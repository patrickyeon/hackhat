#include "./timer.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/systick.h>

static volatile uint32_t _ms;

void timers_init(void) {
    rcc_clock_setup_in_hse_8mhz_out_48mhz();
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB); // 48 MHz
    systick_set_reload(48000); // 1kHz
    _ms = 0;
    systick_clear();
    systick_interrupt_enable();
    systick_counter_enable();
}


uint32_t systime(void) {
    // this will overflow at ~50 days. I hope you're not using this that long
    return _ms;
}


void sys_tick_handler(void) {
    _ms++;
}

void syswait(uint32_t ms) {
    uint32_t tstart = systime();
    while (systime() < tstart + ms);
}
