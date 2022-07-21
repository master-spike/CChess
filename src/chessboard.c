#include <stdio.h>
#include "chessboard.h"

unsigned int absDiff(int i, int j) {
  return (i - j < 0) ? (j - i) : (i - j);
}

unsigned char isIllegalPawnMove(struct Move move, struct Chessboard* board) {
  // we know that origin refs a pawn as this is called from isIllegal;
  // we also check for pins and checks prior to calling this.
   
  unsigned char o = (move.info & 63);
  unsigned char d = (move.info >> 6) & 63;
    
  unsigned char pieceColor = (board->squares[o] & 1);
  unsigned char targetP = board->squares[d];
    
  char ofile = o % 8;
  char dfile = d % 8;
  char orank = o / 8;
  char drank = d / 8;
    
  char movDir = (pieceColor) ? -1 : 1; 
    
    // check color and compare to player to move
  if (absDiff((char) ofile, (char) dfile) >= 2) return 1; // cannot move sideways this far as pawn
  if (absDiff((char) ofile, (char) dfile) == 1) {
    // if moving diagonally check if its going one square 'forward'.
    if (drank - orank != movDir) return 1;
    // now we know its moving 1 left/right and 1 forward for its resp color. Check for valid capture.
    // en-passant case
    if (targetP == 0) {
      if (board->enpassant != dfile) return 64;
      if (4 - pieceColor != orank) return 64;
    }
    // not en-passant
    else {
      if (targetP & 1 == pieceColor) return 8;
    }
  }
  else {
    // moving straight
    // check dest matches
    int dist = movDir * (drank - orank);
    unsigned char homerank = 1 + 5 * pieceColor;
    if (dist <= 0 || dist > 2) return 1;
    if (dist == 2 && orank != homerank) return 1;
    if (targetP != 0 || board->squares[(char) o + 8 * movDir] != 0) return 8;
  }
    
  return 0;
}

unsigned char isIllegalKnightMove (struct Move move, struct Chessboard* board) {
  
  unsigned char o = (move.info & 63);
  unsigned char d = (move.info >> 6) & 63;
    
  unsigned char pieceColor = (board->squares[o] & 1);
  unsigned char targetP = board->squares[d];
    
  char ofile = o % 8;
  char dfile = d % 8;
  char orank = o / 8;
  char drank = d / 8;
    
  unsigned int vd = absDiff(orank, drank);
  unsigned int hd = absDiff(ofile, dfile);
  if ( vd == 0 || hd == 0 || vd > 2 || hd > 2) return 1;
  if ((vd == 1 && !(hd == 2)) || (vd == 2 && !(hd == 1))) return 1;
  if (targetP != 0 && ((targetP & 1) == pieceColor)) return 8; 
  
  return 0;
}

unsigned char isIllegalRookMove (struct Move move, struct Chessboard* board) {
  
  unsigned char o = (move.info & 63);
  unsigned char d = (move.info >> 6) & 63;
    
  unsigned char pieceColor = (board->squares[o] & 1);
  unsigned char targetP = board->squares[d];
    
  char ofile = o % 8;
  char dfile = d % 8;
  char orank = o / 8;
  char drank = d / 8;
  
  if (ofile != dfile && orank != drank) return 1;
  if (targetP != 0 && ((targetP & 1) == pieceColor)) return 8;
  int hdir = (dfile > ofile) ? 1 : ((dfile < ofile) ? -1 : 0);
  int vdir = (drank > orank) ? 1 : ((drank < orank) ? -1 : 0);
  
  while(1) {
    o = o + hdir + vdir * 8;
    if (o == d) break;
    if (board->squares[o] != 0) return 8; 
  }
  
  return 0;
}

