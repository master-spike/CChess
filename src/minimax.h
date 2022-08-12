#ifndef MINIMAX_H_GUARD
#define MINIMAX_H_GUARD


#include "evaluation.h"
#include "bitboard.h"
#include <stdint.h>

struct MinimaxReturn {
  double val;
  uint16_t move;
  int d;
};

struct MinimaxReturn minimaxAlphaBeta(struct BitBoard* b, unsigned int d, double alpha, double beta);

#endif
