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

static void calibrate(void) {
    cal_open = 0;
    cal_closed = 0;
    // open the eye
    servo_steer(EYE_OPEN);
    syswait(200);
    // read for a second, that's the opened value
    for (int i = 0; i < 16; i++) {
        pdat_t val = max30105_read();
        cal_open += val.ir;
        syswait(60);
    }
    cal_open /= 16;
    // turn on the red led
    max30105_red_pwr(0x0f);
    // wait a second
    syswait(1000);
    // read for a second, that the closed value
    for (int i = 0; i < 16; i++) {
        pdat_t val = max30105_read();
        cal_closed += val.ir;
        syswait(60);
    }
    cal_closed /= 16;
    max30105_red_pwr(0x00);
    // close, then open the eye
    servo_steer(EYE_CLOSED);
    syswait(100);
    servo_steer(EYE_OPEN);
    syswait(100);
}


int main(void) {
    timers_init();
    gpio_init();
    servo_init(500, 2500);
    max30105_init();

    calibrate();

    uint32_t last_loop = 0;

    while(1) {
        while (last_loop == systime());
        last_loop = systime();
        // check for a new (single-key) command
        if (last_loop % 40 == 0) {
            pdat_t eye = max30105_read();
            if (eye.ir > (cal_open + cal_closed) / 2) {
                if (cal_open > cal_closed) {
                    servo_steer(EYE_OPEN);
                } else {
                    servo_steer(EYE_CLOSED);
                }
            } else {
                if (cal_open > cal_closed) {
                    servo_steer(EYE_CLOSED);
                } else {
                    servo_steer(EYE_OPEN);
                }
            }
        }
        if (last_loop % 20 == 0) {
            servo_pulse();
        }
    }
}
