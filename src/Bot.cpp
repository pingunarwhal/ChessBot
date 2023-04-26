#include "Bot.h"

#include <bits/stdc++.h>

extern PlaySide engineSide;

const std::string Bot::BOT_NAME = "MyBot"; /* Edit this, escaped characters are forbidden */

GameConfig::GameConfig(PlaySidePiece
                       currentConfig[TABLE_SIZE + 1][TABLE_SIZE + 1]) {
  for (int i = 1; i <= TABLE_SIZE; i++) {
    for (int j = 1; j <= TABLE_SIZE; j++) {
      this->config[i][j] = currentConfig[i][j];
    }
  }
}

Bot::Bot() { /* Initialize custom fields here */
  setGameBoard();
  kingCheck = NO_CHECK;
  previousKingCheck = NO_CHECK;
  shortCastle = CAN_CASTLE;
  longCastle = CAN_CASTLE;
  threeRepeatedConfigs = false;
}

void Bot::setGameBoard() {
  for (int i = 1; i <= TABLE_SIZE; i++) {
    for (int j = 1; j <= TABLE_SIZE; j++) {
      gameBoard[i][j] = NO_PIECE;
    }
  }

  gameBoard[1][1] = WHITE_ROOK;
  gameBoard[1][2] = WHITE_KNIGHT;
  gameBoard[1][3] = WHITE_BISHOP;
  gameBoard[1][4] = WHITE_QUEEN;
  gameBoard[1][5] = WHITE_KING;
  gameBoard[1][6] = WHITE_BISHOP;
  gameBoard[1][7] = WHITE_KNIGHT;
  gameBoard[1][8] = WHITE_ROOK;

  for (int i = 1; i <= TABLE_SIZE; i++) {
    gameBoard[2][i] = WHITE_PAWN;
    gameBoard[7][i] = BLACK_PAWN;
  }

  gameBoard[8][1] = BLACK_ROOK;
  gameBoard[8][2] = BLACK_KNIGHT;
  gameBoard[8][3] = BLACK_BISHOP;
  gameBoard[8][4] = BLACK_QUEEN;
  gameBoard[8][5] = BLACK_KING;
  gameBoard[8][6] = BLACK_BISHOP;
  gameBoard[8][7] = BLACK_KNIGHT;
  gameBoard[8][8] = BLACK_ROOK;
}

void getCoordinates(std::string position, int &x, int &y) {
  y = position[0] - 'a' + 1;
  x = position[1] - '0';
}

void Bot::findKing(PlaySidePiece king, int &x, int &y) {
  for (int i = 1; i <= TABLE_SIZE; i++) {
    for (int j = 1; j <= TABLE_SIZE; j++) {
      if (gameBoard[i][j] == king) {
        x = i;
        y = j;
        return;
      }
    }
  }
}

bool Bot::checkPawnOnBoard(int &x, int &y) {
  if (gameBoard[x][y] == BLACK_PAWN || gameBoard[x][y] == WHITE_PAWN ||
      gameBoard[x][y] == BLACK_EN_PAS || gameBoard[x][y] == WHITE_EN_PAS) {
    return true;
  }

  return false;
}

bool Bot::checkSpecialCases(PlaySide sideToMove, int &x_start, int &y_start,
                            int &x_end, int &y_end) {
  if (sideToMove == BLACK) {
    /* Black en passant from 7th to 5th row */
    if (x_start == 7 && x_end == 5 &&
        gameBoard[x_start][y_start] == BLACK_PAWN) {
      if (((y_end + 1 <= TABLE_SIZE) &&
           (gameBoard[x_end][y_end + 1] == WHITE_PAWN)) ||
        ((y_end - 1 > 0) && (gameBoard[x_end][y_end - 1] == WHITE_PAWN))) {
        gameBoard[x_end][y_end] = BLACK_EN_PAS;
        return true;
      }
    }
  } else {
    /* White en passant from 2nd to 4th row */
    if (x_start == 2 && x_end == 4 &&
        gameBoard[x_start][y_start] == WHITE_PAWN) {
      if (((y_end + 1 <= TABLE_SIZE) &&
           (gameBoard[x_end][y_end + 1] == BLACK_PAWN)) ||
        ((y_end - 1 > 0) && (gameBoard[x_end][y_end - 1] == BLACK_PAWN))) {
        gameBoard[x_end][y_end] = WHITE_EN_PAS;
        return true;
      }
    }
  }

  if (gameBoard[x_start][y_start] == BLACK_KING ||
      gameBoard[x_start][y_start] == WHITE_KING) {
    if (y_end == y_start + 2) {
      gameBoard[x_end][y_end] = gameBoard[x_start][y_start];
      gameBoard[x_end][y_end - 1] = gameBoard[x_start][y_start + 3];
      gameBoard[x_start][y_start + 3] = NO_PIECE;
      return true;
    }

    if (y_end == y_start - 2) {
      gameBoard[x_end][y_end] = gameBoard[x_start][y_start];
      gameBoard[x_end][y_end + 1] = gameBoard[x_start][y_start - 4];
      gameBoard[x_start][y_start - 4] = NO_PIECE;
      return true;
    }
  }

  return false;
}

bool Bot::checkPawnAttack(PlaySide side, PlaySidePiece pawn, int &x, int &y) {
  if (side == BLACK) {
    /* check for black pawns -- they attack on a lower x value */
    for (int i = 1; i <= TABLE_SIZE; i++) {
      for (int j = 1; j <= TABLE_SIZE; j++) {
        if (gameBoard[i][j] != pawn) {
          continue;
        }

        if (x == (i - 1) && (y == (j - 1) || y == (j + 1))) {
          return true;
        }
      }
    }
  } else {
    /* check for white pawns -- they attack on a higher x value */
    for (int i = 1; i <= TABLE_SIZE; i++) {
      for (int j = 1; j <= TABLE_SIZE; j++) {
        if (gameBoard[i][j] != pawn) {
          continue;
        }

        if (x == (i + 1) && (y == (j - 1) || y == (j + 1))) {
          return true;
        }
      }
    }
  }
  return false;
}

bool Bot::checkRookAttack(PlaySidePiece rook, int &x, int &y) {
  bool pieceBetween;

  for (int i = 1; i <= TABLE_SIZE; i++) {
    for (int j = 1; j <= TABLE_SIZE; j++) {
      if (gameBoard[i][j] != rook) {
        continue;
      }

      /* rook and king on the same row */
      if (x == i) {
        int start = std::min(y, j) + 1;
        int end = std::max(y, j);

        pieceBetween = false;

        for (; start < end; start++) {
          if (gameBoard[i][start] != NO_PIECE) {
            pieceBetween = true;
            break;
          }
        }

        if (!pieceBetween) {
          return true;
        }
      }

      /* rook and king on the same column */
      if (y == j) {
        int start = std::min(x, i) + 1;
        int end = std::max(x, i);

        pieceBetween = false;

        for (; start < end; start++) {
          if (gameBoard[start][j] != NO_PIECE) {
            pieceBetween = true;
            break;
          }
        }

        if (!pieceBetween) {
          return true;
        }
      }
    }
  }

  return false;
}

