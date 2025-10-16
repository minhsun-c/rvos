#include <stddef.h>
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

/*
 * Heap allocation metadata
 * -------------------------
 * _alloc_start : the actual starting address of the allocatable heap region
 * _alloc_end   : the actual ending address of the allocatable heap region
 * _num_pages   : total number of allocatable pages
 */
static uint32_t _alloc_start = 0;
static uint32_t _alloc_end = 0;
static uint32_t _num_pages = 0;

/*
 * Page configuration
 * -------------------
 * PAGE_SIZE  : size of each physical page (256 bytes here)
 * PAGE_ORDER : log2(PAGE_SIZE)
 */
#define PAGE_SIZE 256
#define PAGE_ORDER 8

/*
 * Page flags
 * ----------
 * PAGE_TAKEN : page is allocated
 * PAGE_LAST  : marks the last page of an allocated block
 */
#define PAGE_TAKEN (uint8_t) (1 << 0)
#define PAGE_LAST (uint8_t) (1 << 1)

/*
 * Page Descriptor
 * ----------------
 * Each page has a 1-byte flag field indicating its status.
 *
 * flags:
 *  - bit 0 (PAGE_TAKEN): set if this page is allocated
 *  - bit 1 (PAGE_LAST): set if this page is the last page of a block
 */
struct Page {
    uint8_t flags;
};

// ---------------------------------------------------------------------------
// Helper functions to manipulate page flags
// ---------------------------------------------------------------------------
static inline void _clear(struct Page *page)
{
    page->flags = 0;
}

static inline int _is_free(struct Page *page)
{
    return !(page->flags & PAGE_TAKEN);
}

static inline void _set_flag(struct Page *page, uint8_t flags)
{
    page->flags |= flags;
}

static inline int _is_last(struct Page *page)
{
    return !!(page->flags & PAGE_LAST);
}

/*
 * Align the given address up to the nearest page boundary.
 * This ensures that the heap allocation region starts on a
 * properly aligned page address.
 */
static inline uint32_t _align_page(uint32_t address)
{
    uint32_t order = (1 << PAGE_ORDER) - 1;
    return (address + order) & (~order);
}

/*
 * Initialize page descriptors and heap metadata.
 *
 * We reserve 2048 pages at the beginning of the heap for the page
 * descriptor array itself. The remaining pages form the allocatable
 * heap region. Each Page struct corresponds to one physical page.
 */
void page_init()
{
    _num_pages = ((uint32_t) HEAP_SIZE / PAGE_SIZE) - 2048;

    struct Page *page = (struct Page *) HEAP_START;
    for (int i = 0; i < _num_pages; i++) {
        _clear(page);
        page++;
    }

    _alloc_start = _align_page((uint32_t) HEAP_START + 2048 * PAGE_SIZE);
    _alloc_end = _alloc_start + (PAGE_SIZE * _num_pages);
}

/*
 * Allocate a block of contiguous pages.
 * - npages: number of pages to allocate.
 *
 * The allocator performs a simple first-fit search through the page
 * descriptor array. It looks for npages consecutive free pages,
 * sets their PAGE_TAKEN flags, and marks the last page with PAGE_LAST.
 *
 * Returns: pointer to the start of the allocated block, or NULL on failure.
 */
static void *page_alloc(int npages)
{
    int found = 0;
    struct Page *page_i = (struct Page *) HEAP_START;
    for (int i = 0; i <= (_num_pages - npages); i++) {
        if (_is_free(page_i)) {
            found = 1;

            // Check the following (npages - 1) pages for availability
            struct Page *page_j = page_i;
            for (int j = i; j < (i + npages); j++) {
                if (!_is_free(page_j)) {
                    found = 0;
                    break;
                }
                page_j++;
            }

            // If found a contiguous block, mark it and return the address
            if (found) {
                struct Page *page_k = page_i;
                for (int k = i; k < (i + npages); k++) {
                    _set_flag(page_k, PAGE_TAKEN);
                    page_k++;
                }
                page_k--;
                _set_flag(page_k, PAGE_LAST);
                return (void *) (_alloc_start + i * PAGE_SIZE);
            }
        }
        page_i++;
    }
    return NULL;
}

/*
 * Free a previously allocated page block.
 * - p: start address of the memory block to free
 *
 * The function walks through the corresponding page descriptors
 * starting from p, clearing flags until the PAGE_LAST page is reached.
 * Invalid pointers are ignored.
 */
static void page_free(void *p)
{
    if (!p || (uint32_t) p >= _alloc_end) {
        return;
    }

    struct Page *page = (struct Page *) HEAP_START;
    page += ((uint32_t) p - _alloc_start) / PAGE_SIZE;

    while (!_is_free(page)) {
        if (_is_last(page)) {
            _clear(page);
            break;
        } else {
            _clear(page);
            page++;
        }
    }
}

void *malloc(size_t size)
{
    int res = size % PAGE_SIZE;
    int npages = size / PAGE_SIZE;

    if (res > 0)
        npages++;
    return page_alloc(npages);
}

void free(void *p)
{
    page_free(p);
}
