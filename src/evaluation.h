#ifndef EVALUATION_H_GUARD
#define EVALUATION_H_GUARD
#include "bitboard.h"
#include <math.h>

#define MAX INFINITY
#define MIN -INFINITY

double evaluate(struct BitBoard* board);
struct EvalParams defaultParams();

#endif
