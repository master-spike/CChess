#include <stdio.h>
#include <time.h>
#include "evaluation.h"
#include "bitboard.h"
#include "minimax.h"



int main() {
  srand(time(NULL));
  struct BitBoard board = startBoard();
  for (int i = 1; i < 6; i++) {
    printf("%d,%ld\n", i,perft(kiwipete(), i));
  }

  
  printf("\n\n");
  while (1) {
    printBitBoard(board);
    struct MinimaxReturn best = minimaxAlphaBeta(&board,6,-INFINITY,INFINITY);
    printf("Eval: %f. Best move = %d,%d\n\n\n",best.val, best.move%64, (best.move/64)%64);
    if(!best.move) break;
    board = doMove(&board, best.move);
  }
  printBitBoard(board);
  

}
