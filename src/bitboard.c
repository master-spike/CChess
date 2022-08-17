#include "bitboard.h"

const uint64_t default_pieces[12] = {65280ULL, 71776119061217280ULL,
                                     66ULL, 4755801206503243776ULL,
                                     36ULL, 2594073385365405696ULL,
                                     129ULL, 9295429630892703744ULL,
                                     16ULL, 1152921504606846976ULL,
                                     8ULL, 576460752303423488ULL};

const char piece_chars[12] = {'P', 'p', 'N', 'n', 'B', 'b', 'R', 'r', 'Q', 'q', 'K', 'k'};

int countBits(uint64_t n)
{
  int c = 0;
  while (n)
  {
    c++;
    n = n & (n - 1);
  }
  return c;
}

uint64_t allPieces(struct BitBoard *b)
{
  uint64_t v = 0;
  for (int i = 0; i < 12; i++)
  {
    v |= b->pieces[i];
  }
  return v;
}

struct BitBoard bbNullMove(struct BitBoard *b) {
  struct BitBoard newboard = *b;
  newboard.ply_count++;
  newboard.enpassant = 8;
  newboard.last_move = 0;
  newboard.last_move_md = 16;
  newboard.hashcode ^= player_seed;
  newboard.hashcode ^= ep_seeds[b->enpassant];
  newboard.hashcode ^= ep_seeds[8];
  return newboard;
}

struct BitBoard bbDoMove(struct BitBoard *b, int sp, int ep, int sqi, int sqj)
{

  struct BitBoard newboard = *b;

  newboard.pieces[sp] &= ~(1ULL << sqi);
  newboard.pieces[ep] |= (1ULL << sqj);
  newboard.hashcode ^= piece_seeds[sp*64 + sqi];
  newboard.hashcode ^= piece_seeds[ep*64 + sqj];
  for (int i = 0; i < ep; i++)
  {
    newboard.pieces[i] &= ~(1ULL << sqj);
    if ((1ULL<<sqj) & b->pieces[i]) newboard.hashcode ^= piece_seeds[i*64 + sqj];
  }
  for (int i = ep + 1; i < 12; i++)
  {
    newboard.pieces[i] &= ~(1ULL << sqj);
    if ((1ULL<<sqj) & b->pieces[i]) newboard.hashcode ^= piece_seeds[i*64 + sqj];
  }
  newboard.ply_count++;
  newboard.enpassant = 8;
  newboard.last_move = sqi + 64 * sqj;
  if (sp == 0 && sqj - sqi == 16)
    newboard.enpassant = sqi % 8;
  if (sp / 2 == 0 && (sqj >= 56 || sqi <= 7))
    newboard.last_move += 4096 * ((ep / 2) - 1);
  if (sp == 1 && sqi - sqj == 16)
    newboard.enpassant = sqi % 8;
  if (sp == 10)
    newboard.c_rights &= (uint8_t)~3U;
  if (sp == 11)
    newboard.c_rights &= (uint8_t)~12U;
  if (sqi == 0 || sqj == 0)
    newboard.c_rights &= (uint8_t)~1U;
  if (sqi == 7 || sqj == 7)
    newboard.c_rights &= (uint8_t)~2U;
  if (sqi == 56 || sqj == 56)
    newboard.c_rights &= (uint8_t)~4U;
  if (sqi == 63 || sqj == 63)
    newboard.c_rights &= (uint8_t)~8U;

  newboard.hashcode ^= ep_seeds[b->enpassant];
  newboard.hashcode ^= ep_seeds[newboard.enpassant];
  newboard.hashcode ^= cr_seeds[b->c_rights & 15];
  newboard.hashcode ^= cr_seeds[newboard.c_rights & 15];
  newboard.hashcode ^= player_seed;

  newboard.last_move_md = 0;
  newboard.last_move_md += ((allPieces(b) & (1ULL << sqj)) ? 1 : 0);

  return newboard;
}

struct BitBoard bbDoCastles(struct BitBoard *b, int kingside)
{

  uint64_t ki = (b->ply_count & 1) ? (1ULL << 59) : (1ULL << 3);
  uint64_t ri = (kingside) ? ki >> 3 : ki << 4;
  uint64_t kj = (kingside) ? ki >> 2 : ki << 2;
  uint64_t rj = (kingside) ? ri << 2 : ri >> 3;

  int k_p = 10 + (b->ply_count & 1);
  int r_p = 6 + (b->ply_count & 1);

  struct BitBoard newboard = *b;
  newboard.c_rights &= ((b->ply_count & 1) ? 3U : 12U);
  newboard.enpassant = 8;
  newboard.ply_count++;
  newboard.pieces[k_p] ^= (ki | kj);
  newboard.pieces[r_p] ^= (ri | rj);
  newboard.last_move = findKing(ki) + 64 * findKing(kj);
  newboard.last_move_md = 2;

  newboard.hashcode ^= piece_seeds[k_p*64 + findKing(ki)];
  newboard.hashcode ^= piece_seeds[k_p*64 + findKing(kj)];
  newboard.hashcode ^= piece_seeds[r_p*64 + findKing(ri)];
  newboard.hashcode ^= piece_seeds[r_p*64 + findKing(rj)];
  newboard.hashcode ^= ep_seeds[b->enpassant];
  newboard.hashcode ^= ep_seeds[8];
  newboard.hashcode ^= cr_seeds[b->c_rights & 15];
  newboard.hashcode ^= cr_seeds[newboard.c_rights & 15];
  newboard.hashcode ^= player_seed;

  return newboard;
}