bool Bot::checkKnightAttack(PlaySidePiece knight, int &x, int &y) {
  int dx[KNIGHT_MOVES] = {2, 1, -1, -2, -2, -1, 1, 2};
  int dy[KNIGHT_MOVES] = {1, 2, 2, 1, -1, -2, -2, -1};

  for (int i = 1; i <= TABLE_SIZE; i++) {
    for (int j = 1; j <= TABLE_SIZE; j++) {
      if (gameBoard[i][j] != knight) {
        continue;
      }

      /* Knight attack implies 8 relative positions */
      for (int k = 0; k < KNIGHT_MOVES; k++) {
        if ((i + dx[k]) == x && (j + dy[k]) == y) {
          return true;
        }
      }
    }
  }

  return false;
}

bool Bot::checkBishopAttack(PlaySidePiece bishop, int &x, int &y) {
  for (int i = 1; i <= TABLE_SIZE; i++) {
    for (int j = 1; j <= TABLE_SIZE; j++) {
      if (gameBoard[i][j] != bishop) {
        continue;
      }

      /* check if the bishop attacks the king on 1 of the 4 diagonals */
      for (int k = 1; k < TABLE_SIZE; k++) {
        if ((i + k) <= TABLE_SIZE && (j + k) <= TABLE_SIZE) {
          if ((i + k) == x && (j + k) == y) {
            return true;
          }

          if (gameBoard[i + k][j + k] != NO_PIECE) {
            break;
          }
        } else {
          break;
        }
      }

      for (int k = 1; k < TABLE_SIZE; k++) {
        if ((i - k) > 0 && (j + k) <= TABLE_SIZE) {
          if ((i - k) == x && (j + k) == y) {
            return true;
          }

          if (gameBoard[i - k][j + k] != NO_PIECE) {
            break;
          }
        } else {
          break;
        }
      }

      for (int k = 1; k < TABLE_SIZE; k++) {
        if ((i - k) > 0 && (j - k) > 0) {
          if ((i - k) == x && (j - k) == y) {
            return true;
          }

          if (gameBoard[i - k][j - k] != NO_PIECE) {
            break;
          }
        } else {
          break;
        }
      }

      for (int k = 1; k < TABLE_SIZE; k++) {
        if ((i + k) <= TABLE_SIZE && (j - k) > 0) {
          if ((i + k) == x && (j - k) == y) {
            return true;
          }

          if (gameBoard[i + k][j - k] != NO_PIECE) {
            break;
          }
        } else {
          break;
        }
      }
    }
  }

  return false;
}

bool Bot::checkQueenAttack(PlaySidePiece queen, int &x, int &y) {
  /* Queen is just a rook + bishop */
  return (checkRookAttack(queen, x, y) | checkBishopAttack(queen, x, y));
}

bool Bot::checkKingAttack(PlaySidePiece king, int &x, int &y) {
  for (int i = 1; i <= TABLE_SIZE; i++) {
    for (int j = 1; j <= TABLE_SIZE; j++) {
      if (gameBoard[i][j] != king) {
        continue;
      }

      /* Another king needs to be in one of the
       * adjacent 9 fields on board
       */
      if (std::abs(x - i) <= 1 && std::abs(y - j) <= 1) {
        return true;
      }
    }
  }

  return false;
}

bool Bot::checkKingSafety(PlaySide sideToMove) {
  int x_king, y_king;

  if (sideToMove == BLACK) {
    /* check for white king's safety */
    findKing(WHITE_KING, x_king, y_king);
    return !(checkPawnAttack(sideToMove, BLACK_PAWN, x_king, y_king) |
             checkPawnAttack(sideToMove, BLACK_EN_PAS, x_king, y_king) |
             checkRookAttack(BLACK_ROOK, x_king, y_king) |
             checkKnightAttack(BLACK_KNIGHT, x_king, y_king) |
             checkBishopAttack(BLACK_BISHOP, x_king, y_king) |
             checkQueenAttack(BLACK_QUEEN, x_king, y_king) |
             checkKingAttack(BLACK_KING, x_king, y_king));
  } else {
    /* check for black king's safety */
    findKing(BLACK_KING, x_king, y_king);
    return !(checkPawnAttack(sideToMove, WHITE_PAWN, x_king, y_king) |
             checkPawnAttack(sideToMove, WHITE_EN_PAS, x_king, y_king) |
             checkRookAttack(WHITE_ROOK, x_king, y_king) |
             checkKnightAttack(WHITE_KNIGHT, x_king, y_king) |
             checkBishopAttack(WHITE_BISHOP, x_king, y_king) |
             checkQueenAttack(WHITE_QUEEN, x_king, y_king) |
             checkKingAttack(WHITE_KING, x_king, y_king));
  }

  return true;
}

PlaySidePiece Bot::selectPiece(Piece piece, PlaySide enemySide) {
  switch (enemySide) {
    case WHITE:
      switch (piece) {
        case PAWN:
          return BLACK_PAWN;
        case ROOK:
          return BLACK_ROOK;
        case KNIGHT:
          return BLACK_KNIGHT;
        case BISHOP:
          return BLACK_BISHOP;
        case QUEEN:
          return BLACK_QUEEN;
        default:
          return NO_PIECE;
      }
      break;

    case BLACK:
      switch (piece) {
        case PAWN:
          return WHITE_PAWN;
        case ROOK:
          return WHITE_ROOK;
        case KNIGHT:
          return WHITE_KNIGHT;
        case BISHOP:
          return WHITE_BISHOP;
        case QUEEN:
          return WHITE_QUEEN;
        default:
          return NO_PIECE;
      }
    default:
      return NO_PIECE;
  }
}

PlaySidePiece Bot::convertRegularPiece(int &x, int &y) {
  /* convert a pawn from en passant marked pawn to regular pawn after one move */
  if (gameBoard[x][y] == BLACK_EN_PAS) {
    return BLACK_PAWN;
  }

  if (gameBoard[x][y] == WHITE_EN_PAS) {
    return WHITE_PAWN;
  }

  /* otherwise just return the current piece */
  return gameBoard[x][y];
}

PlaySidePiece Bot::switchSide(int &x, int &y) {
  if (gameBoard[x][y] == BLACK_EN_PAS || gameBoard[x][y] == P_BLACK_ROOK ||
      gameBoard[x][y] == P_BLACK_BISHOP || gameBoard[x][y] == P_BLACK_KNIGHT ||
      gameBoard[x][y] == P_BLACK_QUEEN) {
    return WHITE_PAWN;
  }

  if (gameBoard[x][y] == WHITE_EN_PAS || gameBoard[x][y] == P_WHITE_ROOK ||
      gameBoard[x][y] == P_WHITE_BISHOP || gameBoard[x][y] == P_WHITE_KNIGHT ||
      gameBoard[x][y] == P_WHITE_QUEEN) {
    return BLACK_PAWN;
  }

  switch (gameBoard[x][y]) {
    case BLACK_PAWN:
      return WHITE_PAWN;
    case BLACK_BISHOP:
      return WHITE_BISHOP;
    case BLACK_ROOK:
      return WHITE_ROOK;
    case BLACK_KNIGHT:
      return WHITE_KNIGHT;
    case BLACK_QUEEN:
      return WHITE_QUEEN;
    case WHITE_PAWN:
      return BLACK_PAWN;
    case WHITE_ROOK:
      return BLACK_ROOK;
    case WHITE_KNIGHT:
      return BLACK_KNIGHT;
    case WHITE_BISHOP:
      return BLACK_BISHOP;
    case WHITE_QUEEN:
      return BLACK_QUEEN;
    default:
      return gameBoard[x][y];
  }
}

