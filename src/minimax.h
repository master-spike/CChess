#ifndef MINIMAX_H_GUARD
#define MINIMAX_H_GUARD

#include <stdint.h>
#include <time.h>
#include "evaluation.h"
#include "bitboard.h"


struct MinimaxReturn {
  double val;
  uint16_t move;
  int d;
};

uint64_t nodes_in_minimax;
uint64_t nodes_in_quescience;

struct MinimaxReturn minimaxAlphaBeta(struct BitBoard* b, unsigned int d, double alpha, double beta, clock_t max_time);
struct MinimaxReturn timedIterativeDeepening(struct BitBoard board, clock_t max_time);
#endif
