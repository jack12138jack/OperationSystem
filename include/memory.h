#ifndef MEMORY_H
#define MEMORY_H

#include "common.h"

void memory_init(void);
void *kmalloc(size_t size);
void *kmalloc_aligned(size_t size, size_t alignment);
size_t memory_total(void);
size_t memory_used(void);
size_t memory_free(void);

#endif /* MEMORY_H */
