#include "./src/pindefs.h"
#include "./src/gpio.h"
#include "./src/usb.h"


// we're going to do a rough duty-cycling of the LEDs to control brightness.
//  Here's the period of one of those cycles.
const uint8_t t_led = 20;


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


int main(void) {
    init_gpio();
    init_usb();

    struct leds_t {
        // leds on?
        bool led0;
        bool led1;
        bool led2;
        // duty cycle for on time, 0..(t_led - 1)
        int dc;
    };

    struct leds_t leds = {false, false, false, 0};
    int loopcount = 0;

    uint8_t inchar;
    while(1) {
        loopcount = (loopcount + 1) % t_led;
        // check for a new (single-key) command
        if (usb_read(&inchar, 1) == 1) {
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
                case 'J':
                    leds.dc = (leds.dc <= 0 ? 0 : leds.dc - 1);
                    break;
                // K increase brightness
                case 'k':
                case 'K':
                    leds.dc = (leds.dc >= (t_led - 1) ?
                               (t_led - 1) : leds.dc + 1);
                    break;
                // otherwise just echo out the character in hex
                default:
                    hexout(inchar);
                    break;
            }
        }
        if (leds.dc > loopcount && leds.led0) {
            led_on(LED0);
        } else {
            led_off(LED0);
        }
        if (leds.dc > loopcount && leds.led1) {
            led_on(LED1);
        } else {
            led_off(LED1);
        }
        if (leds.dc > loopcount && leds.led2) {
            led_on(LED2);
        } else {
            led_off(LED2);
        }
		usb_poll();
    }
}
