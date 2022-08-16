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

uint64_t nodes_in_minimax;
uint64_t table_hits;
uint64_t nodes_in_quescience;

struct MinimaxReturn quescienceSearch(struct BitBoard* b, double alpha, double beta) {
  
  nodes_in_quescience++;
  struct MinimaxReturn m;
  m.move = 0;
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

struct MinimaxReturn minimaxAlphaBeta(struct BitBoard* b, unsigned int d, double alpha, double beta, struct TTTable* ttt, clock_t start_time, clock_t max_time) {
  
  nodes_in_minimax++;
  


  struct BitBoard nextboards[400]; // this seems like a reasonable upper bound for the number of legal moves
  int n_moves = genMoves(b, nextboards, 0, 1);

  struct MinimaxReturn m;
  m.move = 0;
  if (max_time != 0 && clock() - start_time > max_time) return m;
  if (d == 0 && n_moves != 0) {
    m = quescienceSearch(b, alpha, beta);
    return m;
  }

  int node_type = 0;
  // if no legal moves check whether its checkmate or stalemate
  if (n_moves == 0) {
    if (squareAttackedBy(b, findKing(b->pieces[10+(b->ply_count&1)]), 0, b->ply_count&1)) {
      m.val = (b->ply_count & 1) ? MAX : MIN;
    }
    else {
      m.val = 0;
    }
    return m;
  }

  struct TTTLookupReturn lookup = tttableLookup(ttt, b, d);
  if (lookup.valid && lookup.node_type == 0) {
    m.val = lookup.val;
    table_hits++;
    return m;
  }
  if (b->ply_count&1 && lookup.valid && lookup.node_type == 1) {
    m.val = lookup.val;
    table_hits++;
    return m;
  }
  if (!(b->ply_count&1) && lookup.valid && lookup.node_type == 2) {
    m.val = lookup.val;
    table_hits++;
    return m;
  }

  n_moves = genMoves(b, nextboards, 0, 0);

  int num_best = 0;
  uint16_t best_moves[400];

  double val;
  
  if (b->ply_count&1) { // minimizing player
    val = MAX;
    for (int i = 0; i < n_moves; i++) {
      struct MinimaxReturn t = minimaxAlphaBeta(nextboards+i, d-1, alpha, beta, ttt, start_time, max_time);
      if (t.val == val) {
        best_moves[num_best] = nextboards[i].last_move;
        num_best++;
      }
      if (t.val < val) {
        best_moves[0] = nextboards[i].last_move;
        num_best = 1;
      }
      val = (t.val < val) ? t.val : val;
      if (val < alpha) {
        node_type = 1;
        break;
      }
      beta = (val < beta) ? val : beta;
    }
  }
  else { // maximising player
    val = MIN;
    for (int i = n_moves - 1; i >= 0; i--) {
      struct MinimaxReturn t = minimaxAlphaBeta(nextboards+i, d-1, alpha, beta, ttt, start_time, max_time);
      if (t.val == val) {
        best_moves[num_best] = nextboards[i].last_move;
        num_best++;
      }
      if (t.val > val) {
        best_moves[0] = nextboards[i].last_move;
        num_best = 1;
      }
      val = (t.val > val) ? t.val : val;
      if (val > beta) {
        node_type = 2;
        break;
      }
      alpha = (val > alpha) ? val : alpha;
    }
  }
  
  if (node_type == 0) {
    if (val < alpha) node_type = 3;
    if (val > beta) node_type = 4;
  }

  struct TTTableEntry te;
  te.board = *b;
  te.depth = d;
  te.evaluation = val;
  te.node_type = node_type;
  te.valid = 1;

  if (node_type <= 2) tttableInsert(ttt, te);

  m.val = val;
  m.move = best_moves[(uint8_t) (rand() % num_best)];
  return m;
}

void resetPerfStats() {
  table_hits = 0;
  nodes_in_minimax = 0;
  nodes_in_quescience = 0;
}

struct MinimaxReturn timedIterativeDeepening(struct BitBoard board, clock_t max_time, unsigned int* depth, int max_depth) {
  unsigned int d = 2;
  clock_t start_time = clock();
  clock_t current_time = start_time;

  resetPerfStats();

  struct TTTable tttable = makeTTTable(16, 16);
  struct MinimaxReturn m;
  struct MinimaxReturn m_next = minimaxAlphaBeta(&board, 1, MIN, MAX, &tttable, start_time, 0);
  while(current_time - start_time < max_time && d <= max_depth) {
    d++;
    m = m_next;
    m_next = minimaxAlphaBeta(&board, d, MIN, MAX, &tttable ,start_time, max_time);
    current_time = clock();
  }
  free(tttable.table);
  if (current_time - start_time < max_time) {
    d++;
    m = m_next;
  }
  *depth = d-1;
  return m;
}

