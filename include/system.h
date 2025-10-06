#ifndef SYSTEM_H
#define SYSTEM_H

#include "boot_info.h"
#include <stdbool.h>

void system_store_boot_info(const boot_info_t *info);
const boot_info_t *system_boot_info(void);
bool system_has_boot_info(void);

#endif /* SYSTEM_H */
