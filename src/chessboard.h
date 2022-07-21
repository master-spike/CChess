#ifndef CHESSBOARD_H_GUARD
#define CHESSBOARD_H_GUARD

struct Chessboard {
  unsigned char squares[64]; // 0 = white kingside corner
                    // 7 = white queenside corner
                    // 56 = black kingside corner
                    // 63 = black queenside corner
  unsigned char castling_rights; // xxxxABCD -> A,B queenside black/white resp
                        //          -> C,D kingside black/white resp
  unsigned int toMove; // true -> black, false -> white
  unsigned char enpassant; // 0-7 represents which file has en-passant, any larger number means not allowed
  
  // piece on square = player bit + piece id | 0 (empty)
  // player bit = 0 | 1 [white | black]
  // piece ids: 2 = pawn, 4 = knight, 6 = bishop, 8 = rook, 10 = queen, 12 = king

};

struct Move {
  unsigned short info;
  // info & 63           => origin
  // (info & 4032) >> 6  => dest
  // (info & 12288) > 12 => promo_type (0 -> q, 1 -> r, 2 -> b, 3 -> n)
};


unsigned char isIllegalMove(struct Move move, struct Chessboard *board);
int putLegalMoves(struct Chessboard* board, struct Move* move);
int doMove(struct Move move, struct Chessboard* oldboard, struct Chessboard* newboard);
void printBoard(struct Chessboard* board);
void writeCoord(unsigned char coord, char* str);
void resetChessboard(struct Chessboard* board);
unsigned char displayPiece(unsigned char c, unsigned char blank);


#endif
