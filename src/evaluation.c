#include <stdlib.h>
#include <stdio.h>
#include "evaluation.h"
#include "bitboard.h"


const double v_p = 10.0;
const double v_n = 30.0;
const double v_b = 30.0;
const double v_r = 50.0;
const double v_q = 90.0;

const double bishop_pair = 2.5;

const uint64_t light_squares = 0xAA55AA55AA55AA55ULL;
const uint64_t dark_squares = 0x55AA55AA55AA55AAULL;

int countBits(uint64_t n) {
  int c = 0;
  while(n) {
    c++;
    n = n&(n-1);
  }
  return c;
}

double evaluate(struct BitBoard* board) {
  
  int in_check = (squareAttackedBy(board, findKing(board->pieces[10+board->ply_count]), 0));
  double total = 0;


  // simple material count
  total += v_p * (countBits(board->pieces[0]) - countBits(board->pieces[1]));
  total += v_n * (countBits(board->pieces[2]) - countBits(board->pieces[3]));
  total += v_b * (countBits(board->pieces[4]) - countBits(board->pieces[5]));
  total += v_r * (countBits(board->pieces[6]) - countBits(board->pieces[7]));
  total += v_q * (countBits(board->pieces[8]) - countBits(board->pieces[9]));
  
  if (light_squares & board->pieces[4] && dark_squares & board->pieces[4]) total += bishop_pair;
  if (light_squares & board->pieces[5] && dark_squares & board->pieces[5]) total -= bishop_pair;

  


  return total;
}
