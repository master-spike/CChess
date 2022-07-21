#include <stdio.h>
#include "evaluation.h"
#include "chessboard.h"

// ruy lopez berlin with bxc6
unsigned short moveseq[15] = {1739,2291,1153,2942,2434,2745,67,2356,
                              2918,2934,2331,2413,2852,2933,3372};

int main() {

  struct EvalParams p = defaultParams();
  struct Chessboard board;
  resetChessboard (&board);
  printBoard(&board);
  printf("eval: %f\n",evaluate(&board,&p));
  printf("\n");
  struct Chessboard newboard;
  for (int i = 0; i < 15; i++) {
    struct Move move;
    move.info = moveseq[i];
    if(!doMove(move,&board,&newboard)){
      board = newboard;
      printf("\n");
      printBoard(&board);
      printf("eval: %f\n",evaluate(&board,&p));
      printf("\n");  
    }
    else printf ("\n INVALID MOVE %d \n", moveseq[i]);

  }
  struct Move s[4096];
  char stro[3] = "  ";
  char strd[3] = "  ";
  int z = putLegalMoves(&board,s);
  for (int i = 0; i < z; i++) {
    if (!isIllegalMove(s[i], &board)) {
      writeCoord(s[i].info & 63, stro);
      writeCoord((s[i].info >> 6) & 63, strd);
      printf("%s -> %s | ", stro, strd);
    }
  }


}
