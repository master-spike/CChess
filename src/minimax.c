#include "minimax.h"

void sortMovesQsort(int l, int h, struct Move* moves, double* evals) {
  if (h - l <= 1) return;
  if (h - l == 2) {
    if (evals[l] <= evals[l+1]) return;
    struct Move t = moves[l];
    moves[l] = moves[l+1];
    moves[l+1] = t;
    double td = evals[l];
    evals[l] = evals[l+1];
    evals[l+1] = td;
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
    struct Move t = moves[x];
    moves[x] = moves[y];
    moves[y] = t;
    double td = evals[x];
    evals[x] = evals[y];
    evals[y] = td;
    x++; y--;
  }
  int m = x;
  if (x==y) {
    double v = evals[x];
    if (v > pivot) m = x-1;
  }
  struct Move t = moves[m];
  moves[m] = moves[l];
  moves[l] = t;
  double td = evals[m];
  evals[m] = evals[l];
  evals[l] = td;
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
  if (d > 3) {
    sortMoves(0, n_moves, nextmoves, &b, params);
  }
  
  double val;
  struct Chessboard nextboard;
  
  if (b.toMove) { // minimizing player
    val = MAX;
    for (int i = n_moves - 1; i >= 0; i--) {
      doMove(nextmoves[i], &b, &nextboard);
      struct MinimaxReturn t = minimaxAlphaBeta(nextboard, d-1, alpha, beta, params);
      m.move = (m.move.info == 0 || t.val <= val) ? nextmoves[i] : m.move;
      val = (t.val < val) ? t.val : val;
      if (val <= alpha) break;
      beta = (val < beta) ? val : beta;
    }
  }
  else { // maximising player
    val = MIN;
    for (int i = 0; i < n_moves; i++) {
      doMove(nextmoves[i], &b, &nextboard);
      struct MinimaxReturn t = minimaxAlphaBeta(nextboard, d-1, alpha, beta, params);
      m.move = (m.move.info == 0 || t.val >= val) ? nextmoves[i] : m.move;
      val = (t.val > val) ? t.val : val;
      if (val >= beta) break;
      alpha = (val > alpha) ? val : alpha;
    }
  }
  

  m.val = val;
  return m;
}