struct BitBoard bbDoEnpassant(struct BitBoard *b, int pi, int pj, int ep)
{
  struct BitBoard newboard = *b;
  int player = b->ply_count & 1;
  int opp = (player) ? 0 : 1;
  newboard.pieces[player] ^= ((1ULL << pi) | (1ULL << pj));
  newboard.pieces[opp] ^= (1ULL << ep);
  newboard.enpassant = 8;
  newboard.ply_count++;
  newboard.last_move = pi + 64 * pj + 16384;
  newboard.last_move_md = 1;
  
  newboard.hashcode ^= piece_seeds[player*64 + pi];
  newboard.hashcode ^= piece_seeds[player*64 + pj];
  newboard.hashcode ^= piece_seeds[opp*64 + ep];
  newboard.hashcode ^= ep_seeds[b->enpassant];
  newboard.hashcode ^= ep_seeds[8];
  newboard.hashcode ^= player_seed;

  return newboard;
}

int findKing(uint64_t king_bits)
{

  int p = 0;
  int k = 32;
  while (k > 0)
  {
    uint64_t mask = ((1ULL << k) - 1) << k;
    if (king_bits & mask)
    {
      king_bits = king_bits >> k;
      p += k;
    }
    k = k / 2;
  }

  return p;
}

int getPieceAt(struct BitBoard *b, int pos, int player)
{
  if (pos >= 64 || pos < 0)
    return 12;

  uint64_t mask = 1ULL << pos;

  for (int i = player; i < 12; i += 2)
  {
    if (b->pieces[i] & mask)
      return i;
  }

  return 12;
}

// this mask places restriction on movement of non-king pieces
uint64_t checkBlockCapMask(struct BitBoard *board)
{
  uint64_t mask = ~0ULL;
  int pla = board->ply_count & 1;
  int opp = (pla) ? 0 : 1;
  int our_king = findKing(board->pieces[10 + pla]);
  uint64_t diag_attackers = board->pieces[4 + opp] | board->pieces[8 + opp];
  uint64_t orth_attackers = board->pieces[6 + opp] | board->pieces[8 + opp];
  uint64_t knig_attackers = board->pieces[2 + opp];
  uint64_t pawn_attackers = board->pieces[0 + opp];

  uint64_t all_pieces = 0;
  for (int i = 0; i < 12; i++)
  {
    all_pieces |= board->pieces[i];
  }

  // filter the attackers by pieces which are in LOS of king
  diag_attackers &= bishop_table[our_king];
  orth_attackers &= rook_table[our_king];
  knig_attackers &= knight_table[our_king];
  pawn_attackers &= ((opp) ? white_pawn_attack_table[our_king] : black_pawn_attack_table[our_king]);

  if (knig_attackers)
    mask &= knig_attackers; // these checks can only be captured
  if (pawn_attackers)
    mask &= pawn_attackers; // not blocked.

  for (int i = 0; i < 64; i++)
  {
    if ((1ULL << i) & (orth_attackers | diag_attackers))
    { // if i references a rook, bishop or queen
      if (!(all_pieces & block_masks[our_king * 64 + i]))
      { // if path from i to king is not blocked
        mask &= (1ULL << i) | block_masks[our_king * 64 + i];
      }
    }
  }
  return mask;
}

uint64_t squareAttackedBy(struct BitBoard *board, int sq, int ignore_king, int pla)
{
  uint64_t mask = 0ULL;
  int opp = (pla) ? 0 : 1;
  uint64_t diag_attackers = board->pieces[4 + opp] | board->pieces[8 + opp];
  uint64_t orth_attackers = board->pieces[6 + opp] | board->pieces[8 + opp];
  uint64_t knig_attackers = board->pieces[2 + opp];
  uint64_t pawn_attackers = board->pieces[0 + opp];
  uint64_t king_attackers = board->pieces[10 + opp];
  uint64_t all_pieces = 0;
  for (int i = 0; i < 12; i++)
  {
    all_pieces |= board->pieces[i];
  }
  if (ignore_king)
    all_pieces &= ~board->pieces[10 + pla];

  // filter the attackers by pieces which are in LOS direction of sq
  diag_attackers &= bishop_table[sq];
  orth_attackers &= rook_table[sq];
  knig_attackers &= knight_table[sq];
  pawn_attackers &= ((opp) ? white_pawn_attack_table[sq] : black_pawn_attack_table[sq]);
  king_attackers &= king_att_table[sq];

  mask |= knig_attackers;
  mask |= pawn_attackers;
  mask |= king_attackers;

  uint64_t toa = orth_attackers;
  uint64_t tda = diag_attackers;
  while (toa)
  {
    uint64_t t = toa & (toa - 1); // t is next toa, t ^ toa is the potential candidate for attacker.
    int i = findKing(t ^ toa);
    if (!((all_pieces ^ orth_attackers) & block_masks[i * 64 + sq]))
      mask |= (t ^ toa);
    toa = t;
  }
  while (tda)
  {
    uint64_t t = tda & (tda - 1); // t is next toa, t ^ toa is the potential candidate for attacker.
    int i = findKing(t ^ tda);
    if (!((all_pieces ^ diag_attackers) & block_masks[i * 64 + sq]))
      mask |= (t ^ tda);
    tda = t;
  }

  return mask;
}

uint64_t squareInLosOf(struct BitBoard *board, int sq)
{
  uint64_t mask = 0ULL;
  int pla = board->ply_count & 1;
  int opp = (pla) ? 0 : 1;
  uint64_t diag_attackers = board->pieces[4 + opp] | board->pieces[8 + opp];
  uint64_t orth_attackers = board->pieces[6 + opp] | board->pieces[8 + opp];

  mask |= diag_attackers;
  mask |= orth_attackers;
  return mask;
}

