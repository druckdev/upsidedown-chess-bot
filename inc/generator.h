#ifndef GENERATOR_H
#define GENERATOR_H

#include "chess.h"
#include "types.h"

struct list* generate_moves(struct chess* game);

void test_move_generator();

#endif /* GENERATOR_H */
