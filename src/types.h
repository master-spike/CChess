#ifndef TYPES_H_GUARD
#define TYPES_H_GUARD

#include <stdint.h>

struct MinimaxReturn {
  double val;
  uint16_t move;
  int d;
};

struct BitBoard
{
  uint64_t pieces[12]; // PIECECODES: pawn 0, knight 2, bishop 4, rook 6, queen 8, king 10 | COLOR: white 0, black 1
  uint8_t c_rights;    // castling rights
  uint8_t enpassant;   // if a pawn advanced 2 squares on the previous move, this value is the file of that pawn. otherwise 8
  uint64_t ply_count;  // ply_count & 1 == 0 if white to move, 1 if black to move.
  uint16_t last_move;
  uint8_t last_move_md;
  uint32_t hashcode;
};


struct PerftData {
    uint64_t nodes;
    uint64_t captures;
    uint64_t castles;
    uint64_t enpassants;
    uint64_t checks;
};

struct TTTableEntry {
  struct BitBoard board;
  double evaluation;
  uint16_t depth;
  uint8_t node_type;
  uint8_t valid;
};

struct TTTable {
  int logsize;
  int entries_per_hash;
  struct TTTableEntry* table; 
};

struct TTTLookupReturn {
    double val;
    uint8_t valid;
    uint8_t node_type; // 0: no cutoff. 1: alpha cutoff. 2: beta cutoff. 3: all node (alpha) 4: all node (beta)
};

#endif