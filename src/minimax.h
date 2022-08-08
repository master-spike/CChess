#ifndef MINIMAX_H_GUARD
#define MINIMAX_H_GUARD

#include "evaluation.h"
#include "chessboard.h"

struct MinimaxReturn {
  double val;
  struct Move move;
};

struct MinimaxReturn minimaxAlphaBeta(struct Chessboard b, unsigned int d, double alpha, double beta, struct EvalParams* params);



#endif
