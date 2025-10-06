#include "system.h"

static boot_info_t stored_info;
static bool has_info = false;

void system_store_boot_info(const boot_info_t *info)
{
    if (info && info->magic == BOOT_INFO_MAGIC) {
        stored_info = *info;
        has_info = true;
    } else {
        has_info = false;
    }
}

const boot_info_t *system_boot_info(void)
{
    return has_info ? &stored_info : NULL;
}

bool system_has_boot_info(void)
{
    return has_info;
}
