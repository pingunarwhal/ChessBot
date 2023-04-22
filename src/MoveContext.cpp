#include "MoveContext.h"

MoveContext::MoveContext(Bot bot, Move* move) {
  for (int i = 1; i <= TABLE_SIZE; i++) {
    for (int j = 1; j <= TABLE_SIZE; j++) {
      this->currentBoard[i][j] = bot.gameBoard[i][j]; 
    }
  }

  this->shortCastle = bot.shortCastle;
  this->longCastle = bot.longCastle;
  
  for (auto i : bot.myCapturedPieces) {
    this->myCapturedPieces.push_back(i);
  }

  this->actualMove = move;
}