unsigned char isIllegalBishopMove (struct Move move, struct Chessboard* board) {
  
  unsigned char o = (move.info & 63);
  unsigned char d = (move.info >> 6) & 63;
    
  unsigned char pieceColor = (board->squares[o] & 1);
  unsigned char targetP = board->squares[d];
    
  char ofile = o % 8;
  char dfile = d % 8;
  char orank = o / 8;
  char drank = d / 8;
    
  unsigned int vd = absDiff(orank, drank);
  unsigned int hd = absDiff(ofile, dfile);
  
  if (vd != hd) return 1;
  int hdir = (dfile > ofile) ? 1 : ((dfile < ofile) ? -1 : 0);
  int vdir = (drank > orank) ? 1 : ((drank < orank) ? -1 : 0);
  
  while(1) {
    o = o + hdir + vdir * 8;
    if (o == d) break;
    if (board->squares[o] != 0) return 8; 
  }
  
  return 0;
}


// player is "attacking" color
int diagAttack(unsigned char sq, int player, struct Chessboard* b) {
  player = (player) ? 1 : 0;
  char sqr = sq / 8;
  char sqf = sq % 8;
  unsigned char q_c = 10 + player;
  unsigned char b_c = 6 + player;
  for (char d = 0; d < 4; d++) {
    char dh = 1 - 2 * (d & 1);
    char dv = 1 - 2 * (d >> 1);
    int cr = sqr + dv;
    int cf = sqf + dh;
    while (cr >= 0 && cr < 8 && cf >= 0 && cf < 8) {
      if (b->squares[cr*8 + cf] != 0) {
        unsigned char t = b->squares[cr*8 + cf];
        if (t == q_c || t == b_c) return 1;
        else break;

      }
      cr += dv;
      cf += dh;
    }
  }
  return 0;
}

int lateralAttack(unsigned char sq, int player, struct Chessboard* b) {
  player = (player) ? 1 : 0;
  unsigned char r_c = 8 + player;
  unsigned char q_c = 10 + player;
  char sqr = sq / 8;
  char sqf = sq % 8;
  for (int d = -1; d <= 1; d += 2) {
    if (d == 0) continue;
    int cf = sqf + d;
    while (cf < 8 && cf >= 0) {
      unsigned char t = b->squares[sqr*8 + cf];
      if (t != 0) {
        if (t == q_c || t == r_c) return 1;
        else break;
      }
      cf += d;
    }
    int cr = sqr + d;
    while (cr < 8 && cr >= 0) {
      unsigned char t = b->squares[sqf + 8 * cr];
      if (t != 0) {
        if (t == q_c || t == r_c) return 1;
        else break;
      }
      cr += d;
    }
  }
  return 0;
}

int knightAttack(unsigned char sq, int player, struct Chessboard* b) {
  player = (player) ? 1 : 0;
  unsigned char n_c = 4 + player;
  char sqr = sq / 8;
  char sqf = sq % 8;
  char x;
  char y;
  unsigned char ldir = 0;
  unsigned char count = 0;
  for (int d = 0; d < 4; d++) {
    x = 2 - ((3 * d) / 2);
    y = 3 - x;
    if (sqr + x < 8 && sqr + x >= 0 && sqf + y < 8 && sqf + y >= 0) {
      if (b->squares[8*(sqr+x) + sqf + y] == n_c) {
        count++;
        ldir = 8*(sqr+x) + sqf + y;
      }
    }
    y = -y;
    if (sqr + x < 8 && sqr + x >= 0 && sqf + y < 8 && sqf + y >= 0) {
      if (b->squares[8*(sqr+x) + sqf + y] == n_c) {
        count++;
        ldir = 8*(sqr+x) + sqf + y;
      }
    }
  }
  
  return ldir + (count << 8);
}

int pawnAttack(unsigned char sq, int player, struct Chessboard* b) {
  player = (player) ? 1 : 0;
  int d = -1 + 2 * player;
  unsigned char p_c = 2 + player;
  unsigned char a1 = sq + 7 * d;
  unsigned char a2 = sq + 9 * d;
  if (a1 >= 0 && a1 < 64) {
    if (b->squares[a1] == p_c && absDiff(a1/8, sq/8) == 1) return 1;
  }
  if (a2 >= 0 && a2 < 64) {
    if (b->squares[a2] == p_c && absDiff(a2/8, sq/8) == 1) return 1;
  }
  
  
  return 0;
}

