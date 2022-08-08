#include <stdio.h>
#include "evaluation.h"
#include "chessboard.h"
#include "minimax.h"

int main() {

  struct EvalParams p = defaultParams();
  struct Chessboard board;
  resetChessboard (&board);

  struct Chessboard newboard;
  while (!mateStatus(&board)) {
    struct MinimaxReturn best = minimaxAlphaBeta(board,5,-INFINITY,INFINITY,&p);
    printBoard(&board);
    printf("Eval: %f. Best move = %d,%d\n\n\n",best.val, best.move.info%64, best.move.info/64);
    doMove(best.move, &board, &newboard);
    board = newboard;
  }
  printBoard(&board);


}