uint64_t kingMoveMask(struct BitBoard *board)
{
  int pla = board->ply_count & 1;
  int opp = (pla) ? 0 : 1;
  int our_king = findKing(board->pieces[10 + pla]);
  uint64_t mask = king_att_table[our_king];
  uint64_t temp_mask = mask;

  while (temp_mask)
  {
    int i = findKing(temp_mask);
    if (squareAttackedBy(board, i, 1, pla))
      mask ^= (1ULL << i);
    temp_mask ^= (1ULL << i);
  }
  return mask;
}

uint64_t attackMaskOfPiece(int piece, int pos)
{
  if (piece == 0)
    return white_pawn_attack_table[pos];
  if (piece == 1)
    return black_pawn_attack_table[pos];
  if ((piece | 1) == 3)
    return knight_table[pos];
  if ((piece | 1) == 5)
    return bishop_table[pos];
  if ((piece | 1) == 7)
    return rook_table[pos];
  if ((piece | 1) == 9)
    return bishop_table[pos] | rook_table[pos];
  return king_att_table[pos];
}

uint64_t getPinmask(struct BitBoard *b, int i, int k, uint64_t all_pieces)
{
  // assert i != king position
  if (i == k)
    return ~0ULL;

  uint64_t i_bit = 1ULL << i;
  uint64_t align_test_diagonal = i_bit & (diagonals[k]);
  uint64_t align_test_antidiag = i_bit & (antidiagonals[k]);
  uint64_t align_test_file = i_bit & (files[k]);
  uint64_t align_test_rank = i_bit & (ranks[k]);

  int pla = b->ply_count & 1;
  int opp = pla ^ 1;

  uint64_t test_pieces = 0;
  if (!(align_test_diagonal | align_test_antidiag | align_test_file | align_test_rank))
    return ~0ULL;

  if (align_test_diagonal)
    test_pieces = diagonals[k] & (b->pieces[opp + 4] | b->pieces[opp + 8]);
  if (align_test_antidiag)
    test_pieces = antidiagonals[k] & (b->pieces[opp + 4] | b->pieces[opp + 8]);
  if (align_test_file)
    test_pieces = files[k] & (b->pieces[opp + 6] | b->pieces[opp + 8]);
  if (align_test_rank)
    test_pieces = ranks[k] & (b->pieces[opp + 6] | b->pieces[opp + 8]);
  
  //if (test_pieces == 0) printf("What the fuck \n");
  while (test_pieces)
  {
    uint64_t tp = test_pieces & (test_pieces - 1);
    int tp_n = findKing(tp ^ test_pieces);
    if ((block_masks[64 * k + tp_n] & all_pieces) == i_bit)
      return block_masks[64 * k + tp_n] | (tp ^ test_pieces);
    test_pieces = tp;
  }
  
  return ~0ULL;
}

int genPawnCaptures(struct BitBoard* board, struct BitBoard* new_boards, int pla, int king, uint64_t check_bc_mask, uint64_t opp_pieces, uint64_t our_pieces, uint64_t pinnable, uint64_t targets) {
  int count = 0;
  uint64_t our_pawns = board->pieces[pla];
  uint64_t our_pawns_promoting = (pla) ? (255ULL << 8) & our_pawns : (255ULL << 48) & our_pawns;
  uint64_t our_pawns_not_promo = (~our_pawns_promoting) & our_pawns;

  our_pawns = our_pawns_promoting;
  while(our_pawns) {
    uint64_t our_pawns_next = our_pawns & (our_pawns-1);
    int i = findKing(our_pawns ^ our_pawns_next);
    uint64_t cap_mask = ((pla) ? black_pawn_attack_table[i] : white_pawn_attack_table[i]) & check_bc_mask & opp_pieces;
    uint64_t pinmask = (pinnable & (our_pawns ^ our_pawns_next)) ? getPinmask(board, i, king, opp_pieces | our_pieces) : ~0ULL;
    cap_mask &= pinmask;
    while(cap_mask) {
      uint64_t cap_mask_next = cap_mask & (cap_mask - 1);
      int j = findKing(cap_mask ^ cap_mask_next);
      new_boards[count] = bbDoMove(board, pla, pla+8, i, j);
      new_boards[count+1] = bbDoMove(board, pla, pla+6, i, j);
      new_boards[count+2] = bbDoMove(board, pla, pla+4, i, j);
      new_boards[count+3] = bbDoMove(board, pla, pla+2, i, j);
      count+=4;
      cap_mask = cap_mask_next;
    }
    our_pawns = our_pawns_next;
  }

  our_pawns = our_pawns_not_promo;
  while(our_pawns) {
    uint64_t our_pawns_next = our_pawns & (our_pawns-1);
    int i = findKing(our_pawns ^ our_pawns_next);
    uint64_t cap_mask = ((pla) ? black_pawn_attack_table[i] : white_pawn_attack_table[i]) & check_bc_mask & targets;
    uint64_t pinmask = (pinnable & (our_pawns ^ our_pawns_next)) ? getPinmask(board, i, king, opp_pieces | our_pieces) : ~0ULL;
    cap_mask &= pinmask;
    while(cap_mask) {
      uint64_t cap_mask_next = cap_mask & (cap_mask - 1);
      int j = findKing(cap_mask ^ cap_mask_next);
      new_boards[count] = bbDoMove(board, pla, pla, i, j);
      count++;
      cap_mask = cap_mask_next;
    }
    our_pawns = our_pawns_next;
  }

  return count;
}

