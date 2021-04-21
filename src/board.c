#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>

#include "board.h"

bool
execute_move(struct chess* game, struct move move)
{
    // TODO(Aurel): Stub. Fill this with code.
    assert(("Not implemented yet", 0 != 0));
}


char*
pos_to_str(enum POS pos)
{
    // NOTE(Aurel): Yes, this is ugly, but it's fast!
    char* str;
    switch (pos) {
        case A1: str = "A1"; break;
        case B1: str = "B1"; break;
        case C1: str = "C1"; break;
        case D1: str = "D1"; break;
        case E1: str = "E1"; break;
        case F1: str = "F1"; break;
        case G1: str = "G1"; break;
        case H1: str = "H1"; break;

        case A2: str = "A2"; break;
        case B2: str = "B2"; break;
        case C2: str = "C2"; break;
        case D2: str = "D2"; break;
        case E2: str = "E2"; break;
        case F2: str = "F2"; break;
        case G2: str = "G2"; break;
        case H2: str = "H2"; break;

        case A3: str = "A3"; break;
        case B3: str = "B3"; break;
        case C3: str = "C3"; break;
        case D3: str = "D3"; break;
        case E3: str = "E3"; break;
        case F3: str = "F3"; break;
        case G3: str = "G3"; break;
        case H3: str = "H3"; break;

        case A4: str = "A4"; break;
        case B4: str = "B4"; break;
        case C4: str = "C4"; break;
        case D4: str = "D4"; break;
        case E4: str = "E4"; break;
        case F4: str = "F4"; break;
        case G4: str = "G4"; break;
        case H4: str = "H4"; break;

        case A5: str = "A5"; break;
        case B5: str = "B5"; break;
        case C5: str = "C5"; break;
        case D5: str = "D5"; break;
        case E5: str = "E5"; break;
        case F5: str = "F5"; break;
        case G5: str = "G5"; break;
        case H5: str = "H5"; break;

        case A6: str = "A6"; break;
        case B6: str = "B6"; break;
        case C6: str = "C6"; break;
        case D6: str = "D6"; break;
        case E6: str = "E6"; break;
        case F6: str = "F6"; break;
        case G6: str = "G6"; break;
        case H6: str = "H6"; break;

        case A7: str = "A7"; break;
        case B7: str = "B7"; break;
        case C7: str = "C7"; break;
        case D7: str = "D7"; break;
        case E7: str = "E7"; break;
        case F7: str = "F7"; break;
        case G7: str = "G7"; break;
        case H7: str = "H7"; break;

        case A8: str = "A8"; break;
        case B8: str = "B8"; break;
        case C8: str = "C8"; break;
        case D8: str = "D8"; break;
        case E8: str = "E8"; break;
        case F8: str = "F8"; break;
        case G8: str = "G8"; break;
        case H8: str = "H8"; break;

        default: return NULL;
    }

    return str;
}

void
print_board(enum PIECE board[])
{

    for (int pos = 0; pos < 64; ++pos) {
        if (pos % 8 == 0)
            printf("\n");

        printf("[");

        enum PIECE piece = board[pos];
        switch (piece) {
            case PAWN: printf("P"); break;
            case BISHOP: printf("B"); break;
            case KNIGHT: printf("K"); break;
            case ROOK: printf("R"); break;
            case QUEEN: printf("Q"); break;
            case KING: printf("K"); break;
            default: printf(" "); break;
        }
        printf("]");
    }
    printf("\n");


}