void Bot::copyCurrentConfig() {
  /* deep copy the current config using an auxiliary matrix */
  GameConfig currentGameBoard = GameConfig(gameBoard);

  pastConfigs.push_back(currentGameBoard);
}

bool Bot::checkRepeatedConfigs() {
  int counterRepeatedConfigs = 0;
  bool repeatedConfig;

  for (auto &pastConfig : pastConfigs) {
    repeatedConfig = true;
    for (int i = 1; i <= TABLE_SIZE && repeatedConfig; i++) {
      for (int j = 1; j <= TABLE_SIZE && repeatedConfig; j++) {
        if (pastConfig.config[i][j] != gameBoard[i][j]) {
          repeatedConfig = false;
        }
      }
    }

    if (repeatedConfig == true) {
      counterRepeatedConfigs++;
    }
  }

  if (counterRepeatedConfigs == 2) {
    return true;
  }

  return false;
}

void Bot::recordMove(Move *move, PlaySide sideToMove) {
  /* You might find it useful to also separately
   * record last move in another custom field */
  int x_start, y_start, x_end, y_end;
  getCoordinates(move->getSource().value(), x_start, y_start);
  getCoordinates(move->getDestination().value(), x_end, y_end);

  counterMoves++;

  if (checkPawnOnBoard(x_start, y_start)) {
    counterMoves = 0;
  }

  if (gameBoard[x_end][y_end] != NO_PIECE) {
    enemyCapturesPieces.push_back(switchSide(x_end, y_end));
    counterMoves = 0;
  }

  if (move->isNormal()) {
    if (!checkSpecialCases(sideToMove, x_start, y_start, x_end, y_end)) {
      gameBoard[x_end][y_end] = convertRegularPiece(x_start, y_start);
    }

    gameBoard[x_start][y_start] = NO_PIECE;
  }

  if (move->isPromotion()) {
    gameBoard[x_end][y_end] = selectPiece(move->getReplacement().value(),
                                          sideToMove);
    gameBoard[x_start][y_start] = NO_PIECE;
  }

  if (move->isDropIn()) {
    gameBoard[x_end][y_end] = selectPiece(move->getReplacement().value(),
                                          sideToMove);
  }

  if (!checkKingSafety(sideToMove)) {
    previousKingCheck = IN_CHECK;
    kingCheck = IN_CHECK;
    shortCastle = NO_CASTLE;
    longCastle = NO_CASTLE;
  }

  if (checkRepeatedConfigs()) {
    threeRepeatedConfigs = true;
  }

  copyCurrentConfig();
}

std::string Bot::getPosition(int x, int y) {
  std::string position = "";
  position += 'a' + y - 1;
  position += '0' + x;

  return position;
}

std::vector<MoveContext> Bot::moveWhitePawn(int x, int y, PlaySide side) {
  if (side == BLACK) {
    return std::vector<MoveContext>();
  }

  if (gameBoard[x][y] != WHITE_PAWN && gameBoard[x][y] != WHITE_EN_PAS) {
    return std::vector<MoveContext>();
  }

  std::vector<MoveContext> possibleMoves;

  if (x == 2 && gameBoard[x + 2][y] == NO_PIECE) {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                        getPosition(x + 2, y)));
    newMove.currentBoard[x][y] = NO_PIECE;
    /* to be or not to be en passant */
    if (((y - 1 > 0) &&
          (newMove.currentBoard[x + 2][y - 1] == BLACK_PAWN)) ||
          ((y + 1 <= TABLE_SIZE) &&
          (newMove.currentBoard[x + 2][y + 1] == BLACK_PAWN))) {
      newMove.currentBoard[x + 2][y] = WHITE_EN_PAS;
    } else {
      newMove.currentBoard[x + 2][y] = WHITE_PAWN;
    }

    if (newMove.checkKingSafety(BLACK)) {
      possibleMoves.push_back(newMove);
    }
  }

  /* promote pawn */
  if (x == 7 && gameBoard[x + 1][y] == NO_PIECE) {
    /* set current position to NO_PIECE and push back all possible promotions */
    MoveContext newMoveQ(this, Move::promote(getPosition(x, y),
                         getPosition(x + 1, y), QUEEN));
    newMoveQ.currentBoard[x][y] = NO_PIECE;
    newMoveQ.currentBoard[x + 1][y] = WHITE_QUEEN;
    if (newMoveQ.checkKingSafety(BLACK)) {
      possibleMoves.push_back(newMoveQ);
    }

    MoveContext newMoveR(this, Move::promote(getPosition(x, y),
                         getPosition(x + 1, y), ROOK));
    newMoveR.currentBoard[x][y] = NO_PIECE;
    newMoveR.currentBoard[x + 1][y] = WHITE_ROOK;
    if (newMoveR.checkKingSafety(BLACK)) {
      possibleMoves.push_back(newMoveR);
    }

    MoveContext newMoveB(this, Move::promote(getPosition(x, y),
                         getPosition(x + 1, y), BISHOP));
    newMoveB.currentBoard[x][y] = NO_PIECE;
    newMoveB.currentBoard[x + 1][y] = WHITE_BISHOP;
    if (newMoveB.checkKingSafety(BLACK)) {
      possibleMoves.push_back(newMoveB);
    }

    MoveContext newMoveK(this, Move::promote(getPosition(x, y),
                         getPosition(x + 1, y), KNIGHT));
    newMoveK.currentBoard[x][y] = NO_PIECE;
    newMoveK.currentBoard[x + 1][y] = WHITE_KNIGHT;
    if (newMoveK.checkKingSafety(BLACK)) {
      possibleMoves.push_back(newMoveK);
    }
  }

  if (gameBoard[x + 1][y] == NO_PIECE) {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                        getPosition(x + 1, y)));
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x + 1][y] = WHITE_PAWN;
    if (newMove.checkKingSafety(BLACK)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (y + 1 <= TABLE_SIZE && gameBoard[x + 1][y + 1] < NO_PIECE) {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                        getPosition(x + 1, y + 1)));
    newMove.myCapturedPieces.push_back(gameBoard[x + 1][y + 1]);
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x + 1][y + 1] = WHITE_PAWN;
    if (newMove.checkKingSafety(BLACK)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (y - 1 > 0 && gameBoard[x + 1][y - 1] < NO_PIECE) {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                        getPosition(x + 1, y - 1)));
    newMove.myCapturedPieces.push_back(gameBoard[x + 1][y - 1]);
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x + 1][y - 1] = WHITE_PAWN;
    if (newMove.checkKingSafety(BLACK)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (y - 1 > 0 && gameBoard[x][y - 1] == BLACK_EN_PAS) {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                        getPosition(x + 1, y - 1)));
    newMove.myCapturedPieces.push_back(gameBoard[x][y - 1]);
    newMove.currentBoard[x][y - 1] = NO_PIECE;
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x + 1][y - 1] = WHITE_PAWN;
    if (newMove.checkKingSafety(BLACK)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (y + 1 <= TABLE_SIZE && gameBoard[x][y + 1] == BLACK_EN_PAS) {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                        getPosition(x + 1, y + 1)));
    newMove.myCapturedPieces.push_back(gameBoard[x][y + 1]);
    newMove.currentBoard[x][y + 1] = NO_PIECE;
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x + 1][y + 1] = WHITE_PAWN;
    if (newMove.checkKingSafety(BLACK)) {
      possibleMoves.push_back(newMove);
    }
  }

  std::cout << "PAWN at: " << x << " " << y << ", has possible moves: " <<
      possibleMoves.size() << std::endl;

  return possibleMoves;
}

