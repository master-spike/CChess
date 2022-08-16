#ifndef BITBOARD_H_GUARD
#define BITBOARD_H_GUARD

#include <stdint.h>
#include <stdio.h>
#include "patterns.h"
#include "hashing.h"
#include "types.h"

int countBits(uint64_t n);
struct BitBoard bbNullMove(struct BitBoard *b);
struct BitBoard bbDoMove(struct BitBoard *b, int sp, int ep, int sqi, int sqj);
struct BitBoard bbDoCastles(struct BitBoard *b, int kingside);
struct BitBoard bbDoEnpassant(struct BitBoard *b, int pi, int pj, int ep);
int findKing(uint64_t king_bits);
int getPieceAt(struct BitBoard *b, int pos, int player);
uint64_t checkBlockCapMask(struct BitBoard *board);
uint64_t squareAttackedBy(struct BitBoard *board, int sq, int ignore_king, int pla);
uint64_t squareInLosOf(struct BitBoard *board, int sq);
uint64_t kingMoveMask(struct BitBoard *board);
uint64_t attackMaskOfPiece(int piece, int pos);
int genMoves(struct BitBoard *board, struct BitBoard *new_boards, int cap_only, int just_one);
struct BitBoard startBoard();
void printMask(uint64_t mask);
void printBitBoard(struct BitBoard b);
int equivalent(struct BitBoard* b1, struct BitBoard* b2);
uint64_t perft(struct BitBoard b, int depth);
struct BitBoard doMove(struct BitBoard* board, uint16_t move);
struct BitBoard kiwipete();

#endif
