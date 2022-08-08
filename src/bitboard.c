#include <stdint.h>
#include <stdio.h>


struct BitBoard {
  uint64_t pieces[12]; // PIECECODES: pawn 0, knight 2, bishop 4, rook 6, queen 8, king 10 | COLOR: white 0, black 1
  uint8_t c_rights; // castling rights
  uint8_t enpassant; // if a pawn advanced 2 squares on the previous move, this value is the file of that pawn. otherwise 8
  uint64_t ply_count; // ply_count & 1 == 0 if white to move, 1 if black to move.
};

const uint64_t default_pieces[12] = { 65280ULL, 71776119061217280ULL,
                                      66ULL, 4755801206503243776ULL,
                                      36ULL, 2594073385365405696ULL,
                                      129ULL, 9295429630892703744ULL,
                                      16ULL, 1152921504606846976ULL,
                                      8ULL, 576460752303423488ULL
                                    };

const char piece_chars[12] = { 'P', 'p', 'N', 'n', 'B', 'b', 'R', 'r', 'Q', 'q', 'K', 'k' };


struct BitBoard doMove(struct BitBoard* b, uint16_t move) {
  
  int o = move&63;
  int d = (move>>6)&63;
  int promo_piece = 2 * (move>>12) + (b->ply_count&1) + 2;
  
  int opp_col = (b->ply_count&1) ? 0 : 1;
  
  struct BitBoard newboard = *b;
  int moving_piece = 12;
  for (int i = b->ply_count&1; i < 12; i+=2) {
    if (b->pieces[i]&o) {
      moving_piece = i; break;
    }
  }
  
  
  
}

int findKing(uint64_t king_bits) {
  
  int p = 0;
  int k = 32;
  while (k > 0) {
    uint64_t mask = ((1 << k) - 1) << k;
    if (king_bits & mask) {
      king_bits >> k;
      p += k;
    }
    k = k/2;
  } 
  
  return p;
}

struct BitBoard startBoard() {
  
  struct BitBoard b;
  for (int i = 0; i < 12; i++) {
    b.pieces[i] = default_pieces[i];

  }
  
  b.c_rights = 15;
  b.enpassant = 8;
  b.ply_count = 0;
  
  return b;
  
}

void printBitBoard(struct BitBoard b) {
  
  char str_w[] = "White";
  char str_b[] = "Black";

  char* col_str = (b.ply_count & 1) ? str_b : str_w;
  
  printf("%s to move:\n", col_str);

  char p = ' ';
  char q = '/';
  char t;
  
  for (int i = 56; i >= 0; i-=8) {
    printf("+---+---+---+---+---+---+---+---+\n");
    for (int j = i + 7; j >= i; j--) {
      char c = p;
      for (int k = 0; k < 12; k++) {
        if (b.pieces[k] & (1ULL << j)) {
          c = piece_chars[k]; break;
        }
      }
      printf("|%c%c%c",p,c,p);
      t = p;
      p = q;
      q = t;
    }
    printf("|\n");
    t = p;
    p = q;
    q = t;
  }
  printf("+---+---+---+---+---+---+---+---+\n");
}


int main() {
  
  struct BitBoard b = startBoard();
  
  printBitBoard(b);
  
}
