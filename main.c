#include "./pindefs.h"
#include "./gpio.h"
#include "./usb.h"

void pause(int loops){
    return;
    for (int i = 0; i < loops; i++) {
        __asm__("nop");
    }
}

int main(void) {
    init_gpio();
    init_usb();

    while(1) {
        int tick = 30000;
        led_on(LED0);
        pause(tick);
        led_on(LED1);
        pause(tick);
        led_on(LED2);
        pause(tick);
        led_off(LED0);
        pause(tick);
        led_off(LED1);
        pause(tick);
        led_off(LED2);
        pause(tick);
		usb_poll();
    }
}