int genBlockableCaptures(struct BitBoard* board, struct BitBoard* new_boards, int pla, int king, uint64_t check_bc_mask, uint64_t targets, uint64_t all_pieces, uint64_t pinnable) {
  // rooks, bishops, queens
  int count = 0;
  uint64_t our_bishops = board->pieces[4+pla];
  uint64_t our_rooks = board->pieces[6+pla];
  uint64_t our_queens = board->pieces[8+pla];

  while(our_bishops) {
    uint64_t our_bishops_next = our_bishops & (our_bishops-1);
    int i = findKing(our_bishops ^ our_bishops_next);
    uint64_t cap_mask = bishop_table[i] & check_bc_mask & targets;
    uint64_t pinmask = (pinnable & (our_bishops ^ our_bishops_next)) ? getPinmask(board, i, king, all_pieces) : ~0ULL;
    cap_mask &= pinmask;
    while (cap_mask)
    {
      uint64_t cap_mask_next = cap_mask & (cap_mask - 1);
      int j = findKing(cap_mask ^ cap_mask_next);
      if (!(block_masks[i*64+j] & all_pieces)) {
        new_boards[count] = bbDoMove(board, pla+4, pla+4, i, j);
        count++;
      }
      cap_mask = cap_mask_next;
    }
    our_bishops = our_bishops_next;
  }

  while(our_rooks) {
    uint64_t our_rooks_next = our_rooks & (our_rooks-1);
    int i = findKing(our_rooks ^ our_rooks_next);
    uint64_t cap_mask = rook_table[i] & check_bc_mask & targets;
    uint64_t pinmask = (pinnable & (our_rooks_next ^ our_rooks)) ? getPinmask(board, i, king, all_pieces) : ~0ULL;
    cap_mask &= pinmask;
    while (cap_mask)
    {
      uint64_t cap_mask_next = cap_mask & (cap_mask - 1);
      int j = findKing(cap_mask ^ cap_mask_next);
      if (!(block_masks[i*64+j] & all_pieces)) {
        new_boards[count] = bbDoMove(board, pla+6, pla+6, i, j);
        count++;
      }
      cap_mask = cap_mask_next;
    }
    our_rooks = our_rooks_next;
  }

  while (our_queens)
  {
    uint64_t our_queens_next = our_queens & (our_queens-1);
    int i = findKing(our_queens ^ our_queens_next);
    uint64_t cap_mask = (rook_table[i] | bishop_table[i]) & check_bc_mask & targets;
    uint64_t pinmask = (pinnable & (our_queens ^ our_queens_next)) ? getPinmask(board, i, king, all_pieces) : ~0ULL;
    cap_mask &= pinmask;
    while (cap_mask)
    {
      uint64_t cap_mask_next = cap_mask & (cap_mask - 1);
      int j = findKing(cap_mask ^ cap_mask_next);
      if (!(block_masks[i*64+j] & all_pieces)) {
        new_boards[count] = bbDoMove(board, pla+8, pla+8, i, j);
        count++;
      }
      cap_mask = cap_mask_next;
    }
    our_queens = our_queens_next;
  }
  
  return count;
}

int genKnightCaps(struct BitBoard* board, struct BitBoard* new_boards, int pla, int king, uint64_t check_bc_mask, uint64_t targets, uint64_t all_pieces, uint64_t pinnable) {
  int count = 0;
  
  uint64_t our_knights = board->pieces[2+pla];

  while (our_knights)
  {
    uint64_t our_knights_next = our_knights & (our_knights-1);
    int i = findKing(our_knights ^ our_knights_next);
    uint64_t cap_mask = knight_table[i] & check_bc_mask & targets;
    uint64_t pinmask = (pinnable & (our_knights ^ our_knights_next)) ? getPinmask(board, i, king, all_pieces) : ~0ULL;
    cap_mask &= pinmask;
    while (cap_mask)
    {
      uint64_t cap_mask_next = cap_mask & (cap_mask - 1);
      int j = findKing(cap_mask ^ cap_mask_next);
      new_boards[count] = bbDoMove(board, pla+2, pla+2, i, j);
      count++;
      cap_mask = cap_mask_next;
    }
    our_knights = our_knights_next;
  }

  return count;
}

int genKingCaptures(struct BitBoard* board, struct BitBoard* new_boards, int pla, int king, uint64_t targets, uint64_t king_move_mask) {
  int count = 0;
  
  uint64_t dests = targets & king_move_mask;
  while(dests) {
    uint64_t d_next = dests & (dests-1);
    int j = findKing(d_next ^ dests);
    new_boards[count] = bbDoMove(board, 10+pla, 10+pla, king, j);
    count++;
    dests = d_next;
  }

  return count;
}

