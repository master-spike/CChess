#include "transpostable.h"

struct TTTable makeTTTable(int logsize, int eph) {
  struct TTTable table;
  table.logsize = logsize;
  table.entries_per_hash = eph;
  size_t size = (size_t) (1 << logsize) * (size_t) eph;
  table.table = (struct TTTableEntry*) malloc(sizeof(struct TTTableEntry) * size);
  for (int i = 0; i < size; i++) {
    table.table[i].valid = 0;
  }
  return table;
}




struct TTTLookupReturn tttableLookup(struct TTTable* tttable, struct BitBoard* board, int depth) {
  uint32_t hc = board->hashcode;
  uint64_t index = ((uint64_t) hc & ((1ULL << tttable->logsize)-1)) * (uint64_t) tttable->entries_per_hash;
  struct TTTableEntry* ptr = tttable->table + index;
  struct TTTLookupReturn ret;
  ret.valid = 0;

  for (int i = 0; i < tttable->entries_per_hash; i++) {
    if (ptr[i].depth != depth || !ptr[i].valid) continue;
    if (!equivalent(&(ptr[i].board), board)) continue;
    
    ret.node_type = ptr[i].node_type;
    ret.val = ptr[i].evaluation;
    ret.valid = ptr[i].valid;
    break;
  }
  return ret;
}

void tttableInsert(struct TTTable* tttable, struct TTTableEntry te) {
  uint32_t hc = te.board.hashcode;
  uint64_t index = ((uint64_t) hc & ((1ULL << tttable->logsize)-1)) * (uint64_t) tttable->entries_per_hash;
  struct TTTableEntry* ptr = tttable->table + index;
  for (int i = 0; i < tttable->entries_per_hash; i++) {
    if (!ptr[i].valid) {
      ptr[i] = te;
      return;
    }
  }
  // replace
  int i = (hc >> tttable->logsize) % tttable->entries_per_hash;
  ptr[i] = te;
}

void clearTTable(struct TTTable* tttable) {

  size_t size = (size_t) (1 << tttable->logsize) * (size_t) tttable->entries_per_hash;
  for (int i = 0; i < size; i++) {
    tttable->table[i].valid = 0;
  }
}