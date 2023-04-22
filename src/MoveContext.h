#ifndef MOVECONTEXT_H
#define MOVECONTEXT_H
#include <bits/stdc++.h>

#include "Bot.h"

class MoveContext {
  public:
    PlaySidePiece currentBoard[TABLE_SIZE + 1][TABLE_SIZE + 1];
    Castling shortCastle; /* Short castling available */
    Castling longCastle;
    std::vector<PlaySidePiece> myCapturedPieces;
    Move* actualMove;

    MoveContext(Bot bot, Move* move);
};
#endif