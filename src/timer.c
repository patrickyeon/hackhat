#include "./timer.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/systick.h>


void init_timers(void) {
    rcc_clock_setup_in_hse_8mhz_out_48mhz();
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB); // 48 MHz
    systick_set_reload(48000); // 1kHz
    systick_clear();
    systick_interrupt_enable();
    systick_counter_enable();
}
