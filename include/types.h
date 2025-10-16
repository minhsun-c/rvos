#ifndef __TYPES_H__
#define __TYPES_H__

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

/* task.h */
typedef struct context ctx_t;
typedef enum task_state state_t;
typedef struct task task_t;
typedef void (*taskFunc_t)(void *);

/* list.h */
typedef struct list list_t;

#endif  // __TYPES_H__