int genPawnNonCaps(struct BitBoard* board, struct BitBoard* new_boards, int pla, int king, uint64_t check_bc_mask, uint64_t all_pieces, uint64_t pinnable, uint64_t filter) {

  int count = 0;
  uint64_t our_pawns = board->pieces[pla] & filter;
  uint64_t our_pawns_promoting = (pla) ? (255ULL << 8) & our_pawns : (255ULL << 48) & our_pawns;
  uint64_t our_pawns_home_rank = (pla) ? (255ULL << 48) & our_pawns : (255ULL << 8) & our_pawns;
  uint64_t our_pawns_not_promo = (~our_pawns_promoting) & ~(our_pawns_home_rank) & our_pawns;

  our_pawns = our_pawns_promoting;
  while(our_pawns) {
    uint64_t our_pawns_next = our_pawns & (our_pawns-1);
    int i = findKing(our_pawns ^ our_pawns_next);
    uint64_t push_mask = ((pla) ? black_pawn_move_table[i] : white_pawn_move_table[i]) & check_bc_mask & ~all_pieces;
    uint64_t pinmask = (pinnable & (our_pawns ^ our_pawns_next)) ? getPinmask(board, i, king, all_pieces) : ~0ULL;
    push_mask &= pinmask;
    while(push_mask) {
      uint64_t push_mask_next = push_mask & (push_mask - 1);
      int j = findKing(push_mask ^ push_mask_next);
      new_boards[count] = bbDoMove(board, pla, pla+8, i, j);
      new_boards[count+1] = bbDoMove(board, pla, pla+6, i, j);
      new_boards[count+2] = bbDoMove(board, pla, pla+4, i, j);
      new_boards[count+3] = bbDoMove(board, pla, pla+2, i, j);
      count+=4;
      push_mask = push_mask_next;
    }
    our_pawns = our_pawns_next;
  }

  our_pawns = our_pawns_home_rank;
  while(our_pawns) {
    uint64_t our_pawns_next = our_pawns & (our_pawns-1);
    int i = findKing(our_pawns ^ our_pawns_next);
    uint64_t push_mask = ((pla) ? black_pawn_move_table[i] : white_pawn_move_table[i]) & check_bc_mask & ~all_pieces;
    uint64_t pinmask = (pinnable & (our_pawns ^ our_pawns_next)) ? getPinmask(board, i, king, all_pieces) : ~0ULL;
    push_mask &= pinmask;
    while(push_mask) {
      uint64_t push_mask_next = push_mask & (push_mask - 1);
      int j = findKing(push_mask ^ push_mask_next);
      if (!(block_masks[64*i + j] & all_pieces)) {
        new_boards[count] = bbDoMove(board, pla, pla, i, j);
        count++;
      }
      push_mask = push_mask_next;
    }
    our_pawns = our_pawns_next;
  }

  our_pawns = our_pawns_not_promo;
  while(our_pawns) {
    uint64_t our_pawns_next = our_pawns & (our_pawns-1);
    int i = findKing(our_pawns ^ our_pawns_next);
    uint64_t push_mask = ((pla) ? black_pawn_move_table[i] : white_pawn_move_table[i]) & check_bc_mask & ~all_pieces;
    uint64_t pinmask = (pinnable & (our_pawns ^ our_pawns_next)) ? getPinmask(board, i, king, all_pieces) : ~0ULL;
    push_mask &= pinmask;
    while(push_mask) {
      uint64_t push_mask_next = push_mask & (push_mask - 1);
      int j = findKing(push_mask ^ push_mask_next);
      new_boards[count] = bbDoMove(board, pla, pla, i, j);
      count++;
      push_mask = push_mask_next;
    }
    our_pawns = our_pawns_next;
  }



  return count;

}

int genBlockableNonCaps(struct BitBoard* board, struct BitBoard* new_boards, int pla, int king, uint64_t check_bc_mask, uint64_t all_pieces, uint64_t pinnable) {
  // rooks, bishops, queens
  int count = 0;
  uint64_t our_bishops = board->pieces[4+pla];
  uint64_t our_rooks = board->pieces[6+pla];
  uint64_t our_queens = board->pieces[8+pla];
  
  while(our_bishops) {
    uint64_t our_bishops_next = our_bishops & (our_bishops-1);
    int i = findKing(our_bishops ^ our_bishops_next);
    uint64_t move_mask = bishop_table[i] & check_bc_mask & ~all_pieces;
    uint64_t pinmask = (pinnable & (our_bishops ^ our_bishops_next)) ? getPinmask(board, i, king, all_pieces) : ~0ULL;
    move_mask &= pinmask;
    while (move_mask)
    {
      uint64_t move_mask_next = move_mask & (move_mask - 1);
      int j = findKing(move_mask ^ move_mask_next);
      if (!(block_masks[i*64+j] & all_pieces)) {
        new_boards[count] = bbDoMove(board, pla+4, pla+4, i, j);
        count++;
      }
      move_mask = move_mask_next;
    }
    our_bishops = our_bishops_next;
  }

  while(our_rooks) {
    uint64_t our_rooks_next = our_rooks & (our_rooks-1);
    int i = findKing(our_rooks ^ our_rooks_next);
    uint64_t move_mask = rook_table[i] & check_bc_mask & ~all_pieces;
    uint64_t pinmask = (pinnable & (our_rooks_next ^ our_rooks)) ? getPinmask(board, i, king, all_pieces) : ~0ULL;
    move_mask &= pinmask;
    while (move_mask)
    {
      uint64_t move_mask_next = move_mask & (move_mask - 1);
      int j = findKing(move_mask ^ move_mask_next);
      if (!(block_masks[i*64+j] & all_pieces)) {
        new_boards[count] = bbDoMove(board, pla+6, pla+6, i, j);
        count++;
      }
      move_mask = move_mask_next;
    }
    our_rooks = our_rooks_next;
  }

  while (our_queens)
  {
    uint64_t our_queens_next = our_queens & (our_queens-1);
    int i = findKing(our_queens ^ our_queens_next);
    uint64_t move_mask = (rook_table[i] | bishop_table[i]) & check_bc_mask & ~all_pieces;
    uint64_t pinmask = (pinnable & (our_queens ^ our_queens_next)) ? getPinmask(board, i, king, all_pieces) : ~0ULL;
    move_mask &= pinmask;
    while (move_mask)
    {
      uint64_t move_mask_next = move_mask & (move_mask - 1);
      int j = findKing(move_mask ^ move_mask_next);
      if (!(block_masks[i*64+j] & all_pieces)) {
        new_boards[count] = bbDoMove(board, pla+8, pla+8, i, j);
        count++;
      }
      move_mask = move_mask_next;
    }
    our_queens = our_queens_next;
  }
  
  return count;
}

int genKnightNonCaps(struct BitBoard* board, struct BitBoard* new_boards, int pla, int king, uint64_t check_bc_mask, uint64_t all_pieces, uint64_t pinnable) {
  int count = 0;
  
  uint64_t our_knights = board->pieces[2+pla];

  while (our_knights)
  {
    uint64_t our_knights_next = our_knights & (our_knights-1);
    int i = findKing(our_knights ^ our_knights_next);
    uint64_t cap_mask = knight_table[i] & check_bc_mask & ~all_pieces;
    uint64_t pinmask = (pinnable & (our_knights ^ our_knights_next)) ? getPinmask(board, i, king, all_pieces) : ~0ULL;
    cap_mask &= pinmask;
    while (cap_mask)
    {
      uint64_t cap_mask_next = cap_mask & (cap_mask - 1);
      int j = findKing(cap_mask ^ cap_mask_next);
      new_boards[count] = bbDoMove(board, pla+2, pla+2, i, j);
      count++;
      cap_mask = cap_mask_next;
    }
    our_knights = our_knights_next;
  }
  return count;
}

