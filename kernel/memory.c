#include "memory.h"

#define HEAP_SIZE (1024 * 1024) /* 1 MiB 的教学用堆空间 */

extern uint8_t kernel_end;

static uintptr_t heap_start = 0;
static uintptr_t heap_end = 0;
static uintptr_t heap_current = 0;

static uintptr_t align_up(uintptr_t value, size_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

void memory_init(void)
{
    uintptr_t start = align_up((uintptr_t)&kernel_end, 0x1000);
    heap_start = start;
    heap_current = start;
    heap_end = start + HEAP_SIZE;
}

void *kmalloc_aligned(size_t size, size_t alignment)
{
    if (alignment == 0) {
        alignment = 16;
    }

    uintptr_t current = align_up(heap_current, alignment);
    uintptr_t next = current + size;

    if (next > heap_end) {
        return NULL;
    }

    heap_current = next;
    return (void *)current;
}

void *kmalloc(size_t size)
{
    return kmalloc_aligned(size, 16);
}

size_t memory_total(void)
{
    return (size_t)(heap_end - heap_start);
}

size_t memory_used(void)
{
    return (size_t)(heap_current - heap_start);
}

size_t memory_free(void)
{
    return (size_t)(heap_end - heap_current);
}
