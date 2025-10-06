#ifndef PIT_H
#define PIT_H

#include "common.h"

void pit_init(uint32_t frequency);
uint64_t pit_ticks(void);
void pit_sleep(uint64_t ticks);
uint32_t pit_frequency_hz(void);

#endif /* PIT_H */
