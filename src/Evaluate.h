#ifndef EVALUATE_H
#define EVALUATE_H

#include "BoardHelpers.h"
#include "PlaySide.h"

#define DOUBLE_PAWNS_S -0.5
#define ISOLATED_PAWNS_S -0.5
#define KNIGHT_EARLY_S 1.5
#define ATTACK_NEAR_KING_S 1.5
#define MOBILITY_S 0.05

enum PieceScore {
  PAWN_S = 1, ROOK_S = 5,
  KNIGHT_S = 3, BISHOP_S = 3,
  QUEEN_S = 9, KING_S = 1000,
  P_ROOK_S = 0, P_KNIGHT_S = 6,
  P_BISHOP_S = 0, P_QUEEN_S = 17,
  CASTLE_S = 3, CENTER_PAWN = 2,
};

double evaluate_basic(BoardConfig config, bool canCastle, int possible_moves, int side);

double evaluate_early(BoardConfig config, bool canCastle, int side);

double evaluate_late(BoardConfig config, bool canCastle, int side);

#endif