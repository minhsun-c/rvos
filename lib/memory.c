#include <stddef.h>
#include "types.h"

void *memset(void *src, int value, size_t size)
{
    unsigned char *p = (unsigned char *) src;
    while (size-- > 0) {
        *p++ = (unsigned char) value;
    }
    return src;
}

void *memcpy(void *dest, const void *src, size_t size)
{
    char *p_dest = (char *) dest;
    const char *p_src = (const char *) src;

    while (size-- > 0)
        *p_dest++ = *p_src++;
    return dest;
}