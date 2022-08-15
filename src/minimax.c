#include <stdlib.h>
#include <time.h>
#include "minimax.h"

#define DELTA_CUTOFF 192.0

void swapBBoards(struct BitBoard* b1, struct BitBoard* b2) {
  struct BitBoard t = *b1;
  *b1 = *b2;
  *b2 = t;
}

void swapDouble(double* d1, double* d2) {
  double t = *d1;
  *d1 = *d2;
  *d2 = t;
}

/*
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
*/

struct MinimaxReturn quescienceSearch(struct BitBoard* b, double alpha, double beta) {
  
  nodes_in_quescience++;
  
  struct BitBoard nextboards[400]; // this seems like a reasonable upper bound for the number of legal captures

  double b_eval = evaluate(b, alpha, beta);
  
  int cap_restrict = 1;
  double delta = 0;
  if (b->ply_count&1) {
    delta = b_eval - DELTA_CUTOFF - beta;
  }
  else {
    delta = alpha + b_eval - DELTA_CUTOFF;
  }

  if (delta > 100.0) cap_restrict++;
  if (delta > 320) cap_restrict++;
  if (delta > 330) cap_restrict++;
  if (delta > 500) cap_restrict++;

  int n_moves = genMoves(b, nextboards, 0, 1);

  struct MinimaxReturn m;

  if (n_moves == 0) {
    if (squareAttackedBy(b, findKing(b->pieces[10+(b->ply_count&1)]), 0, b->ply_count&1)) {
      m.val = (b->ply_count & 1) ? MAX : MIN;
    }
    else {
      m.val = b_eval;
    }
    return m;
  }

  m.move = nextboards[0].last_move;
  n_moves = genMoves(b, nextboards, cap_restrict, 0);

  // possibly sort by most forcing moves
  /*
  if (d > 5) {
    sortMoves(0, n_moves, nextmoves, &b, params);
  }
  */

  double val = evaluate(b, alpha, beta);
  if (b->ply_count&1) { // minimizing player
    beta = (val < beta) ? val : beta;
    for (int i = 0; i < n_moves; i++) {
      struct MinimaxReturn t = quescienceSearch(nextboards+i, alpha, beta);
      if (t.val <= val) m.move = nextboards[i].last_move;
      val = (t.val < val) ? t.val : val;
      if (val <= alpha) break;
      beta = (val < beta) ? val : beta;
    }
  }
  else { // maximising player
    alpha = (val > alpha) ? val : alpha;
    for (int i = n_moves - 1; i >= 0; i--) {
      struct MinimaxReturn t = quescienceSearch(nextboards+i, alpha, beta);
      if (t.val >= val) m.move = nextboards[i].last_move;
      val = (t.val > val) ? t.val : val;
      if (val >= beta) break;
      alpha = (val > alpha) ? val : alpha;
    }
  }

  m.val = val;
  return m;
}

struct MinimaxReturn minimaxAlphaBeta(struct BitBoard* b, unsigned int d, double alpha, double beta, clock_t max_time) {
  
  nodes_in_minimax++;
  


  struct BitBoard nextboards[400]; // this seems like a reasonable upper bound for the number of legal moves
  int n_moves = genMoves(b, nextboards, 0, 1);

  struct MinimaxReturn m;
  m.move = 0;
  if (clock() > max_time && max_time != 0) return m;
  if (d == 0 && n_moves != 0) {
    m = quescienceSearch(b, alpha, beta);
    return m;
  }
  

  
  if (n_moves == 0) {
    if (squareAttackedBy(b, findKing(b->pieces[10+(b->ply_count&1)]), 0, b->ply_count&1)) {
      m.val = (b->ply_count & 1) ? MAX : MIN;
    }
    else {
      m.val = 0;
    }
    return m;
  }

  n_moves = genMoves(b, nextboards, 0, 0);

  // possibly sort by most forcing moves
  /*
  if (d > 5) {
    sortMoves(0, n_moves, nextmoves, &b, params);
  }
  */


  int num_best = 0;
  uint16_t best_moves[400];

  double val;
  
  if (b->ply_count&1) { // minimizing player
    val = MAX;
    for (int i = 0; i < n_moves; i++) {
      struct MinimaxReturn t = minimaxAlphaBeta(nextboards+i, d-1, alpha, beta, max_time);
      if (t.val == val) {
        best_moves[num_best] = nextboards[i].last_move;
        num_best++;
      }
      if (t.val < val) {
        best_moves[0] = nextboards[i].last_move;
        num_best = 1;
      }
      val = (t.val < val) ? t.val : val;
      if (val < alpha) break;
      beta = (val < beta) ? val : beta;
    }
  }
  else { // maximising player
    val = MIN;
    for (int i = n_moves - 1; i >= 0; i--) {
      struct MinimaxReturn t = minimaxAlphaBeta(nextboards+i, d-1, alpha, beta, max_time);
      if (t.val == val) {
        best_moves[num_best] = nextboards[i].last_move;
        num_best++;
      }
      if (t.val > val) {
        best_moves[0] = nextboards[i].last_move;
        num_best = 1;
      }
      val = (t.val > val) ? t.val : val;
      if (val > beta) break;
      alpha = (val > alpha) ? val : alpha;
    }
  }
  

  m.val = val;
  m.move = best_moves[(uint8_t) (rand() % num_best)];
  return m;
}

struct MinimaxReturn timedIterativeDeepening(struct BitBoard board, clock_t max_time) {
  int depth = 2;
  clock_t start_time = clock();
  clock_t current_time = start_time;


  struct MinimaxReturn m;
  struct MinimaxReturn m_next = minimaxAlphaBeta(&board, 1, MIN, MAX, 0);
  while(current_time - start_time < max_time) {
    depth++;
    m = m_next;
    m_next = minimaxAlphaBeta(&board, depth, MIN, MAX, start_time + max_time);
    current_time = clock();
  }
  return m;
}

