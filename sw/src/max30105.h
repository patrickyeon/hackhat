#ifndef MAX30105_H
#define MAX30105_H

#include <stdint.h>

typedef struct {
    uint32_t ir;
    uint32_t red;
} pdat_t;

void max30105_init(void);
int8_t max30105_temp(void);
uint8_t max30105_flags(void);
pdat_t max30105_read(void);
void max30105_red_pwr(uint8_t pwr);
void max30105_ir_pwr(uint8_t pwr);
void max30105_green_pwr(uint8_t pwr);
void max30105_averaging(uint8_t doublings);
void max30105_adc_config(int8_t lsb_size, int8_t sample_rate,
                         int8_t resolution);

#endif // MAX30105_H
