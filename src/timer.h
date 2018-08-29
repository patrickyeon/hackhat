#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

void timers_init(void);
uint32_t systime(void);
void syswait(uint32_t ms);

#endif // TIMER_H
