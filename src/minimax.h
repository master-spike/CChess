#ifndef MINIMAX_H_GUARD
#define MINIMAX_H_GUARD

#include <stdint.h>
#include <time.h>
#include "evaluation.h"
#include "types.h"
#include "bitboard.h"
#include "transpostable.h"


extern uint64_t nodes_in_minimax;
extern uint64_t nodes_in_quescience;
extern uint64_t table_hits;

struct MinimaxReturn minimaxAlphaBeta(struct BitBoard* b, unsigned int d, int top_d, double alpha, double beta, struct TTTable* ttt, int allow_qs, clock_t start_time, clock_t max_time);
struct MinimaxReturn timedIterativeDeepening(struct BitBoard board, clock_t max_time, unsigned int *depth, int max_depth);
struct MinimaxReturn findBestMove(struct BitBoard board, int depth);
struct MinimaxReturn idsAspirational(struct BitBoard board);


#endif
