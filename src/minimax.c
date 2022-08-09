#include "minimax.h"

void swapMove(struct Move* m1, struct Move* m2) {
  struct Move t = *m1;
  *m1 = *m2;
  *m2 = t;
}

void swapDouble(double* d1, double* d2) {
  double t = *d1;
  *d1 = *d2;
  *d2 = t;
}

void sortMovesQsort(int l, int h, struct Move* moves, double* evals) {
  if (h - l <= 1) return;
  if (h - l == 2) {
    if (evals[l] <= evals[l+1]) return;
    swapMove(moves+l, moves+l+1);
    swapDouble(evals+l,evals+l+1);
    return;
  }
  double pivot = evals[l];
  int x = l+1;
  int y = h-1;
  while (x < y) {
    double v1 = evals[x];
    if (v1 <= pivot) {
      x++;
      continue;
    }
    double v2 = evals[y];
    if (v2 > pivot) {
      y--;
      continue;
    }
    // here we swap
    swapMove(moves+x, moves+y);
    swapDouble(evals+x,evals+y);
    x++; y--;
  }
  int m = x;
  if (x==y) {
    double v = evals[x];
    if (v > pivot) m = x-1;
  }
  swapMove(moves+m, moves+l);
  swapDouble(evals+m,evals+l);
  sortMovesQsort(l, m, moves, evals);
  sortMovesQsort(m+1, h, moves, evals);
}

void sortMoves(int l, int h, struct Move* moves, struct Chessboard* board, struct EvalParams* params) {
  struct Chessboard nextboard;
  double evals[400];
  for (int i = l; i < h; i++) {
    doMove(moves[i], board, &nextboard);
    evals[i] = evaluate(&nextboard, params);
  }
  sortMovesQsort(l, h, moves, evals);
}

struct MinimaxReturn minimaxAlphaBeta(struct Chessboard b, unsigned int d, double alpha, double beta, struct EvalParams* params) {
  
  struct MinimaxReturn m;
  m.move.info = 0;
  if (d == 0 || mateStatus(&b)) {
    m.val = evaluate(&b, params);
    return m;
  }
  
  struct Move nextmoves[400]; // this seems like a reasonable upper bound for the number of legal moves
  int n_moves = putLegalMoves(&b, nextmoves);
  
  // possibly sort by most forcing moves
  if (d > 5) {
    sortMoves(0, n_moves, nextmoves, &b, params);
  }
  
  double val;
  struct Chessboard nextboard;
  
  if (b.toMove) { // minimizing player
    val = MAX;
    for (int i = 0; i < n_moves; i++) {
      doMove(nextmoves[i], &b, &nextboard);
      struct MinimaxReturn t = minimaxAlphaBeta(nextboard, d-1, alpha, beta, params);
      m.move = (m.move.info == 0 || t.val < val) ? nextmoves[i] : m.move;
      val = (t.val < val) ? t.val : val;
      if (val <= alpha) break;
      beta = (val < beta) ? val : beta;
    }
  }
  else { // maximising player
    val = MIN;
    for (int i = n_moves - 1; i >= 0; i--) {
      doMove(nextmoves[i], &b, &nextboard);
      struct MinimaxReturn t = minimaxAlphaBeta(nextboard, d-1, alpha, beta, params);
      m.move = (m.move.info == 0 || t.val > val) ? nextmoves[i] : m.move;
      val = (t.val > val) ? t.val : val;
      if (val >= beta) break;
      alpha = (val > alpha) ? val : alpha;
    }
  }
  

  m.val = val;
  return m;
}




