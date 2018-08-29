#ifndef MAX30105_H
#define MAX30105_H

#include <stdint.h>

typedef struct {
    uint32_t ir;
    uint32_t red;
} pdat_t;

void init_psens(void);
int8_t psens_temp(void);
pdat_t psens_read(void);
void psens_red(bool);

#endif // MAX30105_H
