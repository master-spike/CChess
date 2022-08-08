#ifndef EVALUATION_H_GUARD
#define EVALUATION_H_GUARD
#include "chessboard.h"
#include <math.h>

#define MAX INFINITY
#define MIN -INFINITY

struct EvalParams {
  double v_p;
  double v_n;
  double v_b;
  double v_r;
  double v_q;
  double v_k;
  double b_pair;
  double p_island;
  double dbl_pawn;
  double opp_sq_val;
  double centre_control;
  double p_adv;
  
};

double evaluate(struct Chessboard* board, struct EvalParams* params);
struct EvalParams defaultParams();

#endif