int kingAttack(unsigned char sq, int player, struct Chessboard* b) {
  player = (player) ? 1 : 0;
  unsigned char k_c = 12 + player;
  for (int i = -1; i<=1; i++) {
    for (int j = -1; j<=1; j++) {
      if (i == 0 && j == 0) continue;
      unsigned char t = sq + j + 8*i;
      if (t < 0 || t >= 64) continue;
      if (absDiff(sq/8,t/8) > 1 || absDiff(sq%8,t%8) > 1) continue;
      if (b->squares[t] == k_c) {
        return 1;
      }
    }
  }
  return 0;
}

// player is the "attacking" color
int sqIsAttacked(unsigned char sq, int player, struct Chessboard* b) {
  int p = pawnAttack(sq, player, b) ? 1 : 0;
  int n = knightAttack(sq, player, b) ? 2 : 0;
  int l = lateralAttack(sq, player, b) ? 4 : 0;
  int d = diagAttack(sq, player, b) ? 8 : 0;
  int k = kingAttack(sq, player, b) ? 16 : 0;
  return pawnAttack(sq, player, b) || knightAttack(sq, player, b)
       || lateralAttack(sq, player, b) || diagAttack(sq, player, b) || kingAttack(sq, player, b);  
}

unsigned char isIllegalKingMove(struct Move move, struct Chessboard* board) {
  unsigned char o = (move.info & 63);
  unsigned char d = (move.info >> 6) & 63;
    
  unsigned char pieceColor = (board->squares[o] & 1);
  unsigned char targetP = board->squares[d];
    
  char ofile = o % 8;
  char dfile = d % 8;
  char orank = o / 8;
  char drank = d / 8;
    
  unsigned int vd = absDiff(orank, drank);
  unsigned int hd = absDiff(ofile, dfile);
  
  
  unsigned char opp_col = (pieceColor+1)&1;
  
  if (hd == 2 && vd == 0) {
    // 1 -> kingside castle file | 5 -> queenside file
    unsigned char crb = 1 << ((dfile - 1) / 4 + 2 * pieceColor); //this is the "bit" to check castling rights with.
    if (! (crb & board->castling_rights)) return 32;
    if (sqIsAttacked(o, opp_col, board)) return 16;
    int dir = (3 - dfile) / 2;
    int k = 1;
    while (dfile + dir*k != 3) {
      if (board->squares[drank*8 + dfile + dir*k] != 0) return 8;
      k++;
    }
    k = 1; dir = -dir;
    while (k <= 2) {
      if (sqIsAttacked(drank*8 + 3 + dir*k, opp_col, board)) return 16;
      k++;
    }
    return 0;
  }
  
  if (vd >= 2 || hd >= 2) return 1;
  if ((targetP != 0) && ((targetP & 1) == pieceColor)) return 8;
  if (sqIsAttacked(d, opp_col, board)) return 16;
  
  return 0;
  
}

