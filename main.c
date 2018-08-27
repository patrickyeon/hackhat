#include "./src/pindefs.h"
#include "./src/gpio.h"
#include "./src/servo.h"
#include "./src/timer.h"
#include "./src/usb.h"

#include <stdint.h>

#define MIN(A,B) (A < B ? A : B)
#define MAX(A,B) (A > B ? A : B)

// we're going to do a rough duty-cycling of the LEDs to control brightness.
//  Here's the period of one of those cycles.
static uint16_t t_led = 500;

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


static uint8_t tolower(uint8_t c) {
    if ('A' <= c && c <= 'Z') {
        c = (c - 'A') + 'a';
    }
    return c;
}


int main(void) {
    init_timers();
    init_gpio();
    init_usb();
    init_servo(500, 2500);

    int loopcount = 0;
    uint32_t angle = 500;
    bool pulsed = false, sweep = false;

    uint8_t inchar;
    while(1) {
        loopcount = (loopcount + 1) % t_led;
        // check for a new (single-key) command
        if (usb_read(&inchar, 1) == 1) {
            inchar = tolower(inchar);
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
                case 's':
                    sweep = true;
                    break;
                // otherwise just echo out the character in hex
                default:
                    hexout(inchar);
                    break;
            }
        }
        if (systime % 20 == 0) {
            if (!pulsed) {
                if (sweep && systime % 80 == 0) {
                    angle = angle < 1000 ? angle + 50 : 0;
                    steer(angle);
                }
                pulse();
                pulsed = true;
            }
        } else {
            pulsed = false;
        }
		usb_poll();
    }
}

void sys_tick_handler(void) {
    systime = (systime + 1) % 10000;
}