std::vector<MoveContext> Bot::moveBlackPawn(int x, int y, PlaySide side) {
  if (side == WHITE) {
    return std::vector<MoveContext>();
  }

  if (gameBoard[x][y] != BLACK_PAWN && gameBoard[x][y] != BLACK_EN_PAS) {
    return std::vector<MoveContext>();
  }

  std::vector<MoveContext> possibleMoves;

  if (x == 7 && gameBoard[x - 2][y] == NO_PIECE) {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                        getPosition(x - 2, y)));
    newMove.currentBoard[x][y] = NO_PIECE;
    /* to be or not to be en passant */
    if (((y - 1 > 0) &&
        (newMove.currentBoard[x - 2][y - 1] == WHITE_PAWN)) ||
        ((y + 1 <= TABLE_SIZE) &&
        (newMove.currentBoard[x - 2][y + 1] == WHITE_PAWN))) {
      newMove.currentBoard[x - 2][y] = BLACK_EN_PAS;
    } else {
      newMove.currentBoard[x - 2][y] = BLACK_PAWN;
    }
    if (newMove.checkKingSafety(WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  /* promote pawn */
  if (x == 2 && gameBoard[x - 1][y] == NO_PIECE) {
    MoveContext newMoveQ(this, Move::promote(getPosition(x, y),
                         getPosition(x - 1, y), QUEEN));
    newMoveQ.currentBoard[x][y] = NO_PIECE;
    newMoveQ.currentBoard[x - 1][y] = BLACK_QUEEN;
    if (newMoveQ.checkKingSafety(WHITE)) {
      possibleMoves.push_back(newMoveQ);
    }

    MoveContext newMoveR(this, Move::promote(getPosition(x, y),
                         getPosition(x - 1, y), ROOK));
    newMoveR.currentBoard[x][y] = NO_PIECE;
    newMoveR.currentBoard[x - 1][y] = BLACK_ROOK;
    if (newMoveR.checkKingSafety(WHITE)) {
      possibleMoves.push_back(newMoveR);
    }

    MoveContext newMoveB(this, Move::promote(getPosition(x, y),
                         getPosition(x - 1, y), BISHOP));
    newMoveB.currentBoard[x][y] = NO_PIECE;
    newMoveB.currentBoard[x - 1][y] = BLACK_BISHOP;
    if (newMoveB.checkKingSafety(WHITE)) {
      possibleMoves.push_back(newMoveB);
    }

    MoveContext newMoveK(this, Move::promote(getPosition(x, y),
                         getPosition(x - 1, y), KNIGHT));
    newMoveK.currentBoard[x][y] = NO_PIECE;
    newMoveK.currentBoard[x - 1][y] = BLACK_KNIGHT;
    if (newMoveK.checkKingSafety(WHITE)) {
      possibleMoves.push_back(newMoveK);
    }
  }

  if (gameBoard[x - 1][y] == NO_PIECE) {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                        getPosition(x - 1, y)));
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x - 1][y] = BLACK_PAWN;
    if (newMove.checkKingSafety(WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (y + 1 <= TABLE_SIZE && gameBoard[x - 1][y + 1] > NO_PIECE) {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                        getPosition(x - 1, y + 1)));
    newMove.myCapturedPieces.push_back(gameBoard[x - 1][y + 1]);
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x - 1][y + 1] = BLACK_PAWN;
    if (newMove.checkKingSafety(WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (y - 1 > 0 && gameBoard[x - 1][y - 1] > NO_PIECE) {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                        getPosition(x - 1, y - 1)));
    newMove.myCapturedPieces.push_back(gameBoard[x - 1][y - 1]);
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x - 1][y - 1] = BLACK_PAWN;
    if (newMove.checkKingSafety(WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (y - 1 > 0 && gameBoard[x][y - 1] == WHITE_EN_PAS) {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                        getPosition(x - 1, y - 1)));
    newMove.myCapturedPieces.push_back(gameBoard[x][y - 1]);
    newMove.currentBoard[x][y - 1] = NO_PIECE;
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x - 1][y - 1] = BLACK_PAWN;
    if (newMove.checkKingSafety(WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (y + 1 <= TABLE_SIZE && gameBoard[x][y + 1] == WHITE_EN_PAS) {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                        getPosition(x - 1, y + 1)));
    newMove.myCapturedPieces.push_back(gameBoard[x][y + 1]);
    newMove.currentBoard[x][y + 1] = NO_PIECE;
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x - 1][y + 1] = BLACK_PAWN;
    if (newMove.checkKingSafety(WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  std::cout << "PAWN at: " << x << " " << y << ", has possible moves: " <<
          possibleMoves.size() << std::endl;

  return possibleMoves;
}

std::vector<MoveContext> Bot::moveKnight(int x, int y, PlaySide side) {
  if ((side == WHITE && gameBoard[x][y] != WHITE_KNIGHT) ||
      (side == BLACK && gameBoard[x][y] != BLACK_KNIGHT)) {
    return std::vector<MoveContext>();
  }

  std::vector<MoveContext> possibleMoves;

  int dx[KNIGHT_MOVES] = {2, 1, -1, -2, -2, -1, 1, 2};
  int dy[KNIGHT_MOVES] = {1, 2, 2, 1, -1, -2, -2, -1};

  for (int i = 0; i < KNIGHT_MOVES; i++) {
    if (x + dx[i] <= 0 || x + dx[i] > TABLE_SIZE ||
        y + dy[i] <= 0 || y + dy[i] > TABLE_SIZE) {
      continue;
    }

    MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                        getPosition(x + 2, y + 1)));

    if (gameBoard[x + dx[i]][y + dy[i]] != NO_PIECE) {
      /* a piece on the desired spot => check if it can be captured or not */
      if (side == WHITE && gameBoard[x + dx[i]][y + dy[i]] < NO_PIECE) {
        newMove.myCapturedPieces.push_back(gameBoard[x + dx[i]][y + dy[i]]);
        newMove.currentBoard[x][y] = NO_PIECE;
        newMove.currentBoard[x + dx[i]][y + dy[i]] = WHITE_KNIGHT;
        if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
          possibleMoves.push_back(newMove);
        }
      }

      if (side == BLACK && gameBoard[x + dx[i]][y + dy[i]] > NO_PIECE) {
        newMove.myCapturedPieces.push_back(gameBoard[x + dx[i]][y + dy[i]]);
        newMove.currentBoard[x][y] = NO_PIECE;
        newMove.currentBoard[x + dx[i]][y + dy[i]] = BLACK_KNIGHT;
        if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
          possibleMoves.push_back(newMove);
        }
      }
    } else {
      /* no piece on the desired spot => free to move there */
      newMove.currentBoard[x + dx[i]][y + dy[i]] = newMove.currentBoard[x][y];
      newMove.currentBoard[x][y] = NO_PIECE;
      if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
        possibleMoves.push_back(newMove);
      }
    }
  }

  std::cout << "KNIGHT at: " << x << " " << y << ", has possible moves: " <<
        possibleMoves.size() << std::endl;

  return possibleMoves;
}

