#include <stdlib.h>
#include <stdio.h>
#include "chessboard.h"
#include "evaluation.h"

double def_v_p = 2.0;
double def_v_n = 6.0;
double def_v_b = 6.0;
double def_v_r = 10.0;
double def_v_q = 18.0;
double def_v_k = 0.0;
 
double def_b_pair = 0.25;
double def_p_island = -0.125;
double def_dbl_pawn = -0.125;
double def_opp_sq_val = 0.125;
double def_centre_control = 0.125;
double def_p_adv = 0.0625;

struct EvalParams defaultParams() {
  struct EvalParams p;
  p.v_p = def_v_p;
  p.v_n = def_v_n;
  p.v_b = def_v_b;
  p.v_r = def_v_r;
  p.v_q = def_v_q;
  p.v_k = def_v_k;
  p.b_pair = def_b_pair;
  p.p_island = def_p_island;
  p.dbl_pawn = def_dbl_pawn;
  p.opp_sq_val = def_opp_sq_val;
  p.centre_control = def_centre_control;
  p.p_adv = def_p_adv;
  
  return p;
}

double evaluate(struct Chessboard* board, struct EvalParams* params) {
  
  int ms = mateStatus(board);
  
  if (ms == 1) return (board->toMove) ? MAX : MIN; // checkmate
  else if (ms >= 2) return 0.0; // stalemate or fifty move rule
  double total = 0.0;
  
  if (board->fifty_move_rule > 40) total = (board->toMove) ? (board->fifty_move_rule - 39) : (-1.0 * board->fifty_move_rule - 39);
  
  
  
  unsigned char attacks_w[64];
  unsigned char attacks_b[64];
  
  for (int i = 0; i < 64; i++) {
  
    /*
    attacks_w[i] = sqIsAttacked(i,0,board);
    attacks_b[i] = sqIsAttacked(i,1,board);
    */
    attacks_w[i] = 0;
    attacks_b[i] = 0;
  }
  
  // flags for detecting bishop pair
  char wwb = 0;
  char wbb = 0;
  char bwb = 0;
  char bbb = 0;
  
  for (int i = 0; i < 64; i++) {
    unsigned char piece = board->squares[i];
    double val = 0.0;
    switch (piece & 14) {
      case 2:
        val = params->v_p;
        if (piece == 2) {
          val += params->p_adv*(i/8 - 1);
        }
        if (piece == 3) {
          val += params->p_adv*(6 - i/8);
        }
        break;
      case 4:
        val = params->v_n; break;
      case 6:
        val = params->v_b;
        if (piece & 1) {
          if ((i % 8 + i / 8)&1) bbb = 1;
          else bwb = 1;
        }
        else {
          if ((i % 8 + i / 8)&1) wbb = 1;
          else wwb = 1;
        }
        break;
      case 8:
        val = params->v_r; break;
      case 10:
        val = params->v_q; break;
      case 12:
        val = params->v_k; break;
      default: val = 0.0;
    }
    total += val * ((piece & 1) ? -1: 1);
  }
  
  if (wwb & wbb) total += params->b_pair;
  if (bwb & bbb) total -= params->b_pair;
  

  char wpisl[9];
  char wc = 0;
  char bpisl[9];
  char bc = 0;
  wpisl[0] = 0;
  bpisl[0] = 0;
  for (int i = 0; i < 8; i++) {
    wpisl[i+1] = 0;
    bpisl[i+1] = 0;
    char bpflag = 0;
    char wpflag = 0;
    for (int j = i; j < 64; j+=8) {
      if (board->squares[i] == 2) {
        if (wpflag) total += params->dbl_pawn;
        else wpflag = 1;
        if (!wpisl[i]) wc++;
        wpisl[i+1] = wc;
      }
      if (board->squares[i] == 3) {
        if (bpflag) total -= params->dbl_pawn;
        else bpflag = 1;
        if (!bpisl[i]) bc++;
        bpisl[i+1] = bc;
      }
    }
  }
  
  total += wc * params->p_island;
  total -= wc * params->p_island;
  
  for (int i = 0; i < 32; i++) {
    if (attacks_b[i]) {
      total -= params->opp_sq_val;
    }
  }
  for (int i = 32; i < 64; i++) {
    if (attacks_w[i]) {
      total += params->opp_sq_val;
    }
  }
  for (int i = 16; i < 48; i+=8) {
    for (int j = i + 2; j < i + 6; j++) {
      if (attacks_w[i]) total += params->centre_control; 
      if (attacks_b[i]) total -= params->centre_control;
    }
  }
  return total;
}
