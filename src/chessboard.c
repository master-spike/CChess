#include <stdio.h>
#include "chessboard.h"

unsigned int absDiff(int i, int j) {
  return (i - j < 0) ? (j - i) : (i - j);
}

const int knight_ys[8] = {-1, 1, -2, 2, -2, 2, -1, 1};
const int knight_xs[8] = {-2, -2, -1, -1, 1, 1, 2, 2};

const int orth_xs[4] = {1, -1, 0, 0};
const int orth_ys[4] = {0, 0, 1, -1};

const int diag_xs[4] = {1, -1, 1, -1};
const int diag_ys[4] = {1, 1, -1, -1};

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

unsigned int checkTest(struct Chessboard* board) {
  
  // return info:
  // [2 bits pawn][8 bits queen][4 bits bishop][4 bits rooks][8 bits knight]
  // knight bits [2L1U,2L1D,1L2U,1L2D,..,2R1U,2R1D]
  // rook bits [L,R,U,D]
  // bishop bits [LU,LD,RU,RD]
  // queen bits: [diagonals like bishop | orthogonals like rook]
  // pawn bits: [L,R]
  
  unsigned char p = (board->toMove) ? 1 : 0;
  unsigned char op = 1 - p;
  unsigned char ks = 64;
  for (int i = 0; i < 64; i++) {
    if (p + 12 == board->squares[i]) {
      ks = i;
      break;
    }
  }
  
  unsigned int attacks = 0;
  
  int kr = ks / 8;
  int kf = ks % 8;
  
  // check knight attacks
  for (int i = 0; i < 8; i++) {
    int x = knight_xs[i];
    int y = knight_ys[i];
    if (kr + y < 0 || kr + y >= 8 || kf + x < 0 || kf + x >= 8) continue;
    unsigned char bs = (kr + y) * 8 + kf + x;
    if (board->squares[bs] == 4 + op) attacks |= (1 << i);
  }
  
  // check orth attacks
  for (int i = 0; i < 4; i++) {
    int x = orth_xs[i];
    int y = orth_ys[i];
    int d = 1;
    while (kr + y*d < 0 && kr + y*d >= 8 && kf + x*d < 0 && kf + x*d >= 8) {
      unsigned char bs = (kr + y*d) * 8 + kf + x*d;
      if (!board->squares[bs]) {
        d++; continue;
      }
      if (board->squares[bs] == 8 + op) attacks |= (1 << (i+8));
      if (board->squares[bs] == 10 + op) attacks |= (1 << (i+16));
      break;
    }
  }
  // check diagonal attacks
  for (int i = 0; i < 4; i++) {
    int x = diag_xs[i];
    int y = diag_ys[i];
    int d = 1;
    while (kr + y*d < 0 && kr + y*d >= 8 && kf + x*d < 0 && kf + x*d >= 8) {
      unsigned char bs = (kr + y*d) * 8 + kf + x*d;
      if (!board->squares[bs]) {
        d++; continue;
      }
      if (board->squares[bs] == 6 + op) attacks |= (1 << (i+12));
      if (board->squares[bs] == 10 + op) attacks |= (1 << (i+20));
      break;
    }
  }
  
  
  int y = (board->toMove) ? -1 : 1;
  // check pawn attacks
  int x2 = 1;
  int x1 = -1;
  if (y + kr >= 0 && y + kr < 8) {
    if (x1 + kf >= 0 && x1 + kf < 8) {
      unsigned char bs = (kr + y) * 8 + kf + x1;
      if (board->squares[bs] == 2 + op) attacks |= (1 << 24);
    }
    if (x2 + kf >= 0 && x2 + kf < 8) {
      unsigned char bs = (kr + y) * 8 + kf + x2;
      if (board->squares[bs] == 2 + op) attacks |= (1 << 25);
    }
  }
  return attacks;
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
    int dh = diag_xs[d];
    int dv = diag_ys[d];
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
    x = knight_xs[d];
    y = knight_ys[d];
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
  

  
  struct Chessboard testboard;
  doMove(move, board, &testboard);

  for (unsigned char i = 0; i < 64; i++) {
    if (testboard.squares[i] == 12+p) {
      if (sqIsAttacked(i, (p+1)&1, &testboard)) return 16;
    }
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
      if (!isIllegalMove(m, board)) return (board->fifty_move_rule >= 50) ? 3 : 0;
    }
  }
  for (int i = 0; i < 64; i++) {
    if (board->squares[i] == 12 + (board->toMove ? 1 : 0)) {
      return sqIsAttacked(i, (board->toMove ? 0 : 1), board) ? 1 : 2; // 1 - checkmate, 2 - stalemate
    }
  }
}


int doMove(struct Move move, struct Chessboard* oldboard, struct Chessboard* newboard) {
  //if (isIllegalMove(move, oldboard)) return 1;
  
  // copy old state over
  for (int i = 0; i < 64; i++) newboard->squares[i] = oldboard->squares[i];
  newboard->castling_rights = oldboard->castling_rights;
  newboard->enpassant = 8; // will set this if a pawn advances 2 squares later
  newboard->toMove = (oldboard->toMove) ? 0 : 1;
  
  unsigned char o = (move.info & 63);
  unsigned char d = (move.info >> 6) & 63;
  
  newboard->fifty_move_rule = (oldboard->squares[o] == 3 - newboard->toMove ||
                               oldboard->squares[d]) ? 0 : oldboard->fifty_move_rule + 1;
  
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
  board->fifty_move_rule = 0;
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
  if (board->castling_rights & 1) printf(" White Kingside |");
  if (board->castling_rights & 2) printf(" White Queenside |");
  if (board->castling_rights & 4) printf(" Black Kingside |");
  if (board->castling_rights & 8) printf(" Black Queenside |");
  printf("\n");
  if (board->enpassant < 8) printf("En passant on file %d\n", board->enpassant);
  if (mateStatus(board) == 1) printf("Checkmate!\n");
  if (mateStatus(board) == 2) printf("Stalemate!\n");
  if (mateStatus(board) == 3) printf("Fifty move rule!\n");
  printf("Hashcode: %d, checkTest: 0x%08x\n",hashcode(board), checkTest(board));
  printf("\n");
  
}

void writeCoord(unsigned char coord, char* str) {
  char rank = coord / 8 + 49;
  char file = 104 - coord % 8;
  str[0] = file;
  str[1] = rank;
}

