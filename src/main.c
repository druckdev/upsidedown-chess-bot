#include <stdio.h>

#include "types.h"

#include "board.h"
#include "generator.h"
#include "main.h"

void
list_example()
{
	int ints[]        = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	struct list* list = NULL;
	for (int i = 0; i < 10; ++i) {
		list = list_push(list, &(ints[i]));
	}

	printf("Initialized\n");
	printf("\n");

	for (int i = 0; i < 10; ++i) {
		int i = *(int*)list_pop(list);
		printf("%i\n", i);
	}
}

int
main(int argc, char* argv[])
{
	// printf("Upside Down Chess Bot version %'.1f\n", VERSION);

	enum COLOR c = WHITE;
	if (argc >= 2 && argv[1][0] == '1')
		c = BLACK;

	struct chess chess = init_chess(c);
	run_chess(&chess);
	//list_example(); // An example on how to use the list.
}
