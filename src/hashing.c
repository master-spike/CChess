#include "hashing.h"
#include "bitboard.h"



struct xorshift64_state {
  uint64_t a;
};

uint64_t xorshift64(struct xorshift64_state *state)
{
  uint64_t x = state->a;
  x ^= x << 13;
  x ^= x >> 7;
  x ^= x << 17;
  return state->a = x;
}

void initialiseSeeds() {
  struct xorshift64_state state;
  state.a = 0x3032ee4579d8eab6ULL; 
  for (int i = 0; i < 768; i++) {
    piece_seeds[i] = xorshift64(&state) >> 32;
  }
  for (int i = 0; i < 16; i++) {
    cr_seeds[i] = xorshift64(&state) >> 32;
  }
  for (int i = 0; i < 9; i++) {
    ep_seeds[i] = xorshift64(&state) >> 32;
  }
  player_seed = xorshift64(&state) >> 32;
}

uint32_t hashcode(struct BitBoard* b) {
  uint32_t hc = 0;
  for (int i = 0; i < 12; i++) {
    uint64_t num = b->pieces[i];
    while(num) {
      uint64_t num_next = num & (num - 1);
      hc ^= piece_seeds[64*i + findKing(num ^ num_next)];
      num = num_next;
    }
  }
  hc ^= cr_seeds[b->c_rights & 15];
  hc ^= (b->ply_count&1) * player_seed;
  hc ^= ep_seeds[(b->enpassant >= 8) ? 8 : b->enpassant];
  b->hashcode = hc;
  return hc;
}