std::vector<MoveContext> Bot::moveKing(int x, int y, PlaySide side) {
  if ((side == WHITE && gameBoard[x][y] != WHITE_KING) ||
      (side == BLACK && gameBoard[x][y] != BLACK_KING)) {
    return std::vector<MoveContext>();
  }

  std::vector<MoveContext> possibleMoves;

  int dx[KING_MOVES] = {1, 0, -1, -1, -1, 0, 1, 1};
  int dy[KING_MOVES] = {1, 1, 1, 0, -1, -1, -1, 0};

  for (int i = 0; i < KING_MOVES; i++) {
    if (x + dx[i] <= 0 || x + dx[i] > TABLE_SIZE ||
        y + dy[i] <= 0 || y + dy[i] > TABLE_SIZE) {
      continue;
    }

    MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                        getPosition(x + dx[i], y + dy[i])));

    newMove.shortCastle = NO_CASTLE;
    newMove.longCastle = NO_CASTLE;

    if (gameBoard[x + dx[i]][y + dy[i]] != NO_PIECE) {
      if (side == WHITE && gameBoard[x + dx[i]][y + dy[i]] < NO_PIECE) {
        newMove.myCapturedPieces.push_back(gameBoard[x + dx[i]][y + dy[i]]);
        newMove.currentBoard[x + dx[i]][y + dy[i]] = newMove.currentBoard[x][y];
        newMove.currentBoard[x][y] = NO_PIECE;

        if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
          possibleMoves.push_back(newMove);
        }
      }

      if (side == BLACK && gameBoard[x + dx[i]][y + dy[i]] > NO_PIECE) {
        newMove.myCapturedPieces.push_back(gameBoard[x + dx[i]][y + dy[i]]);

        newMove.currentBoard[x + dx[i]][y + dy[i]] = newMove.currentBoard[x][y];
        newMove.currentBoard[x][y] = NO_PIECE;

        if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
          possibleMoves.push_back(newMove);
        }
      }
    } else {
      newMove.currentBoard[x + dx[i]][y + dy[i]] = newMove.currentBoard[x][y];
      newMove.currentBoard[x][y] = NO_PIECE;

      if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
        possibleMoves.push_back(newMove);
      }
    }
  }

  if (shortCastle == CAN_CASTLE && gameBoard[x][y + 1] == NO_PIECE &&
      gameBoard[x][y + 2] == NO_PIECE) {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                        getPosition(x, y + 2)));
    newMove.currentBoard[x][y + 2] = newMove.currentBoard[x][y];
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x][y + 1] = newMove.currentBoard[x][TABLE_SIZE];
    newMove.currentBoard[x][TABLE_SIZE] = NO_PIECE;

    newMove.shortCastle = NO_CASTLE;
    newMove.longCastle = NO_CASTLE;

    if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (longCastle == CAN_CASTLE && gameBoard[x][y - 1] == NO_PIECE &&
      gameBoard[x][y - 2] == NO_PIECE && gameBoard[x][y - 3] == NO_PIECE) {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                        getPosition(x, y - 2)));
    newMove.currentBoard[x][y - 2] = newMove.currentBoard[x][y];
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x][y + 1] = newMove.currentBoard[x][1];
    newMove.currentBoard[x][1] = NO_PIECE;

    newMove.shortCastle = NO_CASTLE;
    newMove.longCastle = NO_CASTLE;

    if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  std::cout << "KING at: " << x << " " << y << ", has possible moves: " <<
        possibleMoves.size() << std::endl;

  return possibleMoves;
}

