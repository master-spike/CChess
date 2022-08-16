#ifndef HASHING_H_GUARD
#define HASHING_H_GUARD

#include <stdint.h>
#include "types.h"

uint32_t piece_seeds[768];
uint32_t cr_seeds[16];
uint32_t ep_seeds[9];
uint32_t player_seed;

void initialiseSeeds();
uint32_t hashcode(struct BitBoard* b);

#endif