#ifndef BITBOARD_H_GUARD
#define BITBOARD_H_GUARD

#include <stdint.h>

struct BitBoard
{
  uint64_t pieces[12]; // PIECECODES: pawn 0, knight 2, bishop 4, rook 6, queen 8, king 10 | COLOR: white 0, black 1
  uint8_t c_rights;    // castling rights
  uint8_t enpassant;   // if a pawn advanced 2 squares on the previous move, this value is the file of that pawn. otherwise 8
  uint64_t ply_count;  // ply_count & 1 == 0 if white to move, 1 if black to move.
  uint16_t last_move;
};


struct BitBoard doMove(struct BitBoard *b, int sp, int ep, int sqi, int sqj);
struct BitBoard doCastles(struct BitBoard *b, int kingside);
struct BitBoard doEnpassant(struct BitBoard *b, int pi, int pj, int ep);
int findKing(uint64_t king_bits);
int getPieceAt(struct BitBoard *b, int pos, int player);
uint64_t checkBlockCapMask(struct BitBoard *board);
uint64_t squareAttackedBy(struct BitBoard *board, int sq, int ignore_king);
uint64_t squareInLosOf(struct BitBoard *board, int sq);
uint64_t kingMoveMask(struct BitBoard *board);
uint64_t attackMaskOfPiece(int piece, int pos);
int genMoves(struct BitBoard *board, struct BitBoard *new_boards, int cap_only);
struct BitBoard startBoard();
void printMask(uint64_t mask);
void printBitBoard(struct BitBoard b);
uint64_t perft(struct BitBoard* b, int depth);

#endif