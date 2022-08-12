#include <stdint.h>
#ifndef PATTERNS_H_GUARD
#define PATTERNS_H_GUARD

extern const double piece_square_pawn[64];

extern const double piece_square_knight[64];

extern const double piece_square_bishop[64];

extern const double piece_square_rook[64];

extern const double piece_square_queen[64];

extern const uint64_t white_pawn_move_table[64];

extern const uint64_t white_pawn_attack_table[64];

extern const uint64_t black_pawn_move_table[64];

extern const uint64_t black_pawn_attack_table[64];

extern const uint64_t bishop_table[64];

extern const uint64_t knight_table[64];

extern const uint64_t rook_table[64];

extern const uint64_t king_att_table[64];

// this table allows us to check for blocks using a bit mask.
extern const uint64_t block_masks[4096];

#endif