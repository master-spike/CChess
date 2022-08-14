#include <stdio.h>
#include <time.h>
#include "evaluation.h"
#include "bitboard.h"
#include "minimax.h"



int main() {
  srand(time(NULL));
  struct BitBoard board = startBoard();

  for (int i = 1; i < 6; i++) {
    perft(kiwipete(), i);
  }
  printf("\n\n");

  while (1) {
    printBitBoard(board);
    nodes_in_minimax = 0;
    nodes_in_quescience = 0;
    struct MinimaxReturn best = minimaxAlphaBeta(&board,5,-INFINITY,INFINITY);
    printf("Eval: %f. Best move = %d,%d. Total nodes: %lu of which %lu were in quiescence search. \n\n\n",best.val, best.move%64, (best.move/64)%64, nodes_in_minimax + nodes_in_quescience, nodes_in_quescience);
    if(!best.move) break;
    board = doMove(&board, best.move);
  }
  printBitBoard(board);
  

}
