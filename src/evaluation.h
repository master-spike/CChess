#ifndef EVALUATION_H_GUARD
#define EVALUATION_H_GUARD
#include "chessboard.h"

#define MAX = +16384.0
#define MIN = -16384.0

struct EvalParams {
  double v_p;
  double v_n;
  double v_b;
  double v_r;
  double v_q;
  double b_pair;
  double p_island;
  double dbl_pawn;
  double opp_sq_val;
};

double evaluate(struct Chessboard* board, struct EvalParams* params);
struct EvalParams defaultParams();

#endif
