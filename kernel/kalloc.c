#include <stddef.h>
#include "defs.h"
#include "list.h"
#include "types.h"

// Symbols provided by the linker script
extern char HEAP_START[];
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

void kmem_init()
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

static void *kmem_alloc(size_t size)
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

static void kmem_coalesce(MemHeader_t *cur_hdr)
{
    if (cur_hdr->list.prev != &free_list) {
        MemHeader_t *prev = list_entry(cur_hdr->list.prev, MemHeader_t, list);

        uint32_t prev_end = _block_end(prev, prev->size);

        if (prev_end == (uint32_t) cur_hdr) {
            prev->size += sizeof(MemHeader_t) + cur_hdr->size;
            list_remove(&cur_hdr->list);
            cur_hdr = prev;
        }
    }

    if (cur_hdr->list.next != &free_list) {
        MemHeader_t *next = list_entry(cur_hdr->list.next, MemHeader_t, list);

        uint32_t hdr_end = _block_end(cur_hdr, cur_hdr->size);

        if (hdr_end == (uint32_t) next) {
            cur_hdr->size += sizeof(MemHeader_t) + next->size;
            list_remove(&next->list);
        }
    }
}

static void kmem_free(void *p)
{
    if (!p)
        return;

    MemHeader_t *hdr = (MemHeader_t *) p - 1;

    list_remove(&hdr->list);

    for (list_t *node = free_list.next; node != &free_list; node = node->next) {
        MemHeader_t *cur = list_entry(node, MemHeader_t, list);

        if (cur->start_addr > hdr->start_addr) {
            list_insert_before(node, &hdr->list);
            break;
        }
    }

    kmem_coalesce(hdr);
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

void kalloc_test(void)
{
    kprintf("=== kalloc_test ===\n");

    void *p1 = kalloc(31);
    void *p2 = kalloc(64);
    void *p3 = kalloc(128);

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

    kfree(p2);  // free middle block

    void *p4 = kalloc(256);
    kprintf("p4 = %p\n", p4);

    void *p5 = kalloc(48);  // should reuse p2's block
    kprintf("p5 = %p\n", p5);

    kfree(p1);
    kfree(p3);
    kfree(p4);
    kfree(p5);
}