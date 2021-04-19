#include <stdio.h>

#include "main.h"
#include "board.h"

uint32_t value[] = {0,1,4,4,5,9,1000000};

int main (int argc, char *argv[])
{
    printf("Upside Down Chess Bot version %'.1f\n", VERSION);

    board[A1] = BISHOP;
    print_board(board);
}
