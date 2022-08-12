#ifndef EVALUATION_H_GUARD
#define EVALUATION_H_GUARD
#include "bitboard.h"
#include "patterns.h"
#include <math.h>

#define MAX INFINITY
#define MIN -INFINITY

double evaluate(struct BitBoard* board, double alpha, double beta);
struct EvalParams defaultParams();

#endif