unsigned char isIllegalMove(struct Move move, struct Chessboard* board) {
  // 0 -> legal
  // 1 -> invalid_dest_for_piece_type
  // 2 -> blank_square_origin
  // 4 -> opponents_piece
  // 8 -> obstructed
  // 16 -> king_constraints
  // 32 -> no_castling_rights
  // 64 -> cant_en_passant
  // 128 -> corrupted square ( >= 14);
  // TODO ensure king vulnerability constraints
  unsigned char o = (move.info & 63);
  unsigned char d = (move.info >> 6) & 63;
  
  unsigned char p = (board->toMove) ? 1 : 0;

  if (o == d) return 1;
  if (board->squares[d] && ((board->squares[o] & 1) == (board->squares[d] & 1))) return 8;
  if (board->squares[o] >= 14) return 128;
  if (board->squares[o] == 0) return 2;
  if ((board->squares[o] & 1) != ((board->toMove) ? 1 : 0)) return 4;

  
  unsigned char illegal = 0;
  switch (board->squares[o] & 14) {
    case 2:
      illegal = isIllegalPawnMove(move, board); break;
    case 4:
      illegal = isIllegalKnightMove(move, board); break;
    case 6:
      illegal = isIllegalBishopMove(move, board); break;
    case 8:
      illegal = isIllegalRookMove(move, board); break;
    case 10:
      illegal = isIllegalRookMove(move,board) && isIllegalBishopMove(move,board);
      break;
    case 12:
      illegal = isIllegalKingMove(move,board); break;
    default:
      return 128;
  }
  
  if (illegal) return illegal;

  int in_check = 0;
  
  unsigned char king_square = 64;

  for (unsigned char i = 0; i < 64; i++) {
    if (board->squares[i] == 12+p) {
      in_check = (sqIsAttacked(i, (p+1)&1 ,board)) ? 1 : 0;
      king_square = i;
      break;
    }
  }
  if (in_check) {
    unsigned char katt = knightAttack(king_square,(p+1)&1, board);
    if (katt >> 8 >= 2) { // 2 attacking knights
      if (o != king_square) return 16;
      return isIllegalKingMove(move, board);
    }
    unsigned char attack_dirs = 0;
    int hd = 1;
    int vd = 0;
    
    int opp_col = (p+1)&1;
    
    for (unsigned char i = 0; i < 4; i++) {
      hd = (1 - (1 & i)) * (1 - (2 & i)); vd = (1 & i) * (1 - (2 & i));
      for (unsigned char k = king_square + hd + 8*vd; k >= 0 && k < 64; k += hd+8*vd) {
        if (board->squares[k] == 8+opp_col || board->squares[k] == 10+opp_col) {
          attack_dirs = attack_dirs | (1 << i);
          break;
        }
        if (board->squares[k] || absDiff(k%8, (k+hd)%8) > 1) break;
      }
    }
    char pawn_dir = p ? -1 : 1;
    for (unsigned char i = 4; i < 8; i++) {
      hd = 1-(2&i); vd = 1-(2&(i+1));
      if (absDiff(king_square % 8, (king_square+hd)%8) > 1) continue;
      if (vd == pawn_dir && board->squares[king_square+hd+8*vd] == 2+opp_col) {
        attack_dirs = attack_dirs | (1 << i);
        continue;
      }
      for (unsigned char k = king_square + hd + 8*vd; k >= 0 && k < 64; k += hd+8*vd) {
        if (board->squares[k] == 6+opp_col || board->squares[k] == 10+opp_col) {
          attack_dirs = attack_dirs | (1 << i);
          break;
        }
        if (board->squares[k] || absDiff(k%8, (k+hd)%8) > 1) break;
      }
    }
    
    int count = 0;
    for (int i = 0; i < 8; i++) {
      if (attack_dirs & (1 << i)) count++;
    }
    if (count + (katt ? 1 : 0) > 1) {
      if (o != king_square) return 16;
      return isIllegalKingMove(move, board);
    }
    if (o == king_square) return isIllegalKingMove(move, board);
    // otherwise check if its a block or capturing the checking piece.
    
    // if count == 0 and we are here there is 1 attacking knight and we are not moving the king. Check for capture.
    if (count == 0 && d != (katt & 255)) return 16; 
        
    // here count == 1, no attacking knights.
    int i = 0;
    while (!((1 << i) & attack_dirs)) i++;
    if (i < 4) {
      hd = (1 - (1 & i)) * (1 - (2 & i)); vd = (1 & i) * (1 - (2 & i));
    }
    else {
      hd = 1-(2&i); vd = 1-(2&(i+1));
    }
    int flag = 1;
    unsigned char k;
    for (k = king_square + hd + 8*vd; k >= 0 && k < 64; k += hd+8*vd) {
      if (d == k) {
        flag = 0;
        break;
      }
      
      // we check that if attacker is enpassantable pawn that we are capturing it en passant
      // this is the only case where we can get out of check with a capture with dest not
      // on piece LOS. So we must check for it in a special case.
      if ((board->squares[k] & 1) == opp_col && board->squares[k]){
        if ((board->squares[k] & 14) == 2 && board->enpassant == k%8) {
          if (d + 8*pawn_dir == k && board->squares[o] == p + 2) flag = 0;
        }
        
        break;
      }
    }
    if (flag) return 16;
  }
  
  return 0;
}

