#include <stddef.h>
#include "defs.h"
#include "list.h"
#include "types.h"

// Symbols provided by the linker script
extern char TEXT_START[];
extern char TEXT_END[];
extern char DATA_START[];
extern char DATA_END[];
extern char RODATA_START[];
extern char RODATA_END[];
extern char BSS_START[];
extern char BSS_END[];
extern char HEAP_START[];
extern char HEAP_SIZE[];
extern char HEAP_END[];

#define ALIGNMENT 8U
#define MIN_PAYLOAD ALIGNMENT

list_t alloc_list;
list_t free_list;

typedef struct __attribute__((aligned(ALIGNMENT))) MemHeader {
    uint32_t start_addr;
    size_t size;
    list_t list;
} MemHeader_t;

static inline uint32_t _align_up(uint32_t address)
{
    uint32_t mask = ALIGNMENT - 1;
    return (address + mask) & (~mask);
}

static inline uint32_t _align_down(uint32_t address)
{
    uint32_t mask = ALIGNMENT - 1;
    return address & (~mask);
}

static inline void memheader_init(MemHeader_t *hdr, size_t size)
{
    hdr->start_addr = (uint32_t) (hdr + 1);
    hdr->size = size;
    list_init(&hdr->list);
}

static inline uint32_t _block_end(MemHeader_t *hdr, size_t size)
{
    return hdr->start_addr + size;
}

void page_init()
{
    list_init(&free_list);
    list_init(&alloc_list);

    uint32_t heap_end = _align_down((uint32_t) HEAP_END);
    MemHeader_t *hdr = (MemHeader_t *) _align_up((uint32_t) HEAP_START);
    uint32_t payload_start = (uint32_t) (hdr + 1);
    uint32_t payload_size = heap_end - payload_start;

    if (payload_start >= heap_end)
        panic("Heap is too small");

    memheader_init(hdr, payload_size);
    list_insert_after(&free_list, &hdr->list);
}

static void *page_alloc(size_t size)
{
    size_t request = _align_up(size);  // aligned payload only

    for (list_t *node = free_list.next; node != &free_list; node = node->next) {
        MemHeader_t *hdr = list_entry(node, MemHeader_t, list);
        if (hdr->size < request)
            continue;

        uint32_t block_end = _block_end(hdr, hdr->size);

        uint32_t new_hdr_addr = _align_up(_block_end(hdr, request));
        uint32_t new_payload_start =
            (uint32_t) ((MemHeader_t *) new_hdr_addr + 1);

        size_t remain = (new_payload_start <= block_end)
                            ? (size_t) (block_end - new_payload_start)
                            : 0;

        if (remain >= MIN_PAYLOAD) {
            // Split: [hdr | payload | hdr_split | payload]
            MemHeader_t *hdr_split = (MemHeader_t *) new_hdr_addr;
            memheader_init(hdr_split, remain);

            // Shrink allocated block to exactly 'request'
            hdr->size = request;

            // Replace current free node with the tail free node
            list_replace(&hdr->list, &hdr_split->list);
        } else {
            // No room for a valid tail; give whole block
            list_remove(&hdr->list);
        }

        // move header to allocated list
        list_insert_before(&alloc_list, &hdr->list);
        return (void *) hdr->start_addr;
    }

    return NULL;  // no suitable block
}


static void page_free(void *p)
{
    if (!p)
        return;

    MemHeader_t *hdr =
        (MemHeader_t *) p - 1;  // header immediately before payload

    // Move from alloc_list back to free_list
    list_remove(&hdr->list);

    // (Optional) insert in address-sorted order for coalescing; for now:
    list_insert_after(&free_list, &hdr->list);
}

void *malloc(size_t size)
{
    if (size > 0)
        return page_alloc(size);
    else
        return NULL;
}

void free(void *p)
{
    page_free(p);
}

void malloc_test(void)
{
    kprintf("=== malloc_test ===\n");

    void *p1 = malloc(31);
    void *p2 = malloc(64);
    void *p3 = malloc(128);

    kprintf("p1 = %p\n", p1);
    kprintf("p2 = %p\n", p2);
    kprintf("p3 = %p\n", p3);

    // Check alignment
    if (((unsigned int) p1 & (ALIGNMENT - 1)) != 0)
        kprintf("p1 not aligned!\n");
    if (((unsigned int) p2 & (ALIGNMENT - 1)) != 0)
        kprintf("p2 not aligned!\n");
    if (((unsigned int) p3 & (ALIGNMENT - 1)) != 0)
        kprintf("p3 not aligned!\n");

    free(p2);  // free middle block

    void *p4 = malloc(256);
    kprintf("p4 = %p\n", p4);

    void *p5 = malloc(48);  // should reuse p2's block
    kprintf("p5 = %p\n", p5);

    free(p1);
    free(p3);
    free(p4);
    free(p5);
}