std::vector<MoveContext> Bot::rook_moves(int x, int y, PlaySide side) {
  if ((side == WHITE && gameBoard[x][y] < NO_PIECE) ||
      (side == BLACK && gameBoard[x][y] > NO_PIECE)) {
    return std::vector<MoveContext>();
  }
  std::vector<MoveContext> moves;

  /* right of rook */
  for (int i = x + 1; i <= TABLE_SIZE; i++) {
    if (gameBoard[i][y] != NO_PIECE && gameBoard[i][y] * gameBoard[x][y] < 0) {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                          getPosition(i, y)));
      newMove.myCapturedPieces.push_back(gameBoard[i][y]);
      newMove.currentBoard[i][y] = newMove.currentBoard[x][y];
      newMove.currentBoard[x][y] = NO_PIECE;

      if (y == 1) {
        newMove.longCastle = NO_CASTLE;
      }

      if (y == TABLE_SIZE) {
        newMove.shortCastle = NO_CASTLE;
      }

      if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
        moves.push_back(newMove);
      }

      break;
    } else if (gameBoard[i][y] == NO_PIECE) {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                          getPosition(i, y)));
      newMove.currentBoard[i][y] = newMove.currentBoard[x][y];
      newMove.currentBoard[x][y] = NO_PIECE;

      if (y == 1) {
        newMove.longCastle = NO_CASTLE;
      }

      if (y == TABLE_SIZE) {
        newMove.shortCastle = NO_CASTLE;
      }

      if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
        moves.push_back(newMove);
      }
    } else {
      break;
    }
  }

  /* left of rook */
  for (int i = x - 1; i > 0; i--) {
    if (gameBoard[i][y] != NO_PIECE && gameBoard[i][y] * gameBoard[x][y] < 0) {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                          getPosition(i, y)));
      newMove.myCapturedPieces.push_back(gameBoard[i][y]);
      newMove.currentBoard[i][y] = newMove.currentBoard[x][y];
      newMove.currentBoard[x][y] = NO_PIECE;

      if (y == 1) {
        newMove.longCastle = NO_CASTLE;
      }

      if (y == TABLE_SIZE) {
        newMove.shortCastle = NO_CASTLE;
      }

      if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
        moves.push_back(newMove);
      }

      break;
    } else if (gameBoard[i][y] == NO_PIECE) {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                          getPosition(i, y)));
      newMove.currentBoard[i][y] = newMove.currentBoard[x][y];
      newMove.currentBoard[x][y] = NO_PIECE;

      if (y == 1) {
        newMove.longCastle = NO_CASTLE;
      }

      if (y == TABLE_SIZE) {
        newMove.shortCastle = NO_CASTLE;
      }

      if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
        moves.push_back(newMove);
      }
    } else {
      break;
    }
  }

  /* down of rook */

  for (int i = y - 1; i > 0; i--) {
    if (gameBoard[x][i] != NO_PIECE && gameBoard[x][i] * gameBoard[x][y] < 0) {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                          getPosition(x, i)));
      newMove.myCapturedPieces.push_back(gameBoard[x][i]);
      newMove.currentBoard[x][i] = newMove.currentBoard[x][y];
      newMove.currentBoard[x][y] = NO_PIECE;

      if (y == 1) {
        newMove.longCastle = NO_CASTLE;
      }

      if (y == TABLE_SIZE) {
        newMove.shortCastle = NO_CASTLE;
      }

      if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
        moves.push_back(newMove);
      }
      break;
    } else if (gameBoard[x][i] == NO_PIECE) {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                          getPosition(x, i)));
      newMove.currentBoard[x][i] = newMove.currentBoard[x][y];
      newMove.currentBoard[x][y] = NO_PIECE;

      if (y == 1) {
        newMove.longCastle = NO_CASTLE;
      }

      if (y == TABLE_SIZE) {
        newMove.shortCastle = NO_CASTLE;
      }

      if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
        moves.push_back(newMove);
      }
    } else {
      break;
    }
  }

  /* up of rook */

  for (int i = y + 1; i <= TABLE_SIZE; i++) {
    if (gameBoard[x][i] != NO_PIECE && gameBoard[x][i] * gameBoard[x][y] < 0) {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                          getPosition(x, i)));
      newMove.myCapturedPieces.push_back(gameBoard[x][i]);
      newMove.currentBoard[x][i] = newMove.currentBoard[x][y];
      newMove.currentBoard[x][y] = NO_PIECE;

      if (y == 1) {
        newMove.longCastle = NO_CASTLE;
      }

      if (y == TABLE_SIZE) {
        newMove.shortCastle = NO_CASTLE;
      }

      if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
        moves.push_back(newMove);
      }
      break;
    } else if (gameBoard[x][i] == NO_PIECE) {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                          getPosition(x, i)));
      newMove.currentBoard[x][i] = newMove.currentBoard[x][y];
      newMove.currentBoard[x][y] = NO_PIECE;

      if (y == 1) {
        newMove.longCastle = NO_CASTLE;
      }

      if (y == TABLE_SIZE) {
        newMove.shortCastle = NO_CASTLE;
      }

      if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
        moves.push_back(newMove);
      }
    } else {
      break;
    }
  }

  std::cout << "ROOK at: " << x << " " << y << ", has possible moves: " <<
        moves.size() << std::endl;

  return moves;
}

std::vector<MoveContext> Bot::bishop_moves(int x, int y, PlaySide side) {
  if ((side == WHITE && gameBoard[x][y] < NO_PIECE) ||
      (side == BLACK && gameBoard[x][y] > NO_PIECE)) {
    return std::vector<MoveContext>();
  }

  std::vector<MoveContext> moves;

  for (int i = 1; i + x <= TABLE_SIZE && i + y <= TABLE_SIZE; i++) { // up right
    if (gameBoard[i + x][i + y] != NO_PIECE &&
        gameBoard[i + x][i + y] * gameBoard[x][y] < 0) {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                          getPosition(i + x, i + y)));
      newMove.myCapturedPieces.push_back(gameBoard[i + x][i + y]);
      newMove.currentBoard[i + x][i + y] = newMove.currentBoard[x][y];
      newMove.currentBoard[x][y] = NO_PIECE;

      if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
        moves.push_back(newMove);
      }

      break;
    } else if (gameBoard[x + i][y + i] == NO_PIECE) {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                          getPosition(x + i, y + i)));
      newMove.currentBoard[x + i][y + i] = newMove.currentBoard[x][y];
      newMove.currentBoard[x][y] = NO_PIECE;

      if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
        moves.push_back(newMove);
      }
    } else {
      break;
    }
  }

  for (int i = 1; x - i > 0 && y + i <= TABLE_SIZE; i++) {
    if (gameBoard[x - i][y + i] != NO_PIECE &&
        gameBoard[x - i][y + i] * gameBoard[x][y] < 0) {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                          getPosition(x - i, y + i)));
      newMove.myCapturedPieces.push_back(gameBoard[x - i][y + i]);
      newMove.currentBoard[x - i][y + i] = newMove.currentBoard[x][y];
      newMove.currentBoard[x][y] = NO_PIECE;
      if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
        moves.push_back(newMove);
      }
      break;
    } else if (gameBoard[x - i][y + i] == NO_PIECE) {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                          getPosition(x - i, y + i)));
      newMove.currentBoard[x - i][y + i] = newMove.currentBoard[x][y];
      newMove.currentBoard[x][y] = NO_PIECE;
      if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
        moves.push_back(newMove);
      }
    } else {
      break;
    }
  }

  for (int i = 1; x + i <= TABLE_SIZE && y - i > 0; i++) { // down right
    if (gameBoard[x + i][y - i] != NO_PIECE &&
        gameBoard[x + i][y - i] * gameBoard[x][y] < 0) {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                          getPosition(x + i, y - i)));
      newMove.myCapturedPieces.push_back(gameBoard[x + i][y - i]);
      newMove.currentBoard[x + i][y - i] = newMove.currentBoard[x][y];
      newMove.currentBoard[x][y] = NO_PIECE;
      if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
        moves.push_back(newMove);
      }
      break;
    } else if (gameBoard[x + i][y - i] == NO_PIECE) {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                          getPosition(x + i, y - i)));
      newMove.currentBoard[x + i][y - i] = newMove.currentBoard[x][y];
      newMove.currentBoard[x][y] = NO_PIECE;
      if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
        moves.push_back(newMove);
      }
    } else {
      break;
    }
  }

  for (int i = 1; x - i > 0 && y - i > 0; i++) { // down left
    if (gameBoard[x - i][y - i] != NO_PIECE &&
        gameBoard[x - i][y - i] * gameBoard[x][y] < 0) {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                          getPosition(x - i, y - i)));
      newMove.myCapturedPieces.push_back(gameBoard[x - i][y - i]);
      newMove.currentBoard[x - i][y - i] = newMove.currentBoard[x][y];
      newMove.currentBoard[x][y] = NO_PIECE;
      if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
        moves.push_back(newMove);
      }
      break;
    } else if (gameBoard[x - i][y - i] == NO_PIECE) {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y),
                          getPosition(x - i, y - i)));
      newMove.currentBoard[x - i][y - i] = newMove.currentBoard[x][y];
      newMove.currentBoard[x][y] = NO_PIECE;
      if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
        moves.push_back(newMove);
      }
    } else {
      break;
    }
  }

  std::cout << "BISHOP at: " << x << " " << y << ", has possible moves: " <<
        moves.size() << std::endl;

  return moves;
}

