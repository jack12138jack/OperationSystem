#ifndef BOOT_INFO_H
#define BOOT_INFO_H

#include "common.h"

#define BOOT_INFO_MAGIC 0x45444F53u

typedef struct __attribute__((packed)) boot_info {
    uint32_t magic;
    uint8_t boot_drive;
    uint8_t reserved;
    uint16_t kernel_sectors;
    uint16_t kernel_segment;
    uint16_t kernel_offset;
    uint32_t kernel_entry;
} boot_info_t;

#endif /* BOOT_INFO_H */
