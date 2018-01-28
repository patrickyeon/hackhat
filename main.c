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

    while(1) {
		usb_poll();
    }
}
