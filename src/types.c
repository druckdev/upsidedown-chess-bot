#include <stdlib.h>
#include <stdio.h>

#include "types.h"

struct list*
list_push(struct list* list, void* elem)
{
    struct list* list_elem = malloc(sizeof(struct list));
    list_elem->elem = elem;

    if (list)
        list_elem->next = list;

    return list_elem;
}

struct list*
list_pop(struct list* list)
{
    if(!list)
        return NULL;

    struct list* new_list = list->next;
    free(list);
    return new_list;
}

struct list*
list_append_list(struct list* first, struct list* second)
{
    if (!first)
        return second;
    if (!second)
        return first;

    struct list* last = first;
    while (last->next != NULL)
        last = last->next;

    last->next = second;
    return first;
}
