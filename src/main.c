#include <stdio.h>

#include "board.h"
#include "main.h"

#include "types.h"

void
list_example()
{
    int ints[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    struct list* list = NULL;
    for (int i = 0; i < 10; ++i)
        list = list_push(list, &(ints[i]));

    for (int i = 0; i < 10; ++i) {
        printf("%i\n", *(int*)(list->elem));
        list = list_pop(list);
    }
}

int
main(int argc, char* argv[])
{
	printf("Upside Down Chess Bot version %'.1f\n", VERSION);
    //list_example(); // An example on how to use the list.
}
