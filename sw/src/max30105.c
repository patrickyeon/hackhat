#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/rcc.h>

#include "./max30105.h"
#include "./pindefs.h"

const uint8_t ADDR = 0x57;

// TODO how do we handle timeouts, NACKs?
static void _write(uint8_t reg, uint8_t value) {
    uint8_t cmd[] = {reg, value};
    i2c_transfer7(I2C1, ADDR, cmd, 2, NULL, 0);
}

static void _read(uint8_t reg, uint8_t *buff, uint8_t len) {
    i2c_transfer7(I2C1, ADDR, &reg, 1, buff, len);
}

void max30105_init(void) {
    rcc_periph_clock_enable(RCC_I2C1);
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, (SCL | SDA));
    gpio_set_af(GPIOA, GPIO_AF4, (SCL | SDA));

    i2c_peripheral_disable(I2C1);
    i2c_set_speed(I2C1, i2c_speed_fm_400k, rcc_apb1_frequency / 1000000);
    i2c_peripheral_enable(I2C1);

    // set particle-sensing mode with Red+IR LEDs
    _write(0x09, 0x03);

    // sure, let's set all the interrupts enabled
    _write(0x02, 0xf0);
    _write(0x03, 0x02);

    // WAG about config settings
    // set sample rate fairly low, ADC range and LED power low-mid
    _write(0x0a, 0x25);
    _write(0x0c, 0x00);
    _write(0x0d, 0x0f);

    //  allow the FIFO to roll-over (we'll just always poll the most recent
    // conversion)
    _write(0x08, 0x10);
}

uint8_t max30105_flags(void) {
    uint8_t reg0, reg1;
    _read(0x00, &reg0, 1);
    _read(0x01, &reg1, 1);
    return (reg0 & 0xf1) | (reg1 & 0x02);
}

void max30105_red_pwr(uint8_t pwr) {
    _write(0x0c, pwr);
}

void max30105_ir_pwr(uint8_t pwr) {
    _write(0x0d, pwr);
}

void max30105_green_pwr(uint8_t pwr) {
    _write(0x0e, pwr);
}

void max30105_averaging(uint8_t doublings) {
    uint8_t reg;
    _read(0x08, &reg, 1);
    _write(0x08, (reg & 0x1f) | ((doublings & 0x07) << 5));
}

void max30105_adc_config(int8_t lsb_size, int8_t sample_rate,
                         int8_t resolution) {
    // set any of the arguments negative to have them ignored
    uint8_t reg;
    _read(0x0a, &reg, 1);
    if (lsb_size >= 0) {
        reg = (reg & 0x9f) | ((lsb_size & 0x03) << 5);
    }
    if (sample_rate >= 0) {
        reg = (reg & 0xe3) | ((sample_rate & 0x07) << 2);
    }
    if (resolution >= 0) {
        reg = (reg & 0xfc) | (resolution & 0x03);
    }
    _write(0x0a, reg);
}

int8_t max30105_temp(void) {
    // TODO do we want to know the fractional temperature too?
    uint8_t resp = 0;

    _write(0x21, 0x1);

    while (true) {
        _read(0x21, &resp, 1);
        if (resp == 0) {
            break;
        }
    }

    _read(0x1f, &resp, 1);
    return (int8_t)resp;
}

pdat_t max30105_read(void) {
    uint8_t resp = 0;
    uint8_t buff[6];
    //  get the write pointer, decrement it, set that as read pointer, then
    // read from the FIFO.
    _read(0x04, &resp, 1);
    _write(0x06, (resp - 1) & 0x1f);
    _read(0x07, buff, 6);
    return (pdat_t) {((buff[3] << 16) | (buff[4] << 8) | buff[5]),
                     ((buff[0] << 16) | (buff[1] << 8) | buff[2])};
}
