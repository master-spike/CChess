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

struct MinimaxReturn quescienceSearch(struct BitBoard* b, double alpha, double beta, struct TTTable* ttt) {
  
  nodes_in_quescience++;
  struct MinimaxReturn m;
  m.move = 0;
  struct BitBoard nextboards[400]; // this seems like a reasonable upper bound for the number of legal captures

  double b_eval = evaluate(b, alpha, beta);
  
  int cap_restrict = 1;
  double delta = 0;
  
  if (b->ply_count&1) {
    if (b_eval <= alpha) {
      m.val = alpha;
      return m;
    }
    if (b_eval > beta) beta = b_eval;
    delta = b_eval - DELTA_CUTOFF - beta;
  }
  else {
    if (b_eval >= beta) {
      m.val = beta;
      return m;
    }
    if (b_eval < alpha) alpha = b_eval;
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

  struct TTTLookupReturn lookup = tttableLookup(ttt, b, 0);
  if (lookup.valid && lookup.node_type == 0) {
    m.val = lookup.val;
    table_hits++;
    return m;
  }
  if (b->ply_count&1 && lookup.valid && lookup.node_type == 1) {
    m.val = lookup.val;
    table_hits++;
    if (alpha > m.val) {
      table_hits++;
      return m;
    }
  }
  if (!(b->ply_count&1) && lookup.valid && lookup.node_type == 2) {
    m.val = lookup.val;
    table_hits++;
    if (beta < m.val) {
      table_hits++;
      return m;
    }
  }

  m.move = nextboards[0].last_move;
  n_moves = genMoves(b, nextboards, cap_restrict, 0);

  // possibly sort by most forcing moves
  /*
  if (d > 5) {
    sortMoves(0, n_moves, nextmoves, &b, params);
  }
  */
  int node_type = 0;
  double val = evaluate(b, alpha, beta);
  if (b->ply_count&1) { // minimizing player
    beta = (val < beta) ? val : beta;
    for (int i = 0; i < n_moves; i++) {
      struct MinimaxReturn t = quescienceSearch(nextboards+i, alpha, beta, ttt);
      if (t.val <= val) m.move = nextboards[i].last_move;
      val = (t.val < val) ? t.val : val;
      if (val <= alpha) {
        node_type = 1;
        break;
      }
      beta = (val < beta) ? val : beta;
    }
  }
  else { // maximising player
    alpha = (val > alpha) ? val : alpha;
    for (int i = n_moves - 1; i >= 0; i--) {
      struct MinimaxReturn t = quescienceSearch(nextboards+i, alpha, beta, ttt);
      if (t.val >= val) m.move = nextboards[i].last_move;
      val = (t.val > val) ? t.val : val;
      if (val >= beta) {
        node_type = 2;
        break;
      }
      alpha = (val > alpha) ? val : alpha;
    }
  }


  if (node_type <= 2 && cap_restrict <= 1) {
    struct TTTableEntry te;
    te.board = *b;
    te.evaluation = val;
    te.depth = 0;
    te.valid = 1;
    te.node_type = node_type;
    tttableInsert(ttt, te);
  }

  m.val = val;
  return m;
}



struct MinimaxReturn minimaxAlphaBeta(struct BitBoard* b, unsigned int d, int top_d, double alpha, double beta, struct TTTable* ttt, int allow_qs, clock_t start_time, clock_t max_time) {
  
  nodes_in_minimax++;
  


  struct BitBoard nextboards[400]; // this seems like a reasonable upper bound for the number of legal moves
  int n_moves = genMoves(b, nextboards, 0, 1);

  struct MinimaxReturn m;
  m.move = 0;
  if (max_time != 0 && clock() - start_time > max_time) return m;
  if (d == 0 && n_moves != 0) {
    m = (allow_qs) ? quescienceSearch(b, alpha, beta, ttt) : (struct MinimaxReturn) { .val = evaluate(b, alpha, beta)} ;
    return m;
  }

  int node_type = 0;
  uint64_t check = squareAttackedBy(b, findKing(b->pieces[10+(b->ply_count&1)]), 0, b->ply_count&1);
  // if no legal moves check whether its checkmate or stalemate
  if (n_moves == 0) {
    if (check) {
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
    if (alpha > m.val) {
      table_hits++;
      return m;
    }
  }
  if (!(b->ply_count&1) && lookup.valid && lookup.node_type == 2) {
    m.val = lookup.val;
    if (beta < m.val) { 
      table_hits++;
      return m;
    }
  }
  
  // Null move pruning
  if (allow_qs && d > 3 && !check && d <= top_d - 2) {
    struct BitBoard null_move = bbNullMove(b);
    struct MinimaxReturn null_move_value = minimaxAlphaBeta(&null_move, d-3, top_d, alpha, beta, ttt, 0, start_time, max_time);
    if ((b->ply_count&1) && null_move_value.val < alpha) {
      return (struct MinimaxReturn) {.val = alpha};
    }
    else if (!(b->ply_count&1) && null_move_value.val > beta) {
      return (struct MinimaxReturn) {.val = beta};
    }
  }
  

  n_moves = genMoves(b, nextboards, 0, 0);

  int num_best = 0;
  uint16_t best_moves[400];

  double val;
  
  if (b->ply_count&1) { // minimizing player
    val = MAX;
    for (int i = 0; i < n_moves; i++) {
      struct MinimaxReturn t = minimaxAlphaBeta(nextboards+i, d-1, top_d, alpha, beta, ttt, allow_qs, start_time, max_time);
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
      struct MinimaxReturn t = minimaxAlphaBeta(nextboards+i, d-1, top_d, alpha, beta, ttt, allow_qs, start_time, max_time);
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

  struct TTTable tttable = makeTTTable(16, 4);
  struct MinimaxReturn m;
  struct MinimaxReturn m_next = minimaxAlphaBeta(&board, 1, 1, MIN, MAX, &tttable,  1, start_time, 0);
  while(current_time - start_time < max_time && d < max_depth) {
    d++;
    m = m_next;
    m_next = minimaxAlphaBeta(&board, d, d, MIN, MAX, &tttable, 1 ,start_time, max_time);
    current_time = clock();
    clearTTable(&tttable);
  }
  free(tttable.table);
  if (current_time - start_time < max_time) {
    d++;
    m = m_next;
  }
  *depth = d-1;
  return m;
}

struct MinimaxReturn findBestMove(struct BitBoard board, int depth) {

  resetPerfStats();

  struct TTTable tttable = makeTTTable(16, 4);

  struct MinimaxReturn m;

  m = minimaxAlphaBeta(&board, depth, depth, MIN, MAX, &tttable, 1 , 0, 0);
  free(tttable.table);

  return m;
}


