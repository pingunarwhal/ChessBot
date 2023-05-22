#ifndef EVALUATE_H
#define EVALUATE_H

#include "BoardHelpers.h"

bool isWhite(PlaySidePiece piece);

double evaluate_basic(BoardConfig config, bool canCastle, int side);

double evaluate_early(BoardConfig config, bool canCastle, int side);

double evaluate_late(BoardConfig config, bool canCastle, int side);

#endif