#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/rcc.h>

#include "./max30105.h"
#include "./pindefs.h"

void init_psens(void) {
    rcc_periph_clock_enable(RCC_I2C1);
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, (SCL | SDA));
    gpio_set_af(GPIOA, GPIO_AF4, (SCL | SDA));

    i2c_peripheral_disable(I2C1);
    i2c_set_speed(I2C1, i2c_speed_fm_400k, rcc_apb1_frequency / 1000000);
    i2c_peripheral_enable(I2C1);
}

int8_t psens_temp(void) {
    // TODO how do we handle timeouts, NACKs?
    // TODO do we want to know the fractional temperature too?
    uint8_t cmd[] = {0x21, 0x1};
    uint8_t resp = 0;

    i2c_transfer7(I2C1, 0x57, cmd, 2, NULL, 0);
    while (true) {
        i2c_transfer7(I2C1, 0x57, cmd, 1, &resp, 1);
        if (resp == 0) {
            break;
        }
    }

    cmd[0] = 0x1f;
    i2c_transfer7(I2C1, 0x57, cmd, 1, &resp, 1);
    return (int8_t)resp;
}
