#include "./src/pindefs.h"
#include "./src/gpio.h"
#include "./src/max30105.h"
#include "./src/servo.h"
#include "./src/timer.h"
#include "./src/usb.h"

#include <stdint.h>

#define MIN(A,B) (A < B ? A : B)
#define MAX(A,B) (A > B ? A : B)

// we're going to do a rough duty-cycling of the LEDs to control brightness.
//  Here's the period of one of those cycles.
static uint16_t t_led = 500;

// cal values for eyeball vs. eyelid
static uint32_t cal_open = 0, cal_closed = 0;

// servo positions for the eyeball
const uint32_t EYE_CLOSED = 100, EYE_OPEN = 600;

// tracking systick
volatile uint32_t systime = 0;

// turn a uint8_t into a hex representation like 'B2 ' and output over USB
static void hexout(uint8_t val) {
    uint8_t buff[3];
    buff[0] = (val >> 4) + '0';
    if (buff[0] > '9') {
        buff[0] = buff[0] - '0' + ('A' - 10);
    }
    buff[1] = (val & 0xf) + '0';
    if (buff[1] > '9') {
        buff[1] = buff[1] - '0' + ('A' - 10);
    }
    buff[2] = ' ';
    usb_write(buff, 3);
}

static void valout(uint32_t val) {
    uint8_t buff[11];
    uint32_t idx = 1000*1000*1000;
    for (int i = 9; i >= 0; i--) {
        uint32_t n = val/idx;
        if (n != 0) {
            val -= n * idx;
        }
        buff[9 - i] = '0' + n;
        idx /= 10;
    }
    buff[10] = ' ';
    usb_write(buff, 11);
}


static uint8_t to_lower(uint8_t c) {
    if ('A' <= c && c <= 'Z') {
        c = (c - 'A') + 'a';
    }
    return c;
}


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
    init_usb();
    init_servo(500, 2500);
    init_psens();

    uint32_t angle = 500;
    bool sweep = false, blinking = false;

    uint8_t inchar;
    uint32_t last_loop = 0;
    pdat_t foo;
    while(1) {
        do {
            usb_poll();
        } while (last_loop == systime);
        last_loop = systime;
        // check for a new (single-key) command
        if (usb_read(&inchar, 1) == 1) {
            inchar = to_lower(inchar);
            blinking = false;
            switch (inchar) {
                // 1-3 toggle that LED
                case '1':
                    leds.led0 = leds.led0 ? false : true;
                    break;
                case '2':
                    leds.led1 = leds.led1 ? false : true;
                    break;
                case '3':
                    leds.led2 = leds.led2 ? false : true;
                    break;
                // J decrease brightness (vim style)
                case 'j':
                    t_led = (t_led <= 50 ? 0 : t_led - 50);
                    leds_dc(t_led);
                    break;
                // K increase brightness
                case 'k':
                    t_led = (t_led >= 949 ? 999 : t_led + 50);
                    leds_dc(t_led);
                    break;
                // H/L to turn the servo
                case 'h':
                    angle = MAX(50, angle) - 50;
                    steer(angle);
                    sweep = false;
                    break;
                case 'l':
                    angle = MIN(1000, angle + 50);
                    steer(angle);
                    sweep = false;
                    break;
                // S to have a "sweep" pattern on the servo
                case 's':
                    sweep = true;
                    break;
                // T to read the particle sensor's temperature
                case 't':
                    hexout(psens_temp());
                    break;
                // P to read particle detect
                case 'p':
                    foo = psens_read();
                    valout(foo.ir);
                    valout(foo.red);
                    break;
                // B to do blink-tracking mode
                case 'b':
                    calibrate();
                    blinking = true;
                    break;
                // otherwise just echo out the character in hex
                default:
                    hexout(inchar);
                    break;
            }
        }
        if (systime % 20 == 0) {
            if (sweep && systime % 80 == 0) {
                angle = angle < 950 ? angle + 50 : 0;
                steer(angle);
            }
            if (blinking && systime % 40 == 0) {
                pdat_t eye = psens_read();
                if (eye.ir > (cal_open + cal_closed) / 2) {
                    steer(EYE_OPEN);
                } else {
                    steer(EYE_CLOSED);
                }
            }
            pulse();
        }
    }
}

void sys_tick_handler(void) {
    systime = (systime + 1) % 10000;
}
