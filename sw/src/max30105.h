#ifndef MAX30105_H
#define MAX30105_H

#include <stdint.h>

typedef struct {
    uint32_t ir;
    uint32_t red;
} pdat_t;

void max30105_init(void);
int8_t max30105_temp(void);
pdat_t max30105_read(void);
void max30105_red_pwr(uint8_t pwr);

#endif // MAX30105_H
