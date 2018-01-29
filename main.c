#include "./src/pindefs.h"
#include "./src/gpio.h"
#include "./src/usb.h"

void pause(int loops){
    return;
    for (int i = 0; i < loops; i++) {
        __asm__("nop");
    }
}

int main(void) {
    init_gpio();
    init_usb();

    struct leds_t {
        bool led0;
        bool led1;
        bool led2;
        int dc;
    };

    struct leds_t leds = {false, false, false, 0};

    uint8_t charbuff[40];
    while(1) {
        if (usb_read(charbuff, 1) == 1) {
            uint8_t inchar = charbuff[0];
            switch (inchar) {
                case '1':
                    leds.led0 = leds.led0 ? false : true;
                    break;
                case '2':
                    leds.led1 = leds.led1 ? false : true;
                    break;
                case '2':
                    leds.led2 = leds.led2 ? false : true;
                    break;
                default:
                    charbuff[0] = (inchar / 100) + '0';
                    charbuff[1] = ((inchar / 10) % 10) + '0';
                    charbuff[2] = (inchar % 10) + '0';
                    charbuff[3] = ' ';
                    usb_write(charbuff, 4);
                    break;
            }
        }
        if (leds.led0) {
            led_on(LED0);
        } else {
            led_off(LED0);
        }
        if (leds.led1) {
            led_on(LED1);
        } else {
            led_off(LED1);
        }
        if (leds.led2) {
            led_on(LED2);
        } else {
            led_off(LED2);
        }
		usb_poll();
    }
}
