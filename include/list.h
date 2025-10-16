#ifndef __LIST_H__
#define __LIST_H__

struct list {
    struct list *prev;
    struct list *next;
};

static inline void list_init(list_t *head)
{
    head->next = head;
    head->prev = head;
}

static inline void list_insert_after(list_t *head, list_t *node)
{
    head->next->prev = node;
    node->next = head->next;
    head->next = node;
    node->prev = head;
}

static inline void list_insert_before(list_t *head, list_t *node)
{
    head->prev->next = node;
    node->prev = head->prev;
    head->prev = node;
    node->next = head;
}

static inline void list_remove(list_t *node)
{
    node->next->prev = node->prev;
    node->prev->next = node->next;

    node->next = node;
    node->prev = node;
}


#endif  // __LIST_H__