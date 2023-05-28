#ifndef EVALUATE_H
#define EVALUATE_H

#include "BoardHelpers.h"
#include "PlaySide.h"

#define DOUBLE_PAWNS_S -0.5
#define ISOLATED_PAWNS_S -0.5
#define KNIGHT_EARLY_S 1.5
#define ATTACK_NEAR_KING_S 1.5
#define MOBILITY_S 0.05
#define BISHOP_CONTROL_S 1
#define ROOK_CONTROL_S 1.5
#define KING_S 1000.0

enum PieceScore {
  PAWN_S = 1, ROOK_S = 5,
  KNIGHT_S = 3, BISHOP_S = 3,
  QUEEN_S = 9,
  P_ROOK_S = 0, P_KNIGHT_S = 6,
  P_BISHOP_S = 0, P_QUEEN_S = 17,
  CASTLE_S = 3, CENTER_PAWN = 2,
  PAWN_STRUCTURE = 1
};

bool check_file_isolated(BoardConfig config, int fileToCheck, PlaySide side);

double pawnPosition(BoardConfig config, int i, int j);

double pawnControl(BoardConfig config);

double knightsEarly(BoardConfig config);

double bishopControl(BoardConfig config);

double rookControl(BoardConfig config);

double evaluate_basic(BoardConfig config, bool canCastle, int possible_moves, PlaySide side);

double evaluate_early(BoardConfig config, bool canCastle, int possible_moves, PlaySide side, PlaySide engineSide);

double evaluate_late(BoardConfig config, bool canCastle, int possible_moves, PlaySide side, PlaySide engineSide);

#endif