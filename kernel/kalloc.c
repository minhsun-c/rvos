#include <stddef.h>
#include "defs.h"
#include "list.h"
#include "types.h"

// Symbols provided by the linker script
extern char HEAP_START[];
extern char HEAP_END[];

#define ALIGNMENT 8U
#define MIN_PAYLOAD ALIGNMENT
#define MIN_BLOCKSIZE (sizeof(MemHeader_t) + MIN_PAYLOAD)

list_t alloc_list;
list_t free_list;

typedef struct __attribute__((aligned(ALIGNMENT))) MemHeader {
    uint32_t start_addr;
    size_t size;
    list_t list;
} MemHeader_t;

static inline uint32_t _align_up(uint32_t address)
{
    /* Finish the function */
}

static inline uint32_t _align_down(uint32_t address)
{
    /* Finish the function */
}

static inline void memheader_init(MemHeader_t *hdr, size_t size)
{
    /* Finish the function */
}

static inline uint32_t _block_end(MemHeader_t *hdr, size_t size)
{
    return hdr->start_addr + size;
}

void kmem_init()
{
    /* Finish the function */
}

static void *kmem_alloc(size_t size)
{
    /* Finish the function */
}


static void kmem_free(void *p)
{
    /* Finish the function */
}

void *kalloc(size_t size)
{
    if (size > 0)
        return kmem_alloc(size);
    else
        return NULL;
}

void kfree(void *p)
{
    kmem_free(p);
}