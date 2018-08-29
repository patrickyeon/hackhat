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


int main(void) {
    timers_init();
    gpio_init();
    usb_init();
    servo_init(500, 2500);
    max30105_init();

    uint32_t angle = 500;
    bool sweep = false;

    uint8_t inchar;
    uint32_t last_loop = 0;
    pdat_t sensordat;
    while(1) {
        do {
            usb_poll();
        } while (last_loop == systime());
        last_loop = systime();
        // check for a new (single-key) command
        if (usb_read(&inchar, 1) == 1) {
            inchar = to_lower(inchar);
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
                    servo_steer(angle);
                    sweep = false;
                    break;
                case 'l':
                    angle = MIN(1000, angle + 50);
                    servo_steer(angle);
                    sweep = false;
                    break;
                // S to have a "sweep" pattern on the servo
                case 's':
                    sweep = true;
                    break;
                // T to read the particle sensor's temperature
                case 't':
                    hexout(max30105_temp());
                    break;
                // P to read particle detect
                case 'p':
                    sensordat = max30105_read();
                    valout(sensordat.ir);
                    valout(sensordat.red);
                    break;
                // otherwise just echo out the character in hex
                default:
                    hexout(inchar);
                    break;
            }
        }
        if (last_loop % 20 == 0) {
            if (sweep && last_loop % 80 == 0) {
                angle = angle < 950 ? angle + 50 : 0;
                servo_steer(angle);
            }
            servo_pulse();
        }
    }
}
