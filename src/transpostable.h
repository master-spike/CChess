#ifndef TRANSPOSTABLE_H_GUARD
#define TRANSPOSTABLE_H_GUARD

#include <stdlib.h>
#include "bitboard.h"
#include <stdint.h>
#include "types.h"

struct TTTLookupReturn tttableLookup(struct TTTable* tttable, struct BitBoard* board, int depth);
void tttableInsert(struct TTTable* tttable, struct TTTableEntry te);
struct TTTable makeTTTable(int logsize, int eph);

#endif