int putLegalMoves(struct Chessboard* board, struct Move* moves) {
  int k = 0;
  for (int i = 0; i < 64; i++) {
    if (board->squares[i] == 0 || (board->squares[i] & 1) != (board->toMove ? 1 : 0)) continue;
    for (int j = 0; j < 64; j++) {
      
      if (board->squares[j] && (board->squares[j]&1 == board->squares[i]&1)) continue;
      struct Move m;
      m.info = i + j*64;
      if (!isIllegalMove(m, board)) {
        if ((board->squares[i] & 14) == 2 && (j >= 56|| j < 8)) {
          for (int x = 0; x < 4; x++) {
            moves[k] = m;
            m.info+=64*64;
            k++;
          }
        }
        else {
          moves[k] = m;
          k++;
        }
      }
    }
    
  }
  return k;
}


int mateStatus(struct Chessboard* board) {
  for (int i = 0; i < 64; i++) {
    if (board->squares[i] == 0 || (board->squares[i] & 1) != (board->toMove ? 1 : 0)) continue;
    for (int j = 0; j < 64; j++) {
      
      if (board->squares[j] && (board->squares[j]&1 == board->squares[i]&1)) continue;
      struct Move m;
      m.info = i + j*64;
      if (!isIllegalMove(m, board)) return 0;
    }
  }
  
  for (int i = 0; i < 64; i++) {
    if (board->squares[i] == 12 + (board->toMove ? 1 : 0)) {
      return sqIsAttacked(i, board->toMove ? 0 : 1, board) ? 1 : 2; // 1 - checkmate, 2 - stalemate
    }
  }
}

// returns 0 on success, 1 if move was invalid.
int doMove(struct Move move, struct Chessboard* oldboard, struct Chessboard* newboard) {
  if (isIllegalMove(move, oldboard)) return 1;
  
  // copy old state over
  for (int i = 0; i < 64; i++) newboard->squares[i] = oldboard->squares[i];
  newboard->castling_rights = oldboard->castling_rights;
  newboard->enpassant = 8; // will set this if a pawn advances 2 squares later
  newboard->toMove = (oldboard->toMove) ? 0 : 1;
  
  unsigned char o = (move.info & 63);
  unsigned char d = (move.info >> 6) & 63;
  
  // if king move update castling rights and check for castles
  if ((newboard->squares[o] & 14) == 12) {
    newboard->castling_rights &= (15 ^ (3 << 2 * oldboard->toMove));
    // check for castles
    if (absDiff(o,d) == 2) {
      unsigned char rank = (o >> 3) << 3;
      unsigned char rook_loc = (o > d) ? 0 + rank : 7 + rank;
      char t = (o > d) ? 1 : -1;
      newboard->squares[d] = newboard->squares[o];
      newboard->squares[o] = 0;
      newboard->squares[d+t] = newboard->squares[rook_loc];
      newboard->squares[rook_loc] = 0;
      return 0;
    }
  }

  // check for enpassant - absdiff(o,d) & 1 checks for diag movement
  if ((newboard->squares[o] & 14) == 2 && (absDiff(o,d) & 1) && newboard->squares[d] == 0) {
    char rp = (newboard->toMove) ? -8 : 8;
    newboard->squares[d] = newboard->squares[o];
    newboard->squares[o] = 0;
    newboard->squares[d + rp] = 0;
    return 0; // en passant cant be done simultaneous to promotion so we are done here
  }
  // check for double advancement to update enpassant
  if ((newboard->squares[o] & 14) == 2 && (absDiff(o,d) == 16)) {
    newboard->enpassant = o % 8;
  } 
  // check for movement of rook to update castling rights
  if ((newboard->squares[o] & 14) == 8) {
    // identify rook file and rank for castling rights
    char bit;
    switch(o) {
      case 0:
        bit = 0; break;
      case 7:
        bit = 1; break;
      case 56:
        bit = 2; break;
      case 63:
        bit = 3; break;
      default:
        bit = 4;
    }
    newboard->castling_rights &= (15 ^ (1 << bit));
  }
  newboard->squares[d] = newboard->squares[o];
  newboard->squares[o] = 0;
  // if moving piece not a pawn we are done
  if ((newboard->squares[d] & 14) != 2) return 0;
  // check for promotion
  unsigned char drank = d / 8;
  unsigned char promorank = (oldboard->toMove) ? 0 : 7;
  if (promorank != drank) return 0;
  
  // promote the pawn
  newboard->squares[d] += 8 - 2 * (move.info >> 12); // 0 = Q, 1 = R, 2 = B, 3 = N
  return 0;
}