int genKingNonCaptures(struct BitBoard* board, struct BitBoard* new_boards, int pla, int king, uint64_t all_pieces, uint64_t king_move_mask) {
  int count = 0;
  
  uint64_t dests =  king_move_mask & ~all_pieces;
  while(dests) {
    uint64_t d_next = dests & (dests-1);
    int j = findKing(d_next ^ dests);
    new_boards[count] = bbDoMove(board, 10+pla, 10+pla, king, j);
    count++;
    dests = d_next;
  }

  return count;
}

int genMoves(struct BitBoard *board, struct BitBoard *new_boards, int cap_only, int just_one)
{
  int count = 0;
  int pla = board->ply_count & 1;
  int opp = (pla) ? 0 : 1;
  int our_king = findKing(board->pieces[10 + pla]);

  uint64_t check_bc_mask = checkBlockCapMask(board);
  uint64_t king_move_mask = kingMoveMask(board);

  uint64_t our_pieces = board->pieces[pla] | board->pieces[pla + 2] | board->pieces[pla + 4] | board->pieces[pla + 6] | board->pieces[pla + 8] | board->pieces[pla + 10];
  uint64_t opp_pieces = board->pieces[opp] | board->pieces[opp + 2] | board->pieces[opp + 4] | board->pieces[opp + 6] | board->pieces[opp + 8] | board->pieces[opp + 10];

  uint64_t cap_tgts = (~check_bc_mask || !cap_only) ? opp_pieces : 0;

  for (int i = 5; i > cap_only && i >= 1; i--) {
    cap_tgts |= board->pieces[opp+2*i-2];
  }

  uint64_t co_mask = (~check_bc_mask || (!cap_only)) ? ~0ULL : opp_pieces; // this ensures if we are doing captures only
  if (~check_bc_mask)
  {
    for (int i = 1; i < cap_only && i < 6; i++)
    {
      co_mask &= ~(board->pieces[2 * i + opp - 2]);
    }
  }

  uint64_t p_bits = board->pieces[pla];
  uint64_t n_bits = board->pieces[pla + 2];
  uint64_t b_bits = board->pieces[pla + 4];
  uint64_t q_bits = board->pieces[pla + 6];
  uint64_t k_bits = board->pieces[pla + 8];
  
  /*
  while (p_bits)
  {
    uint64_t pawn = p_bits & (p_bits - 1);
    p_bits ^= pawn;
    int i = findKing(pawn);
    uint64_t capture_mask = opp_pieces & ((pla) ? black_pawn_attack_table[i] : white_pawn_attack_table[i]);
  }*/

  uint64_t pinnable = 0ULL;
  if (diagonals[our_king] & (board->pieces[4+opp] | board->pieces[8+opp])) pinnable |= diagonals[our_king];
  if (antidiagonals[our_king] & (board->pieces[4+opp] | board->pieces[8+opp])) pinnable |= antidiagonals[our_king];
  if (ranks[our_king] & (board->pieces[6+opp] | board->pieces[8+opp])) pinnable |= ranks[our_king];
  if (files[our_king] & (board->pieces[6+opp] | board->pieces[8+opp])) pinnable |= files[our_king];
  
  pinnable &= ~(1ULL << our_king);

  uint64_t promo_filter = (check_bc_mask == ~0ULL && cap_only) ? (255ULL << (8 + 40*opp)) : ~0ULL;
  
  count += genPawnCaptures(board, new_boards+count, pla, our_king, check_bc_mask, opp_pieces, our_pieces, pinnable, cap_tgts);
  if (just_one && count) return count;
  count += genKnightCaps(board, new_boards+count, pla, our_king, check_bc_mask, cap_tgts, opp_pieces | our_pieces, pinnable);
  if (just_one && count) return count;
  count += genBlockableCaptures(board, new_boards+count, pla, our_king, check_bc_mask, cap_tgts, opp_pieces | our_pieces, pinnable);
  if (just_one && count) return count;
  count += genKingCaptures(board, new_boards+count, pla, our_king, cap_tgts, king_move_mask);
  if (just_one && count) return count;
  if (!cap_only || check_bc_mask != ~0ULL) {
    count += genBlockableNonCaps(board, new_boards+count, pla, our_king, check_bc_mask, opp_pieces | our_pieces, pinnable);
    if (just_one && count) return count;
    count += genKnightNonCaps(board, new_boards+count, pla, our_king, check_bc_mask, opp_pieces | our_pieces, pinnable);
    if (just_one && count) return count;
    count += genPawnNonCaps(board, new_boards+count, pla, our_king, check_bc_mask, opp_pieces | our_pieces, pinnable, promo_filter);
    if (just_one && count) return count;
    count += genKingNonCaptures(board, new_boards+count, pla, our_king, opp_pieces | our_pieces, king_move_mask);
    if (just_one && count) return count;
  }

  /*
  for (int i = 0; i < 64; i++)
  {

    if (!(our_pieces & (1ULL << i)))
      continue; // our piece isnt here
    int i_piece = getPieceAt(board, i, pla);

    uint64_t pinmask = (pinnable & (1ULL << i)) ? getPinmask(board, i, our_king, opp_pieces | our_pieces) : ~0ULL;

    uint64_t move_mask;
    uint64_t capture_mask;
    if (i_piece == 0)
      move_mask = white_pawn_move_table[i];
    if (i_piece == 1)
      move_mask = black_pawn_move_table[i];
    if ((i_piece | 1) == 3)
      move_mask = knight_table[i];
    if ((i_piece | 1) == 5)
      move_mask = bishop_table[i];
    if ((i_piece | 1) == 7)
      move_mask = rook_table[i];
    if ((i_piece | 1) == 9)
      move_mask = bishop_table[i] | rook_table[i];
    if ((i_piece | 1) == 11)
      move_mask = king_move_mask;

    capture_mask = move_mask;
    if (i_piece == 0)
      capture_mask = white_pawn_attack_table[i];
    if (i_piece == 1)
      capture_mask = black_pawn_attack_table[i];

    capture_mask &= opp_pieces;
    move_mask &= ~(opp_pieces | our_pieces);

    uint64_t promo_mask = (i_piece <= 1) ? 255ULL << (56 * opp) : 0;
    uint64_t i_mask = (move_mask | capture_mask) & pinmask & (co_mask | promo_mask);
    // allow all promotions during quiescence
    if (i != our_king)
      i_mask &= check_bc_mask;

    // filters target squares by direct line of sight for these pieces
    if ((i_piece | 1) == 1 || (i_piece | 1) == 5 || (i_piece | 1) == 7 || (i_piece | 1) == 9)
    {
      uint64_t i_mask_cpy = i_mask;
      while (i_mask_cpy)
      {
        int p = findKing(i_mask_cpy);
        if (block_masks[64 * p + i] & (opp_pieces | our_pieces))
          i_mask ^= (1ULL << p);
        i_mask_cpy ^= (1ULL << p);
      }
    }

    while (i_mask)
    {
      int j = findKing(i_mask);
      if ((i_piece == 0 && j >= 56) || (i_piece == 1 && j <= 7))
      { // promoting
        new_boards[count] = bbDoMove(board, i_piece, i_piece + 2, i, j);
        new_boards[count + 1] = bbDoMove(board, i_piece, i_piece + 4, i, j);
        new_boards[count + 2] = bbDoMove(board, i_piece, i_piece + 6, i, j);
        new_boards[count + 3] = bbDoMove(board, i_piece, i_piece + 8, i, j);
        count += 4;
      }
      else
      { // not promoting
        new_boards[count] = bbDoMove(board, i_piece, i_piece, i, j);
        count++;
      }
      i_mask ^= (1ULL << j);
    }

    if (just_one && count > 0)
      return count;
  }
  */
  // en passant and castles dealt with here. NOTE that it is impossible to double check with a pawn advancement due to geometry
  // so if an enpassantable pawn is checking the king, en passant auto-resolves the check. It is also impossible to castle through check
  // so we need to use another mask to check this. can't castle out of check but we can just check that check_bc_mask == ~0ULL

  if (~check_bc_mask == 0 && !cap_only)
  {
    // castles
    uint8_t castle_bits = board->c_rights >> (2 * pla);
    if (castle_bits & 1 && !(block_masks[our_king * 64 + our_king - 3] & (our_pieces | opp_pieces)))
    {
      char flag = 1;
      for (int i = our_king - 2; i <= our_king; i++)
      {
        if (squareAttackedBy(board, i, 0, pla))
          flag = 0;
      }
      if (flag)
      {
        new_boards[count] = bbDoCastles(board, 1);
        count++;
      }
    }
    if (castle_bits & 2 && !(block_masks[our_king * 64 + our_king + 4] & (our_pieces | opp_pieces)))
    {
      char flag = 1;
      for (int i = our_king; i <= our_king + 2; i++)
      {
        if (squareAttackedBy(board, i, 0, pla))
          flag = 0;
      }
      if (flag)
      {
        new_boards[count] = bbDoCastles(board, 0);
        count++;
      }
    }
  }
  if (just_one && count) return count;
  
  // now generate en passants. Note that if our king and an enemy rook/queen is on the same rank and either side of the pawns
  // then we must check that en passant wont discover our king.
  if (board->enpassant < 8)
  {
    int targ_sq = (opp) ? board->enpassant + 40 : board->enpassant + 16;
    int targ_pawn = (opp) ? board->enpassant + 32 : board->enpassant + 24;
    if (!(check_bc_mask & (1ULL << targ_pawn)))
      return count; // this prevents us doing en passant if we are in a check that cannot be resolved by the move.
    uint64_t ep_pawn_mask = ((pla) ? white_pawn_attack_table[targ_sq] : black_pawn_attack_table[targ_sq]) & board->pieces[pla];
    while (ep_pawn_mask)
    {
      int i = findKing(ep_pawn_mask);

      uint64_t pinmask = (pinnable & (1ULL << i)) ? getPinmask(board, i, our_king, opp_pieces | our_pieces) : ~0ULL;
      uint64_t orth_threats = ranks[our_king] & (board->pieces[opp+6] | board->pieces[opp+8]);
      uint64_t ot_blockers = (our_pieces | opp_pieces) & ~((1ULL << i) | (1ULL << targ_pawn));
      
      int flag = 1;
      while(orth_threats) { // check for cases where en passant would discover our king
        uint64_t oth_new = orth_threats & (orth_threats - 1);
        if (!(block_masks[our_king*64 + findKing(oth_new ^ orth_threats)] & ot_blockers)) {
          flag = 0;
          break;
        }
        orth_threats = oth_new; 
      }
      
      if (pinmask & (1ULL << targ_sq) && flag)
      {
        new_boards[count] = bbDoEnpassant(board, i, targ_sq, targ_pawn);
        count++;
      }

      ep_pawn_mask ^= (1ULL << i);
    }
  }

  return count;
}

