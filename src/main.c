#include <stdio.h>
#include <time.h>
#include "evaluation.h"
#include "bitboard.h"
#include "minimax.h"



int main() {
  srand(time(NULL));
  struct BitBoard board = startBoard();
  for (int i = 1; i < 6; i++) {
    printf("%d,%ld\n", i,perft(startBoard(), i));
  }
  /*
  struct BitBoard testpos;
  testpos.pieces[0] = 0x0000000004208900ULL;
  testpos.pieces[1] = 0x00D7000000000000ULL;
  testpos.pieces[2] = 0x0000000000000140ULL;
  testpos.pieces[3] = 0x0000000040000000ULL;
  testpos.pieces[4] = 0x0000000000000208ULL;
  testpos.pieces[5] = 0x2000000000080000ULL;
  testpos.pieces[6] = 0x0000000000000081ULL;
  testpos.pieces[7] = 0x8100000000000000ULL;
  testpos.pieces[8] = 0x0000000000400000ULL;
  testpos.pieces[9] = 0x0000000000000020ULL;
  testpos.pieces[10] =0x0000000000000004ULL;
  testpos.pieces[11] =0x0800000000000000ULL;
  testpos.c_rights = 12;
  testpos.enpassant = 8;
  testpos.ply_count = 1;
  testpos.last_move = 18 + 26*64;

  printBitBoard(testpos);

  struct BitBoard tb_succs[50];
  int n = genMoves(&testpos, tb_succs, 0, 1);
  
  board = testpos;
  
  for (int i = 0; i < n; i++) {
    printBitBoard(tb_succs[i]);
  }
  */
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
