#include "./src/pindefs.h"
#include "./src/gpio.h"
#include "./src/max30105.h"
#include "./src/servo.h"
#include "./src/timer.h"

#include <stdint.h>

// cal values for eyeball vs. eyelid
static uint32_t cal_open = 0, cal_closed = 0;

// servo positions for the eyeball
const uint32_t EYE_CLOSED = 100, EYE_OPEN = 600;

// tracking systick
volatile uint32_t systime = 0;


static void syswait(uint32_t ms) {
    uint32_t tstart = systime;
    while (systime < tstart + ms);
}


static void calibrate(void) {
    cal_open = 0;
    cal_closed = 0;
    // open the eye
    steer(EYE_OPEN);
    syswait(200);
    // read for a second, that's the opened value
    for (int i = 0; i < 16; i++) {
        pdat_t val = psens_read();
        cal_open += val.ir;
        syswait(60);
    }
    cal_open /= 16;
    // turn on the red led
    psens_red(true);
    // wait a second
    syswait(1000);
    // read for a second, that the closed value
    for (int i = 0; i < 16; i++) {
        pdat_t val = psens_read();
        cal_closed += val.ir;
        syswait(60);
    }
    cal_closed /= 16;
    psens_red(false);
    // close, then open the eye
    steer(EYE_CLOSED);
    syswait(100);
    steer(EYE_OPEN);
    syswait(100);
}


int main(void) {
    init_timers();
    init_gpio();
    init_servo(500, 2500);
    init_psens();

    calibrate();

    uint32_t last_loop = 0;

    while(1) {
        while (last_loop == systime);
        last_loop = systime;
        // check for a new (single-key) command
        if (systime % 40 == 0) {
            pdat_t eye = psens_read();
            if (eye.ir > (cal_open + cal_closed) / 2) {
                if (cal_open > cal_closed) {
                    steer(EYE_OPEN);
                } else {
                    steer(EYE_CLOSED);
                }
            } else {
                if (cal_open > cal_closed) {
                    steer(EYE_CLOSED);
                } else {
                    steer(EYE_OPEN);
                }
            }
        }
        if (systime % 20 == 0) {
            pulse();
        }
    }
}


void sys_tick_handler(void) {
    systime++;
}