unsigned char backRankInitState[8] = { 8, 4, 6, 12, 10, 6, 4, 8 };


void resetChessboard(struct Chessboard* board) {
  // clear empty squares
  for (int i = 16; i < 48; i++) {
    board->squares[i] = 0;
  }
  
  // set pawns
  for (int i = 8; i < 16; i++) {
    board->squares[i] = 2;
  }
  for (int i = 48; i < 56; i++) {
    board->squares[i] = 3;
  }
  
  //set pieces on back ranks
  for (int i = 0; i < 8; i++) {
    board->squares[i]    = backRankInitState[i];
    board->squares[i+56] = backRankInitState[i] + 1; // add the black bit
  }
  board->toMove = 0;
  board->enpassant = 8;
  board->castling_rights = 15; // 15 = 00001111 => all castling rights available

}

unsigned char displayPiece(unsigned char c, unsigned char blank) {
  unsigned char p;
  switch (c & 254) {
    case 2:
      p = 80;
      break;
    case 4:
      p = 78;
      break;
    case 6:
      p = 66;
      break;
    case 8:
      p = 82;
      break;
    case 10:
      p = 81;
      break;
    case 12:
      p = 75;
      break;
    default:
      return blank;
  }
  p += (c & 1) * 32;
  return p;
}

unsigned short hashcode(struct Chessboard* board) {
  int z = 0;
  for (int i = 0; i < 16; i++) {
    int p = 0;
    for (int j = 0; j < 4; j++) {
      p ^= board->squares[i*4+j] ? 1 : 0;
    }
    z ^= (p << i);
  }
  return z;
}

int equal(struct Chessboard* b1, struct Chessboard* b2) {
  if ((b2->toMove && !b1->toMove) || (b1->toMove && !b2->toMove)) return 1;
  if (b1->enpassant < 8 && b1->enpassant>=0 && b1->enpassant != b2->enpassant) return 1;
  if ((b1->castling_rights ^ b2->castling_rights) & 15 != 0) return 1;
  for (int i = 0; i < 64; i++) {
    if (b1->squares[i] != b2->squares[i]) return 1;
  }
  return 0;
}

void printBoard(struct Chessboard* board) {
  if (board->toMove) printf("Black to move\n");
  else printf ("White to move\n");
  printf("\n+---+---+---+---+---+---+---+---+\n");
  unsigned char sqcol = '/';
  unsigned char othercol = ' ';
  for (int i = 56; i >= 0; i-=8) {
    for (int j = 7; j >= 0; j--) {
      printf("|%c%c%c", sqcol,displayPiece(board->squares[i+j],sqcol),sqcol);
      unsigned char t = sqcol;
      sqcol = othercol;
      othercol = t;
    }
    unsigned char t = sqcol;
    sqcol = othercol;
    othercol = t;
    printf("|\n+---+---+---+---+---+---+---+---+\n");
  }
  printf("Castling rights:\n");
  if (board->castling_rights & 1) printf("White Kingside\n");
  if (board->castling_rights & 2) printf("White Queenside\n");
  if (board->castling_rights & 4) printf("Black Kingside\n");
  if (board->castling_rights & 8) printf("Black Queenside\n");
  if (board->enpassant < 8) printf("En passant on file %d\n", board->enpassant);
  if (mateStatus(board) == 1) printf("Checkmate!\n");
  if (mateStatus(board) == 2) printf("Stalemate!\n");
  printf("Hashcode: %d\n",hashcode(board));
  printf("\n");
  
}

void writeCoord(unsigned char coord, char* str) {
  char rank = coord / 8 + 49;
  char file = 104 - coord % 8;
  str[0] = file;
  str[1] = rank;
}

