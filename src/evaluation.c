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

const double centre_dist[64] = {3, 3, 3, 3, 3, 3, 3, 3,
                             3, 2, 2, 2, 2, 2, 2, 3,
                             3, 2, 1, 1, 1, 1, 2, 3,
                             3, 2, 1, 0, 0, 1, 2, 3,
                             3, 2, 1, 0, 0, 1, 2, 3,
                             3, 2, 1, 1, 1, 1, 2, 3,
                             3, 2, 2, 2, 2, 2, 2, 3,
                             3, 3, 3, 3, 3, 3, 3, 3 };

const double corner_dist[64] = {0, 1, 2, 3, 3, 2, 1, 0,
                                1, 2, 3, 4, 4, 3, 2, 1,
                                2, 3, 4, 5, 5, 4, 3, 2,
                                3, 4, 5, 6, 6, 5, 4, 3,
                                3, 4, 5, 6, 6, 5, 4, 3,
                                2, 3, 4, 5, 5, 4, 3, 2,
                                1, 2, 3, 4, 4, 3, 2, 1,
                                0, 1, 2, 3, 3, 2, 1, 0
};

int sgn(int x)
{
  return (x < 0) ? -1 : (x == 0) ? 0
                                 : 1;
}

double evaluate(struct BitBoard *board, double alpha, double beta)
{
  double init_mat_total = v_p*16 + v_n * 4 + v_b * 4 + v_r * 4 + v_q * 2;
  //int in_check = (squareAttackedBy(board, findKing(board->pieces[10 + (board->ply_count & 1)]), 0, (board->ply_count&1)));
  double total = 0;

  // simple material count
  total += v_p * (countBits(board->pieces[0]) - countBits(board->pieces[1]));
  total += v_n * (countBits(board->pieces[2]) - countBits(board->pieces[3]));
  total += v_b * (countBits(board->pieces[4]) - countBits(board->pieces[5]));
  total += v_r * (countBits(board->pieces[6]) - countBits(board->pieces[7]));
  total += v_q * (countBits(board->pieces[8]) - countBits(board->pieces[9]));

  double total_material = v_p * (countBits(board->pieces[0]) + countBits(board->pieces[1]));
  total_material  += v_n * (countBits(board->pieces[2]) + countBits(board->pieces[3]));
  total_material += v_b * (countBits(board->pieces[4]) + countBits(board->pieces[5]));
  total_material += v_r * (countBits(board->pieces[6]) + countBits(board->pieces[7]));
  total_material += v_q * (countBits(board->pieces[8]) + countBits(board->pieces[9]));


  if (light_squares & board->pieces[4] && dark_squares & board->pieces[4])
    total += bishop_pair;
  if (light_squares & board->pieces[5] && dark_squares & board->pieces[5])
    total -= bishop_pair;

  if (total <= alpha - 340.0) return total;
  if (total >= beta + 340.0) return total;

  for (int i = 0; i < 64; i++) {
    int r = i/8;
    int bi = (7-r)*8 + i % 8;
    total += piece_square_pawn[63-i]*((board->pieces[0] >> i) & 1);
    total += piece_square_knight[63-i]*((board->pieces[2] >> i) & 1);
    total += piece_square_bishop[63-i]*((board->pieces[4] >> i) & 1);
    total += piece_square_rook[63-i]*((board->pieces[6] >> i) & 1);
    total += piece_square_queen[63-i]*((board->pieces[8] >> i) & 1);
    total -= piece_square_pawn[63-bi]*((board->pieces[1] >> i) & 1);
    total -= piece_square_knight[63-bi]*((board->pieces[3] >> i) & 1);
    total -= piece_square_bishop[63-bi]*((board->pieces[5] >> i) & 1);
    total -= piece_square_rook[63-bi]*((board->pieces[7] >> i) & 1);
    total -= piece_square_queen[63-bi]*((board->pieces[9] >> i) & 1);
  }
  int whitekingpos = findKing(board->pieces[10]);
  int blackkingpos = findKing(board->pieces[11]);
  
  double endgame_interp = 1.5 * total_material / init_mat_total;
  
  total += 35 * (endgame_interp * centre_dist[whitekingpos] + (1-endgame_interp) * corner_dist[whitekingpos]);
  total -= 35 * (endgame_interp * centre_dist[blackkingpos] + (1-endgame_interp) * corner_dist[blackkingpos]);

  if (total <= alpha - 170.0) return total;
  if (total >= beta + 170.0) return total;

  int sq_control[64];
  
  
  for (int i = 0; i < 64; i++)
  {
    sq_control[i] = countBits(squareAttackedBy(board, i, 1, 1)) - countBits(squareAttackedBy(board, i, 1, 0));
    sq_control[i] += sgn(sq_control[i]); // diminishing returns
  }

  for (int i = 0; i < 64; i++) {
    total += ((3 - centre_dist[i]) * sq_control[i]) * 3;
  }

  return total;
}


