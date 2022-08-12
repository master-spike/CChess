#include <stdlib.h>
#include <stdio.h>
#include "patterns.h"
#include "evaluation.h"
#include "bitboard.h"

const double v_p = 100.0;
const double v_n = 320.0;
const double v_b = 330.0;
const double v_r = 500.0;
const double v_q = 900.0;

const double bishop_pair = 25;

const uint64_t light_squares = 0xAA55AA55AA55AA55ULL;
const uint64_t dark_squares = 0x55AA55AA55AA55AAULL;

const int centre_dist[64] = {3, 3, 3, 3, 3, 3, 3, 3,
                             3, 2, 2, 2, 2, 2, 2, 3,
                             3, 2, 1, 1, 1, 1, 2, 3,
                             3, 2, 1, 0, 0, 1, 2, 3,
                             3, 2, 1, 0, 0, 1, 2, 3,
                             3, 2, 1, 1, 1, 1, 2, 3,
                             3, 2, 2, 2, 2, 2, 2, 3,
                             3, 3, 3, 3, 3, 3, 3, 3 };

int sgn(int x)
{
  return (x < 0) ? -1 : (x == 0) ? 0
                                 : 1;
}

double evaluate(struct BitBoard *board, double alpha, double beta)
{
  
  //int in_check = (squareAttackedBy(board, findKing(board->pieces[10 + (board->ply_count & 1)]), 0, (board->ply_count&1)));
  double total = 0;

  // simple material count
  total += v_p * (countBits(board->pieces[0]) - countBits(board->pieces[1]));
  total += v_n * (countBits(board->pieces[2]) - countBits(board->pieces[3]));
  total += v_b * (countBits(board->pieces[4]) - countBits(board->pieces[5]));
  total += v_r * (countBits(board->pieces[6]) - countBits(board->pieces[7]));
  total += v_q * (countBits(board->pieces[8]) - countBits(board->pieces[9]));

  if (light_squares & board->pieces[4] && dark_squares & board->pieces[4])
    total += bishop_pair;
  if (light_squares & board->pieces[5] && dark_squares & board->pieces[5])
    total -= bishop_pair;
  /*
  int sq_control[64];
  
  
  for (int i = 0; i < 64; i++)
  {
    sq_control[i] = countBits(squareAttackedBy(board, i, 1, 1)) - countBits(squareAttackedBy(board, i, 1, 0));
    sq_control[i] += sgn(sq_control[i]); // diminishing returns
  }

  for (int i = 0; i < 64; i++) {
    total += ((3 - sq_control[i]) * centre_dist[i]) * 0.1;
  } */

  if (total <= alpha - 400.0) return total;
  if (total >= beta + 400.0) return total;

  for (int i = 0; i < 64; i++) {
    int r = 56 + i % 8 - i;
    int bi = r + i % 8;
    total += piece_square_pawn[63-i]*((board->pieces[0] >> i) & 1);
    total += piece_square_knight[63-i]*((board->pieces[2] >> i) & 1);
    total += piece_square_bishop[63-i]*((board->pieces[4] >> i) & 1);
    total += piece_square_rook[63-i]*((board->pieces[6] >> i) & 1);
    total += piece_square_queen[63-i]*((board->pieces[8] >> i) & 1);
    total -= piece_square_pawn[bi]*((board->pieces[1] >> i) & 1);
    total -= piece_square_knight[bi]*((board->pieces[3] >> i) & 1);
    total -= piece_square_bishop[bi]*((board->pieces[5] >> i) & 1);
    total -= piece_square_rook[bi]*((board->pieces[7] >> i) & 1);
    total -= piece_square_queen[bi]*((board->pieces[9] >> i) & 1);
  }

  return total;
}