std::vector<MoveContext> Bot::queen_moves(int x, int y, PlaySide side) {
  if ((side == WHITE && gameBoard[x][y] < NO_PIECE) ||
      (side == BLACK && gameBoard[x][y] > NO_PIECE)) {
    return std::vector<MoveContext>();
  }

  auto queen_moves = rook_moves(x, y, side);
  auto queen_as_bishop_moves = bishop_moves(x, y, side);
  queen_moves.insert(queen_moves.end(), queen_as_bishop_moves.begin(),
                     queen_as_bishop_moves.end());

  std::cout << "QUEEN at: " << x << " " << y << ", has possible moves: " <<
        queen_moves.size() << std::endl;

  return queen_moves;
}
Piece Bot::convertPlaySidePiece(PlaySidePiece piece) {
  switch (piece) {
    case WHITE_PAWN:
    case BLACK_PAWN:
    case WHITE_EN_PAS:
    case BLACK_EN_PAS:
    case P_WHITE_ROOK:
    case P_WHITE_BISHOP:
    case P_WHITE_KNIGHT:
    case P_WHITE_QUEEN:
    case P_BLACK_ROOK:
    case P_BLACK_BISHOP:
    case P_BLACK_KNIGHT:
    case P_BLACK_QUEEN: {
      return PAWN;
      break;
    }
    case WHITE_ROOK:
    case BLACK_ROOK: {
      return ROOK;
      break;
    }
    case WHITE_BISHOP:
    case BLACK_BISHOP: {
      return BISHOP;
      break;
    }
    case WHITE_KNIGHT:
    case BLACK_KNIGHT: {
      return KNIGHT;
      break;
    }
    case WHITE_QUEEN:
    case BLACK_QUEEN: {
      return QUEEN;
      break;
    }
    default:
      return PAWN;
      break;
  }
}
std::vector<MoveContext> Bot::createCrazyHouse(int x, int y, PlaySide side) {
  std::vector<MoveContext> possibleMoves;

  for (auto & piece: myCapturedPieces) {
    if ((x == 1 || x == 8) && (piece == WHITE_PAWN || piece == BLACK_PAWN)) {
      continue;
    }

    // Most likely it is not legal to simulate introducing a CrazyHouse piece by movien it
    // from (i, j) to (i, j)
    MoveContext newMove(this, Move::dropIn(getPosition(x, y), convertPlaySidePiece(piece)));
    newMove.currentBoard[x][y] = piece;

    if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  return possibleMoves;
}

Move *Bot::calculateNextMove() {
  /* Play move for the side the engine is playing (Hint: Main.getEngineSide())
   * Make sure to record your move in custom structures before returning.
   *
   * Return move that you are willing to submit
   * Move is to be constructed via one of the factory methods declared in Move.h */

  std::vector<MoveContext> possibleMoves;

  for (int i = 1; i <= TABLE_SIZE; i++) {
    for (int j = 1; j <= TABLE_SIZE; j++) {
      switch (gameBoard[i][j]) {
      case WHITE_PAWN:
      case WHITE_EN_PAS: {
        std::vector<MoveContext> whitePawnMoves = moveWhitePawn(i, j,
                                                                engineSide);
        possibleMoves.insert(possibleMoves.end(), whitePawnMoves.begin(),
                             whitePawnMoves.end());
        break;
      }
      case BLACK_PAWN:
      case BLACK_EN_PAS: {
        std::vector<MoveContext> blackPawnMoves = moveBlackPawn(i, j,
                                                                engineSide);
        possibleMoves.insert(possibleMoves.end(), blackPawnMoves.begin(),
                             blackPawnMoves.end());
        break;
      }
      case WHITE_ROOK:
      case BLACK_ROOK: {
        std::vector<MoveContext> rookMoves = rook_moves(i, j, engineSide);
        possibleMoves.insert(possibleMoves.end(), rookMoves.begin(),
                             rookMoves.end());
        break;
      }
      case WHITE_BISHOP:
      case BLACK_BISHOP: {
        std::vector<MoveContext> bishopMoves = bishop_moves(i, j, engineSide);
        possibleMoves.insert(possibleMoves.end(), bishopMoves.begin(),
                             bishopMoves.end());
        break;
      }
      case WHITE_KNIGHT:
      case BLACK_KNIGHT: {
        std::vector<MoveContext> knightMoves = moveKnight(i, j, engineSide);
        possibleMoves.insert(possibleMoves.end(), knightMoves.begin(),
                             knightMoves.end());
        break;
      }
      case WHITE_QUEEN:
      case BLACK_QUEEN: {
        std::vector<MoveContext> queenMoves = queen_moves(i, j, engineSide);
        possibleMoves.insert(possibleMoves.end(), queenMoves.begin(),
                             queenMoves.end());
        break;
      }
      case WHITE_KING:
      case BLACK_KING: {
        std::vector<MoveContext> kingMoves = moveKing(i, j, engineSide);
        possibleMoves.insert(possibleMoves.end(), kingMoves.begin(),
                             kingMoves.end());
        break;
      }
      default:
        std::vector<MoveContext> crazyMoves = createCrazyHouse(i, j,
                                                               engineSide);
        possibleMoves.insert(possibleMoves.end(), crazyMoves.begin(),
                             crazyMoves.end());
        break;
      }
    }
  }

  std::cout << possibleMoves.size() << std::endl;

  srand(time(0));

  int r = rand() % possibleMoves.size();

  /* overwrite current game table */
  this->myCapturedPieces = possibleMoves[r].myCapturedPieces;
  this->shortCastle = possibleMoves[r].shortCastle;
  this->longCastle = possibleMoves[r].longCastle;

  for (int i = 1; i <= TABLE_SIZE; i++) {
    for (int j = 1; j <= TABLE_SIZE; j++) {
      this->gameBoard[i][j] = possibleMoves[r].currentBoard[i][j];
    }
  }

  return possibleMoves[r].actualMove;
}

std::string Bot::getBotName() { return Bot::BOT_NAME; }

MoveContext::MoveContext(Bot *bot, Move *move) {
  for (int i = 1; i <= TABLE_SIZE; i++) {
    for (int j = 1; j <= TABLE_SIZE; j++) {
      this->currentBoard[i][j] = bot->gameBoard[i][j];
    }
  }

  this->shortCastle = bot->shortCastle;
  this->longCastle = bot->longCastle;

  for (auto i : bot->myCapturedPieces) {
    this->myCapturedPieces.push_back(i);
  }

  this->actualMove = move;
}

void MoveContext::findKing(PlaySidePiece king, int &x, int &y) {
  for (int i = 1; i <= TABLE_SIZE; i++) {
    for (int j = 1; j <= TABLE_SIZE; j++) {
      if (currentBoard[i][j] == king) {
        x = i;
        y = j;
        return;
      }
    }
  }
}

bool MoveContext::checkPawnAttack(PlaySide side, PlaySidePiece pawn,
                                  int &x, int &y) {
  if (side == BLACK) {
    /* check for black pawns -- they attack on a lower x value */
    for (int i = 1; i <= TABLE_SIZE; i++) {
      for (int j = 1; j <= TABLE_SIZE; j++) {
        if (currentBoard[i][j] != pawn) {
          continue;
        }

        if (x == (i - 1) && (y == (j - 1) || y == (j + 1))) {
          return true;
        }
      }
    }
  } else {
    /* check for white pawns -- they attack on a higher x value */
    for (int i = 1; i <= TABLE_SIZE; i++) {
      for (int j = 1; j <= TABLE_SIZE; j++) {
        if (currentBoard[i][j] != pawn) {
          continue;
        }

        if (x == (i + 1) && (y == (j - 1) || y == (j + 1))) {
          return true;
        }
      }
    }
  }

  return false;
}

bool MoveContext::checkRookAttack(PlaySidePiece rook, int &x, int &y) {
  bool pieceBetween;

  for (int i = 1; i <= TABLE_SIZE; i++) {
    for (int j = 1; j <= TABLE_SIZE; j++) {
      if (currentBoard[i][j] != rook) {
        continue;
      }

      /* rook and king on the same row */
      if (x == i) {
        int start = std::min(y, j) + 1;
        int end = std::max(y, j);

        pieceBetween = false;

        for (; start < end; start++) {
          if (currentBoard[i][start] != NO_PIECE) {
            pieceBetween = true;
            break;
          }
        }

        if (!pieceBetween) {
          return true;
        }
      }

      /* rook and king on the same column */
      if (y == j) {
        int start = std::min(x, i) + 1;
        int end = std::max(x, i);

        pieceBetween = false;

        for (; start < end; start++) {
          if (currentBoard[start][j] != NO_PIECE) {
            pieceBetween = true;
            break;
          }
        }

        if (!pieceBetween) {
          return true;
        }
      }
    }
  }

  return false;
}

bool MoveContext::checkKnightAttack(PlaySidePiece knight, int &x, int &y) {
  int dx[KNIGHT_MOVES] = {2, 1, -1, -2, -2, -1, 1, 2};
  int dy[KNIGHT_MOVES] = {1, 2, 2, 1, -1, -2, -2, -1};

  for (int i = 1; i <= TABLE_SIZE; i++) {
    for (int j = 1; j <= TABLE_SIZE; j++) {
      if (currentBoard[i][j] != knight) {
        continue;
      }

      /* Knight attack implies 8 relative positions */
      for (int k = 0; k < KNIGHT_MOVES; k++) {
        if ((i + dx[k]) == x && (j + dy[k]) == y) {
          return true;
        }
      }
    }
  }

  return false;
}

bool MoveContext::checkBishopAttack(PlaySidePiece bishop, int &x, int &y) {
  for (int i = 1; i <= TABLE_SIZE; i++) {
    for (int j = 1; j <= TABLE_SIZE; j++) {
      if (currentBoard[i][j] != bishop) {
        continue;
      }

      /* check if the bishop attacks the king on 1 of the 4 diagonals */
      for (int k = 1; k < TABLE_SIZE; k++) {
        if ((i + k) <= TABLE_SIZE && (j + k) <= TABLE_SIZE) {
          if ((i + k) == x && (j + k) == y) {
            return true;
          }

          if (currentBoard[i + k][j + k] != NO_PIECE) {
            break;
          }
        } else {
          break;
        }
      }

      for (int k = 1; k < TABLE_SIZE; k++) {
        if ((i - k) > 0 && (j + k) <= TABLE_SIZE) {
          if ((i - k) == x && (j + k) == y) {
            return true;
          }

          if (currentBoard[i - k][j + k] != NO_PIECE) {
            break;
          }
        } else {
          break;
        }
      }

      for (int k = 1; k < TABLE_SIZE; k++) {
        if ((i - k) > 0 && (j - k) > 0) {
          if ((i - k) == x && (j - k) == y) {
            return true;
          }

          if (currentBoard[i - k][j - k] != NO_PIECE) {
            break;
          }
        } else {
          break;
        }
      }

      for (int k = 1; k < TABLE_SIZE; k++) {
        if ((i + k) <= TABLE_SIZE && (j - k) > 0) {
          if ((i + k) == x && (j - k) == y) {
            return true;
          }

          if (currentBoard[i + k][j - k] != NO_PIECE) {
            break;
          }
        } else {
          break;
        }
      }
    }
  }

  return false;
}

bool MoveContext::checkQueenAttack(PlaySidePiece queen, int &x, int &y) {
  /* Queen is just a rook + bishop */
  return (checkRookAttack(queen, x, y) | checkBishopAttack(queen, x, y));
}

bool MoveContext::checkKingAttack(PlaySidePiece king, int &x, int &y) {
  for (int i = 1; i <= TABLE_SIZE; i++) {
    for (int j = 1; j <= TABLE_SIZE; j++) {
      if (currentBoard[i][j] != king) {
        continue;
      }

      /* Another king needs to be in one of the
       * adjacent 9 fields on board
       */
      if (std::abs(x - i) <= 1 && std::abs(y - j) <= 1) {
        return true;
      }
    }
  }

  return false;
}

bool MoveContext::checkKingSafety(PlaySide sideToMove) {
  int x_king, y_king;

  if (sideToMove == BLACK) {
    /* check for white king's safety */
    findKing(WHITE_KING, x_king, y_king);
    return !(checkPawnAttack(sideToMove, BLACK_PAWN, x_king, y_king) |
             checkPawnAttack(sideToMove, BLACK_EN_PAS, x_king, y_king) |
             checkRookAttack(BLACK_ROOK, x_king, y_king) |
             checkKnightAttack(BLACK_KNIGHT, x_king, y_king) |
             checkBishopAttack(BLACK_BISHOP, x_king, y_king) |
             checkQueenAttack(BLACK_QUEEN, x_king, y_king) |
             checkKingAttack(BLACK_KING, x_king, y_king));
  } else {
    /* check for black king's safety */
    findKing(BLACK_KING, x_king, y_king);
    return !(checkPawnAttack(sideToMove, WHITE_PAWN, x_king, y_king) |
             checkPawnAttack(sideToMove, WHITE_EN_PAS, x_king, y_king) |
             checkRookAttack(WHITE_ROOK, x_king, y_king) |
             checkKnightAttack(WHITE_KNIGHT, x_king, y_king) |
             checkBishopAttack(WHITE_BISHOP, x_king, y_king) |
             checkQueenAttack(WHITE_QUEEN, x_king, y_king) |
             checkKingAttack(WHITE_KING, x_king, y_king));
  }

  return true;
}

void Bot::showBoard(PlaySidePiece gameBoard[TABLE_SIZE + 1][TABLE_SIZE + 1]) {
  /* that's how the chess table is usually seen */
  std::cout << "board:\n";
  for (int i = TABLE_SIZE; i > 0; i--) {
    for (int j = 1; j <= TABLE_SIZE; j++) {
      std::cout << gameBoard[i][j] << " ";
    }
    std::cout << "\n";
  }

  std::cout << "My captured Pieces: ";
  for (auto& piece : myCapturedPieces) {
    std::cout << piece << " ";
  }

  std::cout << "\n";

  std::cout << "Enemy captured Pieces: ";
  for (auto& piece : enemyCapturesPieces) {
    std::cout << piece << " ";
  }

  std::cout << "\n";
}
