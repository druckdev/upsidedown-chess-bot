#include <stdlib.h>

#include "types.h"

struct int_list*
int_list_push(struct int_list* list, int elem)
{
    struct int_list* list_elem = malloc(sizeof(int));
    list_elem->elem = elem;

    if (list)
        list_elem->next = list;

    return list_elem;
}

struct int_list*
int_list_pop(struct int_list* list)
{
    struct int_list* new_list = list->next;
    int value = list->elem;
    free(list);
    return new_list;
}

struct int_list*
int_list_append_list(struct int_list* first, struct int_list* second)
{
    if (!first)
        return second;
    if (!second)
        return first;

    struct int_list* last = first;
    while (last->next != NULL)
        last = last->next;

    last->next = second;
    return first;
}