struct BitBoard doMove(struct BitBoard *board, uint16_t move)
{
  int o = move % 64;
  int d = (move / 64) % 64;

  int pla = board->ply_count & 1;
  int sp = getPieceAt(board, o, pla);

  if ((sp == 10 || sp == 11) && (o - d == 2))
    return bbDoCastles(board, 1);
  if ((sp == 10 || sp == 11) && (d - o == 2))
    return bbDoCastles(board, 0);

  if ((sp == 0 || sp == 1) && (move >> 14))
  {
    int en_p = (o - d == 9 || o - d == -7) ? o - 1 : o + 1;
    return bbDoEnpassant(board, o, d, en_p);
  }

  int ep = sp;
  if (sp == pla && (d >= 56 || d <= 7))
    ep = ((move / 4096) % 4) * 2 + pla + 2;

  return bbDoMove(board, sp, ep, o, d);
}

struct BitBoard startBoard()
{

  struct BitBoard b;
  for (int i = 0; i < 12; i++)
  {
    b.pieces[i] = default_pieces[i];
  }

  b.c_rights = 15;
  b.enpassant = 8;
  b.ply_count = 0;

  return b;
}

void printMask(uint64_t mask)
{
  for (int i = 7; i >= 0; i--)
  {
    for (int j = 7; j >= 0; j--)
    {
      printf(" %c", ((mask & (1ULL << (i * 8 + j))) ? '#' : '-'));
    }
    printf("\n");
  }
}

void printBitBoard(struct BitBoard b)
{

  char str_w[] = "White";
  char str_b[] = "Black";

  char *col_str = (b.ply_count & 1) ? str_b : str_w;

  printf("Last move: %d,%d | %s to move:\n", b.last_move % 64, (b.last_move / 64) % 64, col_str);

  char p = '/';
  char q = ' ';
  char t;

  for (int i = 56; i >= 0; i -= 8)
  {
    printf("+---+---+---+---+---+---+---+---+\n");
    for (int j = i + 7; j >= i; j--)
    {
      char c = p;
      for (int k = 0; k < 12; k++)
      {
        if (b.pieces[k] & (1ULL << j))
        {
          c = piece_chars[k];
          break;
        }
      }
      printf("|%c%c%c", p, c, p);
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
  printf("Stored hashcode %x | ", b.hashcode);
  printf("generated: %x\n", hashcode(&b));
}

struct BitBoard kiwipete()
{
  struct BitBoard b;
  b.c_rights = 15;
  b.enpassant = 8;
  b.ply_count = 0;
  b.pieces[0] = 0x000000100800E700ULL;
  b.pieces[1] = 0x00B40A0040010000ULL;
  b.pieces[2] = 0x0000000800200000ULL;
  b.pieces[3] = 0x0000440000000000ULL;
  b.pieces[4] = 0x0000000000001800ULL;
  b.pieces[5] = 0x0002800000000000ULL;
  b.pieces[6] = 0x0000000000000081ULL;
  b.pieces[7] = 0x8100000000000000ULL;
  b.pieces[8] = 0x0000000000040000ULL;
  b.pieces[9] = 0x0008000000000000ULL;
  b.pieces[10] = 0x0000000000000008ULL;
  b.pieces[11] = 0x0800000000000000ULL;

  return b;
}

void addPerftData(struct PerftData *a, struct PerftData b)
{
  a->nodes += b.nodes;
  a->captures += b.captures;
  a->castles += b.castles;
  a->enpassants += b.enpassants;
  a->checks += b.checks;
}

struct PerftData perftSearch(struct BitBoard *b, int depth)
{
  
  if (depth == 0)
  {
    struct PerftData pd;
    pd.nodes = 1;
    pd.captures = b->last_move_md & 1;
    pd.checks = 0;
    pd.castles = (b->last_move_md & 2) / 2;
    pd.enpassants = (b->last_move & 16384) ? 1 : 0;
    int k = findKing(b->pieces[((b->ply_count&1) ^ 1) + 10]);
    if (squareAttackedBy(b, k, 1,(b->ply_count+1)&1)){
      printf("ERROR: king capturable. Last move: %d,%d\n",b->last_move&63,(b->last_move/64)%64);
      printBitBoard(*b);
      printf("masks: diag - %lx | antidiag - %lx | rank - %lx | file = %lx\n", diagonals[k], antidiagonals[k], ranks[k], files[k]);
    }
    uint32_t hc = b->hashcode;
    if (hashcode(b) != hc) printf("ERROR: wrong hashcode: %x,%x | type: %lu\n",hc,b->hashcode, pd.captures + pd.castles*2 + pd.enpassants*4);
    

    return pd;
  }
  struct PerftData count;
  count.nodes = 0;
  count.captures = 0;
  count.checks = 0;
  count.castles = 0;
  count.enpassants = 0;
  struct BitBoard successors[500];
  int num = genMoves(b, successors, 0, 0);
  for (int i = 0; i < num; i++)
  {
    addPerftData(&count, perftSearch(successors + i, depth - 1));
  }
  return count;
}

int equivalent(struct BitBoard* b1, struct BitBoard* b2) {
  if (b1->hashcode != b2->hashcode) return 0;
  if ((b1->ply_count&1) != (b2->ply_count&1)) return 0;
  if ((b1->c_rights & 15 != b2->c_rights & 15)) return 0;
  if ((b1->enpassant != b2->enpassant)) return 0; 
  for (int i = 0; i < 12; i++) {
    if (b1->pieces[i] != b2->pieces[i]) return 0;
  }
}

uint64_t perft(struct BitBoard b, int depth)
{
  // printBitBoard(b);
  initialiseSeeds();
  hashcode(&b);
  struct PerftData pd = perftSearch(&b, depth);

  printf("depth %d: %ld nodes, %ld captures, %ld enpassants , %ld checks, %ld castles \n", depth, pd.nodes, pd.captures, pd.enpassants, pd.checks, pd.castles);
  return pd.nodes;
}
