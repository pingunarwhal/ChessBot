#include "Bot.h"

#include <bits/stdc++.h>

extern PlaySide engineSide;

const std::string Bot::BOT_NAME = "MyBot"; /* Edit this, escaped characters are forbidden */

GameConfig::GameConfig(PlaySidePiece currentConfig[TABLE_SIZE + 1][TABLE_SIZE + 1])
{
  for (int i = 1; i <= TABLE_SIZE; i++)
  {
    for (int j = 1; j <= TABLE_SIZE; j++)
    {
      this->config[i][j] = currentConfig[i][j];
    }
  }
}

Bot::Bot()
{ /* Initialize custom fields here */
  setGameBoard();
  kingCheck = NO_CHECK;
  previousKingCheck = NO_CHECK;
  shortCastle = CAN_CASTLE;
  longCastle = CAN_CASTLE;
  threeRepeatedConfigs = false;
}

void Bot::setGameBoard()
{
  for (int i = 1; i <= TABLE_SIZE; i++)
  {
    for (int j = 1; j <= TABLE_SIZE; j++)
    {
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

  for (int i = 1; i <= TABLE_SIZE; i++)
  {
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

void getCoordinates(std::string position, int &x, int &y)
{
  y = position[0] - 'a' + 1;
  x = position[1];
}

void Bot::findKing(PlaySidePiece king, int &x, int &y)
{
  for (int i = 1; i <= TABLE_SIZE; i++)
  {
    for (int j = 1; j <= TABLE_SIZE; j++)
    {
      if (gameBoard[i][j] == king)
      {
        x = i;
        y = j;
        return;
      }
    }
  }
}

bool Bot::checkPawnOnBoard(int &x, int &y)
{
  if (gameBoard[x][y] == BLACK_PAWN || gameBoard[x][y] == WHITE_PAWN ||
      gameBoard[x][y] == BLACK_EN_PAS || gameBoard[x][y] == WHITE_EN_PAS)
  {
    return true;
  }

  return false;
}

bool Bot::checkEnPassant(PlaySide sideToMove, int &x_start, int &y_start, int &x_end, int &y_end)
{
  if (sideToMove == BLACK)
  {
    /* Black en passant from 7th to 5th row */
    if (x_start == 7 && x_end == 5 && gameBoard[x_start][y_start] == BLACK_PAWN)
    {
      gameBoard[x_end][y_end] = BLACK_EN_PAS;
      return true;
    }
  }
  else
  {
    /* White en passant from 2nd to 4th row */
    if (x_start == 2 && x_end == 4 && gameBoard[x_start][y_start] == WHITE_PAWN)
    {
      gameBoard[x_end][y_end] = WHITE_EN_PAS;
      return true;
    }
  }

  return false;
}

bool Bot::checkPawnAttack(PlaySide side, PlaySidePiece pawn, int &x, int &y)
{
  if (side == BLACK)
  {
    /* check for black pawns -- they attack on a lower x value */
    for (int i = 1; i <= TABLE_SIZE; i++)
    {
      for (int j = 1; j <= TABLE_SIZE; j++)
      {
        if (gameBoard[i][j] != pawn)
        {
          continue;
        }

        if (x == (i - 1) && (y == (j - 1) || y == (j + 1)))
        {
          return true;
        }
      }
    }
  }
  else
  {
    /* check for white pawns -- they attack on a higher x value */
    for (int i = 1; i <= TABLE_SIZE; i++)
    {
      for (int j = 1; j <= TABLE_SIZE; j++)
      {
        if (gameBoard[i][j] != pawn)
        {
          continue;
        }

        if (x == (i + 1) && (y == (j - 1) || y == (j + 1)))
        {
          return true;
        }
      }
    }
  }
  return false;
}

bool Bot::checkRookAttack(PlaySidePiece rook, int &x, int &y)
{
  bool pieceBetween;

  for (int i = 1; i <= TABLE_SIZE; i++)
  {
    for (int j = 1; j <= TABLE_SIZE; j++)
    {
      if (gameBoard[i][j] != rook)
      {
        continue;
      }

      /* rook and king on the same row */
      if (x == i)
      {
        int start = std::min(y, j) + 1;
        int end = std::max(y, j);

        pieceBetween = false;

        for (; start < end; start++)
        {
          if (gameBoard[i][start] != NO_PIECE)
          {
            pieceBetween = true;
            break;
          }
        }

        if (!pieceBetween)
        {
          return true;
        }
      }

      /* rook and king on the same column */
      if (y == j)
      {
        int start = std::min(x, i) + 1;
        int end = std::max(x, i);

        pieceBetween = false;

        for (; start < end; start++)
        {
          if (gameBoard[start][j] != NO_PIECE)
          {
            pieceBetween = true;
            break;
          }
        }

        if (!pieceBetween)
        {
          return true;
        }
      }
    }
  }

  return false;
}

bool Bot::checkKnightAttack(PlaySidePiece knight, int &x, int &y)
{
  int dx[KNIGHT_MOVES] = {2, 1, -1, -2, -2, -1, 1, 2};
  int dy[KNIGHT_MOVES] = {1, 2, 2, 1, -1, -2, -2, -1};

  for (int i = 1; i <= TABLE_SIZE; i++)
  {
    for (int j = 1; j <= TABLE_SIZE; j++)
    {
      if (gameBoard[i][j] != knight)
      {
        continue;
      }

      /* Knight attack implies 8 relative positions */
      for (int k = 0; k < KNIGHT_MOVES; k++)
      {
        if ((i + dx[k]) == x && (j + dy[k]) == y)
        {
          return true;
        }
      }
    }
  }

  return false;
}

bool Bot::checkBishopAttack(PlaySidePiece bishop, int &x, int &y)
{
  for (int i = 1; i <= TABLE_SIZE; i++)
  {
    for (int j = 1; j <= TABLE_SIZE; j++)
    {
      if (gameBoard[i][j] != bishop)
      {
        continue;
      }

      /* check if the bishop attacks the king on 1 of the 4 diagonals */
      for (int k = 1; k < TABLE_SIZE; k++)
      {
        if ((i + k) <= TABLE_SIZE && (j + k) <= TABLE_SIZE)
        {
          if ((i + k) == x && (j + k) == y)
          {
            return true;
          }

          if (gameBoard[i + k][j + k] != NO_PIECE)
          {
            break;
          }
        }
        else
        {
          break;
        }
      }

      for (int k = 1; k < TABLE_SIZE; k++)
      {
        if ((i - k) > 0 && (j + k) <= TABLE_SIZE)
        {
          if ((i - k) == x && (j + k) == y)
          {
            return true;
          }

          if (gameBoard[i - k][j + k] != NO_PIECE)
          {
            break;
          }
        }
        else
        {
          break;
        }
      }

      for (int k = 1; k < TABLE_SIZE; k++)
      {
        if ((i - k) > 0 && (j - k) > 0)
        {
          if ((i - k) == x && (j - k) == y)
          {
            return true;
          }

          if (gameBoard[i - k][j - k] != NO_PIECE)
          {
            break;
          }
        }
        else
        {
          break;
        }
      }

      for (int k = 1; k < TABLE_SIZE; k++)
      {
        if ((i + k) <= TABLE_SIZE && (j - k) > 0)
        {
          if ((i + k) == x && (j - k) == y)
          {
            return true;
          }

          if (gameBoard[i + k][j - k] != NO_PIECE)
          {
            break;
          }
        }
        else
        {
          break;
        }
      }
    }
  }

  return false;
}

bool Bot::checkQueenAttack(PlaySidePiece queen, int &x, int &y)
{
  /* Queen is just a rook + bishop */
  return (checkRookAttack(queen, x, y) | checkBishopAttack(queen, x, y));
}

bool Bot::checkKingAttack(PlaySidePiece king, int &x, int &y)
{
  for (int i = 1; i <= TABLE_SIZE; i++)
  {
    for (int j = 1; j <= TABLE_SIZE; j++)
    {
      if (gameBoard[i][j] != king)
      {
        continue;
      }

      /* Another king needs to be in one of the
       * adjacent 9 fields on board
       */
      if (std::abs(x - i) <= 1 && std::abs(y - j) <= 1)
      {
        return true;
      }
    }
  }

  return false;
}

bool Bot::checkKingSafety(PlaySide sideToMove)
{
  int x_king, y_king;

  if (sideToMove == BLACK)
  {
    /* check for white king's safety */
    findKing(WHITE_KING, x_king, y_king);
    return !(checkPawnAttack(sideToMove, BLACK_PAWN, x_king, y_king) |
             checkPawnAttack(sideToMove, BLACK_EN_PAS, x_king, y_king) |
             checkRookAttack(BLACK_ROOK, x_king, y_king) |
             checkKnightAttack(BLACK_KNIGHT, x_king, y_king) |
             checkBishopAttack(BLACK_BISHOP, x_king, y_king) |
             checkQueenAttack(BLACK_QUEEN, x_king, y_king) |
             checkKingAttack(BLACK_KING, x_king, y_king));
  }
  else
  {
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

PlaySidePiece Bot::selectPiece(Piece piece, PlaySide side)
{
  switch (side)
  {
  case BLACK:
    switch (piece)
    {
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

  case WHITE:
    switch (piece)
    {
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

PlaySidePiece Bot::convertRegularPiece(int &x, int &y)
{
  /* convert a pawn from en passant marked pawn to regular pawn after one move */
  if (gameBoard[x][y] == BLACK_EN_PAS)
  {
    return BLACK_PAWN;
  }

  if (gameBoard[x][y] == WHITE_EN_PAS)
  {
    return WHITE_PAWN;
  }

  /* otherwise just return the current piece */
  return gameBoard[x][y];
}

void Bot::copyCurrentConfig()
{
  /* deep copy the current config using an auxiliary matrix */
  GameConfig currentGameBoard = GameConfig(gameBoard);

  pastConfigs.push_back(currentGameBoard);
}

bool Bot::checkRepeatedConfigs()
{
  int counterRepeatedConfigs = 0;
  bool repeatedConfig;

  for (auto &pastConfig : pastConfigs)
  {
    repeatedConfig = true;
    for (int i = 1; i <= TABLE_SIZE && repeatedConfig; i++)
    {
      for (int j = 1; j <= TABLE_SIZE && repeatedConfig; j++)
      {
        if (pastConfig.config[i][j] != gameBoard[i][j])
        {
          repeatedConfig = false;
        }
      }
    }

    if (repeatedConfig == true)
    {
      counterRepeatedConfigs++;
    }
  }

  if (counterRepeatedConfigs == 2)
  {
    return true;
  }

  return false;
}

void Bot::recordMove(Move *move, PlaySide sideToMove)
{
  /* You might find it useful to also separately
   * record last move in another custom field */
  int x_start, y_start, x_end, y_end;
  getCoordinates(move->getSource().value(), x_start, y_start);
  getCoordinates(move->getDestination().value(), x_end, y_end);

  counterMoves++;

  if (checkPawnOnBoard(x_start, y_start))
  {
    counterMoves = 0;
  }

  if (gameBoard[x_end][y_end] != NO_PIECE)
  {
    enemyCapturesPieces.push_back(convertRegularPiece(x_end, y_end));
    counterMoves = 0;
  }

  if (move->isNormal())
  {
    if (!checkEnPassant(sideToMove, x_start, y_start, x_end, y_end))
    {
      gameBoard[x_end][y_end] = convertRegularPiece(x_start, y_start);
    }

    gameBoard[x_start][y_start] = NO_PIECE;
  }

  if (move->isPromotion())
  {
    gameBoard[x_end][y_end] = selectPiece(move->getReplacement().value(), sideToMove);
    gameBoard[x_start][y_start] = NO_PIECE;
  }

  if (move->isDropIn())
  {
    gameBoard[x_end][y_end] = selectPiece(move->getReplacement().value(), sideToMove);
  }

  if (!checkKingSafety(sideToMove))
  {
    previousKingCheck = IN_CHECK;
    kingCheck = IN_CHECK;
    shortCastle = NO_CASTLE;
    longCastle = NO_CASTLE;
  }

  if (checkRepeatedConfigs())
  {
    threeRepeatedConfigs = true;
  }

  copyCurrentConfig();
}

std::string Bot::getPosition(int x, int y)
{
  std::string position = "";
  position += 'a' + y - 1;
  position += '0' + x;

  return position;
}

std::vector<MoveContext> Bot::moveWhitePawn(int x, int y)
{
  if (gameBoard[x][y] != WHITE_PAWN || gameBoard[x][y] != WHITE_EN_PAS)
  {
    return std::vector<MoveContext>();
  }

  std::vector<MoveContext> possibleMoves;

  /* become en passant */
  if (x == 2 && gameBoard[x + 2][y] == NO_PIECE)
  {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x + 2, y)));
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x + 2][y] = WHITE_EN_PAS;
    if (!newMove.checkKingSafety(BLACK)) {
      possibleMoves.push_back(newMove);
    }
  }

  /* promote pawn */
  if (x == 7 && gameBoard[x + 1][y] == NO_PIECE)
  {
    /* set current position to NO_PIECE and push back all possible promotions */
    MoveContext newMoveQ(this, Move::promote(getPosition(x, y), getPosition(x + 1, y), QUEEN));
    newMoveQ.currentBoard[x][y] = NO_PIECE;
    newMoveQ.currentBoard[x + 1][y] = WHITE_QUEEN;
    if (!newMoveQ.checkKingSafety(BLACK)) {
      possibleMoves.push_back(newMoveQ);
    }

    MoveContext newMoveR(this, Move::promote(getPosition(x, y), getPosition(x + 1, y), ROOK));
    newMoveR.currentBoard[x][y] = NO_PIECE;
    newMoveR.currentBoard[x + 1][y] = WHITE_ROOK;
    if (!newMoveR.checkKingSafety(BLACK)) {
      possibleMoves.push_back(newMoveR);
    }

    MoveContext newMoveB(this, Move::promote(getPosition(x, y), getPosition(x + 1, y), BISHOP));
    newMoveB.currentBoard[x][y] = NO_PIECE;
    newMoveB.currentBoard[x + 1][y] = WHITE_BISHOP;
    if (!newMoveB.checkKingSafety(BLACK)) {
      possibleMoves.push_back(newMoveB);
    }

    MoveContext newMoveK(this, Move::promote(getPosition(x, y), getPosition(x + 1, y), KNIGHT));
    newMoveK.currentBoard[x][y] = NO_PIECE;
    newMoveK.currentBoard[x + 1][y] = WHITE_KNIGHT;
    if (!newMoveK.checkKingSafety(BLACK)) {
      possibleMoves.push_back(newMoveK);
    }
  }

  if (gameBoard[x + 1][y] == NO_PIECE)
  {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x + 1, y)));
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x + 1][y] = WHITE_PAWN;
    if (!newMove.checkKingSafety(BLACK)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (gameBoard[x + 1][y + 1] < NO_PIECE)
  {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x + 1, y + 1)));
    newMove.myCapturedPieces.push_back(gameBoard[x + 1][y + 1]);
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x + 1][y + 1] = WHITE_PAWN;
    if (!newMove.checkKingSafety(BLACK)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (gameBoard[x + 1][y - 1] < NO_PIECE)
  {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x + 1, y - 1)));
    newMove.myCapturedPieces.push_back(gameBoard[x + 1][y - 1]);
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x + 1][y - 1] = WHITE_PAWN;
    if (!newMove.checkKingSafety(BLACK)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (gameBoard[x][y - 1] == BLACK_EN_PAS)
  {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x + 1, y - 1)));
    newMove.myCapturedPieces.push_back(gameBoard[x][y - 1]);
    newMove.currentBoard[x][y - 1] = NO_PIECE;
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x + 1][y - 1] = WHITE_PAWN;
    if (!newMove.checkKingSafety(BLACK)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (gameBoard[x][y + 1] == BLACK_EN_PAS)
  {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x + 1, y + 1)));
    newMove.myCapturedPieces.push_back(gameBoard[x][y + 1]);
    newMove.currentBoard[x][y + 1] = NO_PIECE;
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x + 1][y + 1] = WHITE_PAWN;
    if (!newMove.checkKingSafety(BLACK)) {
      possibleMoves.push_back(newMove);
    }
  }

  return possibleMoves;
}

std::vector<MoveContext> Bot::moveBlackPawn(int x, int y)
{
  if (gameBoard[x][y] != BLACK_PAWN || gameBoard[x][y] != BLACK_EN_PAS)
  {
    return std::vector<MoveContext>();
  }

  std::vector<MoveContext> possibleMoves;

  /* become en passant */
  if (x == 7 && gameBoard[x - 2][y] == NO_PIECE)
  {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x - 2, y)));
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x - 2][y] = BLACK_EN_PAS;
    if (!newMove.checkKingSafety(WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  /* promote pawn */
  if (x == 2 && gameBoard[x - 1][y] == NO_PIECE)
  {
    MoveContext newMoveQ(this, Move::promote(getPosition(x, y), getPosition(x - 1, y), QUEEN));
    newMoveQ.currentBoard[x][y] = NO_PIECE;
    newMoveQ.currentBoard[x - 1][y] = BLACK_QUEEN;
    if (!newMoveQ.checkKingSafety(WHITE)) {
      possibleMoves.push_back(newMoveQ);
    }

    MoveContext newMoveR(this, Move::promote(getPosition(x, y), getPosition(x - 1, y), ROOK));
    newMoveR.currentBoard[x][y] = NO_PIECE;
    newMoveR.currentBoard[x - 1][y] = BLACK_ROOK;
    if (!newMoveR.checkKingSafety(WHITE)) {
      possibleMoves.push_back(newMoveR);
    }

    MoveContext newMoveB(this, Move::promote(getPosition(x, y), getPosition(x - 1, y), BISHOP));
    newMoveB.currentBoard[x][y] = NO_PIECE;
    newMoveB.currentBoard[x - 1][y] = BLACK_BISHOP;
    if (!newMoveB.checkKingSafety(WHITE)) {
      possibleMoves.push_back(newMoveB);
    }

    MoveContext newMoveK(this, Move::promote(getPosition(x, y), getPosition(x - 1, y), KNIGHT));
    newMoveK.currentBoard[x][y] = NO_PIECE;
    newMoveK.currentBoard[x - 1][y] = BLACK_KNIGHT;
    if (!newMoveK.checkKingSafety(WHITE)) {
      possibleMoves.push_back(newMoveK);
    }
  }

  if (gameBoard[x - 1][y] == NO_PIECE)
  {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x - 1, y)));
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x - 1][y] = BLACK_PAWN;
    if (!newMove.checkKingSafety(WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (gameBoard[x - 1][y + 1] > NO_PIECE)
  {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x - 1, y + 1)));
    newMove.myCapturedPieces.push_back(gameBoard[x - 1][y + 1]);
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x - 1][y + 1] = BLACK_PAWN;
    if (!newMove.checkKingSafety(WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (gameBoard[x - 1][y - 1] > NO_PIECE)
  {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x - 1, y - 1)));
    newMove.myCapturedPieces.push_back(gameBoard[x - 1][y - 1]);
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x - 1][y - 1] = BLACK_PAWN;
    if (!newMove.checkKingSafety(WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (gameBoard[x][y - 1] == WHITE_EN_PAS)
  {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x - 1, y - 1)));
    newMove.myCapturedPieces.push_back(gameBoard[x][y - 1]);
    newMove.currentBoard[x][y - 1] = NO_PIECE;
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x - 1][y - 1] = BLACK_PAWN;
    if (!newMove.checkKingSafety(WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (gameBoard[x][y + 1] == WHITE_EN_PAS)
  {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x - 1, y + 1)));
    newMove.myCapturedPieces.push_back(gameBoard[x][y + 1]);
    newMove.currentBoard[x][y + 1] = NO_PIECE;
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x - 1][y + 1] = BLACK_PAWN;
    if (!newMove.checkKingSafety(WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  return possibleMoves;
}

std::vector<MoveContext> Bot::moveKnight(int x, int y, PlaySide side)
{
  if ((side == WHITE && gameBoard[x][y] != WHITE_KNIGHT) || (side == BLACK && gameBoard[x][y] != BLACK_KNIGHT))
  {
    return std::vector<MoveContext>();
  }

  std::vector<MoveContext> possibleMoves;

  if (x + 2 <= 8)
  {
    if (y + 1 <= 8)
    {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x + 2, y + 1)));

      if (gameBoard[x + 2][y + 1] != NO_PIECE)
      {
        /* a piece on the desired spot => check if it can be captured or not */
        if (side == WHITE && gameBoard[x + 2][y + 1] < NO_PIECE)
        {
          newMove.myCapturedPieces.push_back(gameBoard[x + 2][y + 1]);
          newMove.currentBoard[x][y] = NO_PIECE;
          newMove.currentBoard[x + 2][y + 1] = WHITE_KNIGHT;
          if (!newMove.checkKingSafety(side == WHITE? BLACK : WHITE)) {
            possibleMoves.push_back(newMove);
          }
        }

        if (side == BLACK && gameBoard[x + 2][y + 1] > NO_PIECE)
        {
          newMove.myCapturedPieces.push_back(gameBoard[x + 2][y + 1]);
          newMove.currentBoard[x][y] = NO_PIECE;
          newMove.currentBoard[x + 2][y + 1] = BLACK_KNIGHT;
          if (!newMove.checkKingSafety(side == WHITE? BLACK : WHITE)) {
            possibleMoves.push_back(newMove);
          }
        }
      }
      else
      {
        /* no piece on the desired spot => free to move there */
        newMove.currentBoard[x + 2][y + 1] = newMove.currentBoard[x][y];
        newMove.currentBoard[x][y] = NO_PIECE;
        if (!newMove.checkKingSafety(side == WHITE? BLACK : WHITE)) {
          possibleMoves.push_back(newMove);
        }
      }
    }

    if (y - 1 >= 1)
    {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x + 2, y - 1)));

      if (gameBoard[x + 2][y - 1] != NO_PIECE)
      {
        /* a piece on the desired spot => check if it can be captured or not */
        if (side == WHITE && gameBoard[x + 2][y - 1] < NO_PIECE)
        {
          newMove.myCapturedPieces.push_back(gameBoard[x + 2][y - 1]);
          newMove.currentBoard[x][y] = NO_PIECE;
          newMove.currentBoard[x + 2][y - 1] = WHITE_KNIGHT;
          if (!newMove.checkKingSafety(side == WHITE? BLACK : WHITE)) {
            possibleMoves.push_back(newMove);
          }
        }

        if (side == BLACK && gameBoard[x + 2][y - 1] > NO_PIECE)
        {
          newMove.myCapturedPieces.push_back(gameBoard[x + 2][y - 1]);
          newMove.currentBoard[x][y] = NO_PIECE;
          newMove.currentBoard[x + 2][y - 1] = BLACK_KNIGHT;
          if (!newMove.checkKingSafety(side == WHITE? BLACK : WHITE)) {
            possibleMoves.push_back(newMove);
          }
        }
      }
      else
      {
        /* no piece on the desired spot => free to move there */
        newMove.currentBoard[x + 2][y - 1] = newMove.currentBoard[x][y];
        newMove.currentBoard[x][y] = NO_PIECE;
        if (!newMove.checkKingSafety(side == WHITE? BLACK : WHITE)) {
          possibleMoves.push_back(newMove);
        }
      }
    }
  }

  if (x - 2 >= 1)
  {
    if (y + 1 <= 8)
    {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x - 2, y + 1)));

      if (gameBoard[x - 2][y + 1] != NO_PIECE)
      {
        /* a piece on the desired spot => check if it can be captured or not */
        if (side == WHITE && gameBoard[x - 2][y + 1] < NO_PIECE)
        {
          newMove.myCapturedPieces.push_back(gameBoard[x - 2][y + 1]);
          newMove.currentBoard[x][y] = NO_PIECE;
          newMove.currentBoard[x - 2][y + 1] = WHITE_KNIGHT;
          if (!newMove.checkKingSafety(side == WHITE? BLACK : WHITE)) {
            possibleMoves.push_back(newMove);
          }
        }

        if (side == BLACK && gameBoard[x - 2][y + 1] > NO_PIECE)
        {
          newMove.myCapturedPieces.push_back(gameBoard[x - 2][y + 1]);
          newMove.currentBoard[x][y] = NO_PIECE;
          newMove.currentBoard[x - 2][y + 1] = BLACK_KNIGHT;
          if (!newMove.checkKingSafety(side == WHITE? BLACK : WHITE)) {
            possibleMoves.push_back(newMove);
          }
        }
      }
      else
      {
        /* no piece on the desired spot => free to move there */
        newMove.currentBoard[x - 2][y + 1] = newMove.currentBoard[x][y];
        newMove.currentBoard[x][y] = NO_PIECE;
        if (!newMove.checkKingSafety(side == WHITE? BLACK : WHITE)) {
          possibleMoves.push_back(newMove);
        }
      }
    }

    if (y - 1 >= 1)
    {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x - 2, y - 1)));

      if (gameBoard[x - 2][y - 1] != NO_PIECE)
      {
        /* a piece on the desired spot => check if it can be captured or not */
        if (side == WHITE && gameBoard[x - 2][y - 1] < NO_PIECE)
        {
          newMove.myCapturedPieces.push_back(gameBoard[x - 2][y - 1]);
          newMove.currentBoard[x][y] = NO_PIECE;
          newMove.currentBoard[x - 2][y - 1] = WHITE_KNIGHT;
          if (!newMove.checkKingSafety(side == WHITE? BLACK : WHITE)) {
            possibleMoves.push_back(newMove);
          }
        }

        if (side == BLACK && gameBoard[x - 2][y - 1] > NO_PIECE)
        {
          newMove.myCapturedPieces.push_back(gameBoard[x - 2][y - 1]);
          newMove.currentBoard[x][y] = NO_PIECE;
          newMove.currentBoard[x - 2][y - 1] = BLACK_KNIGHT;
          if (!newMove.checkKingSafety(side == WHITE? BLACK : WHITE)) {
            possibleMoves.push_back(newMove);
          }
        }
      }
      else
      {
        /* no piece on the desired spot => free to move there */
        newMove.currentBoard[x - 2][y - 1] = newMove.currentBoard[x][y];
        newMove.currentBoard[x][y] = NO_PIECE;
        if (!newMove.checkKingSafety(side == WHITE? BLACK : WHITE)) {
          possibleMoves.push_back(newMove);
        }
      }
    }
  }

  return possibleMoves;
}

std::vector<MoveContext> Bot::moveKing(int x, int y, PlaySide side)
{
  if ((side == WHITE && gameBoard[x][y] != WHITE_KING) || (side == BLACK && gameBoard[x][y] != BLACK_KING))
  {
    return std::vector<MoveContext>();
  }

  std::vector<MoveContext> possibleMoves;

  if (x + 1 <= 8)
  {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x + 1, y)));

    if (gameBoard[x + 1][y] != NO_PIECE)
    {
      if (side == WHITE && gameBoard[x + 1][y] < NO_PIECE)
      {
        newMove.myCapturedPieces.push_back(gameBoard[x + 1][y]);
      }

      if (side == BLACK && gameBoard[x + 1][y] > NO_PIECE)
      {
        newMove.myCapturedPieces.push_back(gameBoard[x + 1][y]);
      }
    }

    newMove.currentBoard[x + 1][y] = newMove.currentBoard[x][y];
    newMove.currentBoard[x][y] = NO_PIECE;
    if (!newMove.checkKingSafety(side == WHITE? BLACK : WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (x - 1 >= 1)
  {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x - 1, y)));

    if (gameBoard[x - 1][y] != NO_PIECE)
    {
      if (side == WHITE && gameBoard[x - 1][y] < NO_PIECE)
      {
        newMove.myCapturedPieces.push_back(gameBoard[x - 1][y]);
      }

      if (side == BLACK && gameBoard[x - 1][y] > NO_PIECE)
      {
        newMove.myCapturedPieces.push_back(gameBoard[x - 1][y]);
      }
    }

    newMove.currentBoard[x - 1][y] = newMove.currentBoard[x][y];
    newMove.currentBoard[x][y] = NO_PIECE;
    if (!newMove.checkKingSafety(side == WHITE? BLACK : WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (y + 1 <= 8)
  {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x, y + 1)));

    if (gameBoard[x][y + 1] != NO_PIECE)
    {
      if (side == WHITE && gameBoard[x][y + 1] < NO_PIECE)
      {
        newMove.myCapturedPieces.push_back(gameBoard[x][y + 1]);
      }

      if (side == BLACK && gameBoard[x][y + 1] > NO_PIECE)
      {
        newMove.myCapturedPieces.push_back(gameBoard[x][y + 1]);
      }
    }

    newMove.currentBoard[x][y + 1] = newMove.currentBoard[x][y];
    newMove.currentBoard[x][y] = NO_PIECE;
    if (!newMove.checkKingSafety(side == WHITE? BLACK : WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (y - 1 <= 8)
  {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x, y - 1)));

    if (gameBoard[x][y - 1] != NO_PIECE)
    {
      if (side == WHITE && gameBoard[x][y - 1] < NO_PIECE)
      {
        newMove.myCapturedPieces.push_back(gameBoard[x][y - 1]);
      }

      if (side == BLACK && gameBoard[x][y - 1] > NO_PIECE)
      {
        newMove.myCapturedPieces.push_back(gameBoard[x][y - 1]);
      }
    }

    newMove.currentBoard[x][y - 1] = newMove.currentBoard[x][y];
    newMove.currentBoard[x][y] = NO_PIECE;
    if (!newMove.checkKingSafety(side == WHITE? BLACK : WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (x + 1 <= 8 && y + 1 <= 8)
  {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x + 1, y + 1)));

    if (gameBoard[x + 1][y + 1] != NO_PIECE)
    {
      if (side == WHITE && gameBoard[x + 1][y + 1] < NO_PIECE)
      {
        newMove.myCapturedPieces.push_back(gameBoard[x + 1][y + 1]);
      }

      if (side == BLACK && gameBoard[x + 1][y + 1] > NO_PIECE)
      {
        newMove.myCapturedPieces.push_back(gameBoard[x + 1][y + 1]);
      }
    }

    newMove.currentBoard[x + 1][y + 1] = newMove.currentBoard[x][y];
    newMove.currentBoard[x][y] = NO_PIECE;
    if (!newMove.checkKingSafety(side == WHITE? BLACK : WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (x + 1 <= 8 && y - 1 >= 1)
  {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x + 1, y - 1)));

    if (gameBoard[x + 1][y - 1] != NO_PIECE)
    {
      if (side == WHITE && gameBoard[x + 1][y - 1] < NO_PIECE)
      {
        newMove.myCapturedPieces.push_back(gameBoard[x + 1][y - 1]);
      }

      if (side == BLACK && gameBoard[x + 1][y - 1] > NO_PIECE)
      {
        newMove.myCapturedPieces.push_back(gameBoard[x + 1][y - 1]);
      }
    }

    newMove.currentBoard[x + 1][y - 1] = newMove.currentBoard[x][y];
    newMove.currentBoard[x][y] = NO_PIECE;
    if (!newMove.checkKingSafety(side == WHITE? BLACK : WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (x - 1 >= 1 && y + 1 <= 8)
  {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x - 1, y + 1)));

    if (gameBoard[x - 1][y + 1] != NO_PIECE)
    {
      if (side == WHITE && gameBoard[x - 1][y + 1] < NO_PIECE)
      {
        newMove.myCapturedPieces.push_back(gameBoard[x - 1][y + 1]);
      }

      if (side == BLACK && gameBoard[x - 1][y + 1] > NO_PIECE)
      {
        newMove.myCapturedPieces.push_back(gameBoard[x - 1][y + 1]);
      }
    }

    newMove.currentBoard[x - 1][y + 1] = newMove.currentBoard[x][y];
    newMove.currentBoard[x][y] = NO_PIECE;
    if (!newMove.checkKingSafety(side == WHITE? BLACK : WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (x - 1 >= 1 && y - 1 >= 1)
  {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x - 1, y - 1)));

    if (gameBoard[x - 1][y - 1] != NO_PIECE)
    {
      if (side == WHITE && gameBoard[x - 1][y - 1] < NO_PIECE)
      {
        newMove.myCapturedPieces.push_back(gameBoard[x - 1][y - 1]);
      }

      if (side == BLACK && gameBoard[x - 1][y - 1] > NO_PIECE)
      {
        newMove.myCapturedPieces.push_back(gameBoard[x - 1][y - 1]);
      }
    }

    newMove.currentBoard[x - 1][y - 1] = newMove.currentBoard[x][y];
    newMove.currentBoard[x][y] = NO_PIECE;
    if (!newMove.checkKingSafety(side == WHITE? BLACK : WHITE)) {
      possibleMoves.push_back(newMove);
    }
  }

  if (shortCastle == CAN_CASTLE)
  {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x, y + 2)));
    newMove.currentBoard[x][y + 2] = newMove.currentBoard[x][y];
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x][y - 1] = newMove.currentBoard[x][TABLE_SIZE];
    newMove.currentBoard[x][TABLE_SIZE] = NO_PIECE;
  }

  if (longCastle == CAN_CASTLE)
  {
    MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x, y - 2)));
    newMove.currentBoard[x][y - 2] = newMove.currentBoard[x][y];
    newMove.currentBoard[x][y] = NO_PIECE;
    newMove.currentBoard[x][y + 1] = newMove.currentBoard[x][1];
    newMove.currentBoard[x][1] = NO_PIECE;
  }

  return possibleMoves;
}

std::vector<MoveContext> Bot::rook_moves(int x, int y, PlaySide side)
{
  if ((side == WHITE && gameBoard[x][y] != WHITE_ROOK) || (side == BLACK && gameBoard[x][y] != BLACK_ROOK))
  {
    return std::vector<MoveContext>();
  }
  std::vector<MoveContext> moves;
  bool turn = false;
  auto valid_move = turn ? [](char c)
  { return c < 0; }
                         : [](char c)
  { return c > 0; };
  for (int i = x + 1; i < TABLE_SIZE; ++i)
  { // right
    if (gameBoard[i][y] != NO_PIECE && valid_move(gameBoard[i][y]))
    {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(i, y)));
      newMove.myCapturedPieces.push_back(gameBoard[i][y]);
      moves.push_back(newMove);
      break;
    }
    else if (valid_move(gameBoard[i][y]))
    {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(i, y)));
      moves.push_back(newMove);
    }
  }

  for (int i = x - 1; i >= 0; i--)
  { // left
    if (gameBoard[i][y] != NO_PIECE && valid_move(gameBoard[i][y]))
    {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(i, y)));
      newMove.myCapturedPieces.push_back(gameBoard[i][y]);
      moves.push_back(newMove);
      break;
    }
    else if (valid_move(gameBoard[i][y]))
    {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(i, y)));
      moves.push_back(newMove);
    }
  }

  for (int i = y - 1; i >= 0; i--)
  { // down
    if (gameBoard[x][i] != NO_PIECE && valid_move(gameBoard[x][i]))
    {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x, i)));
      newMove.myCapturedPieces.push_back(gameBoard[x][i]);
      moves.push_back(newMove);
      break;
    }
    else if (valid_move(gameBoard[x][i]))
    {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x, i)));
      moves.push_back(newMove);
    }
  }

  for (int i = y + 1; y < TABLE_SIZE; i++)
  { // up
    if (gameBoard[x][i] != NO_PIECE && valid_move(gameBoard[x][i]))
    {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x, i)));
      newMove.myCapturedPieces.push_back(gameBoard[x][i]);
      moves.push_back(newMove);
      break;
    }
    else if (valid_move(gameBoard[x][i]))
    {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(x, i)));
      moves.push_back(newMove);
    }
  }
  return moves;
}

std::vector<MoveContext> Bot::bishop_moves(int x, int y, PlaySide side)
{
  if ((side == WHITE && gameBoard[x][y] != WHITE_BISHOP) || (side == BLACK && gameBoard[x][y] != BLACK_BISHOP))
  {
    return std::vector<MoveContext>();
  }
  std::vector<MoveContext> moves;
  bool turn = false;
  auto valid_move = turn ? [](char c)
  { return c < 0; }
                         : [](char c)
  { return c > 0; };
  for (int i = x + 1, j = y + 1; i < TABLE_SIZE && j < TABLE_SIZE; i++, j++)
  { // up right
    if (gameBoard[i][j] != NO_PIECE && valid_move(gameBoard[i][j]))
    {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(i, j)));
      newMove.myCapturedPieces.push_back(gameBoard[i][j]);
      moves.push_back(newMove);
      break;
    }
    else if (valid_move(gameBoard[i][j]))
    {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(i, j)));
      moves.push_back(newMove);
    }
  }

  for (int i = x - 1, j = y + 1; i >= 0 && j < TABLE_SIZE; i--, j++)
  { // up left
    if (gameBoard[i][j] != NO_PIECE && valid_move(gameBoard[i][j]))
    {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(i, j)));
      newMove.myCapturedPieces.push_back(gameBoard[i][j]);
      moves.push_back(newMove);
      break;
    }
    else if (valid_move(gameBoard[i][j]))
    {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(i, j)));
      moves.push_back(newMove);
    }
  }

  for (int i = x + 1, j = y - 1; i < TABLE_SIZE && j >= 0; i++, j--)
  { // down right
    if (gameBoard[i][j] != NO_PIECE && valid_move(gameBoard[i][j]))
    {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(i, j)));
      newMove.myCapturedPieces.push_back(gameBoard[i][j]);
      moves.push_back(newMove);
      break;
    }
    else if (valid_move(gameBoard[i][j]))
    {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(i, j)));
      moves.push_back(newMove);
    }
  }

  for (int i = x - 1, j = y - 1; i >= 0 && j >= 0; i--, j--)
  { // down left
    if (gameBoard[i][j] != NO_PIECE && valid_move(gameBoard[i][j]))
    {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(i, j)));
      newMove.myCapturedPieces.push_back(gameBoard[i][j]);
      moves.push_back(newMove);
      break;
    }
    else if (valid_move(gameBoard[i][j]))
    {
      MoveContext newMove(this, Move::moveTo(getPosition(x, y), getPosition(i, j)));
      moves.push_back(newMove);
    }
  }
  return moves;
}

std::vector<MoveContext> Bot::queen_moves(int x, int y, PlaySide side)
{
  if ((side == WHITE && gameBoard[x][y] != WHITE_QUEEN) || (side == BLACK && gameBoard[x][y] != BLACK_QUEEN))
  {
    return std::vector<MoveContext>();
  }
  auto all_moves = rook_moves(x, y, side);
  auto moves_bishop = bishop_moves(x, y, side);
  all_moves.insert(all_moves.end(), moves_bishop.begin(), moves_bishop.end());
  return all_moves;
}

std::vector<MoveContext> Bot::createCrazyHouse()
{
  std::vector<MoveContext> possibleMoves;

  for (auto & piece: myCapturedPieces) {
    for (int i = 1; i <= TABLE_SIZE; ++i)
      for (int j = 1; j <= TABLE_SIZE; ++j) {
        if ((i == 1 || i == 8) && (piece == WHITE_PAWN || piece == BLACK_PAWN)) {
          continue;
        }

        if (gameBoard[i][j] == NO_PIECE) {
          gameBoard[i][j] = piece;

          // Most likely it is not legal to simulate introducing a CrazyHouse piece by movien it
          // from (i, j) to (i, j)
          MoveContext newMove(this, Move::moveTo(getPosition(i, j), getPosition(i, j)));
          possibleMoves.push_back(newMove);

          // Set the board back to the state before introducing the fictive piece
          gameBoard[i][j] = NO_PIECE;
        }
      }
  }

  return possibleMoves;
}

Move *Bot::calculateNextMove()
{
  /* Play move for the side the engine is playing (Hint: Main.getEngineSide())
   * Make sure to record your move in custom structures before returning.
   *
   * Return move that you are willing to submit
   * Move is to be constructed via one of the factory methods declared in Move.h */

  std::vector<MoveContext> possibleMoves;

  for (int i = 1; i <= TABLE_SIZE; i++)
  {
    for (int j = 1; j <= TABLE_SIZE; j++)
    {
      switch (gameBoard[i][j])
      {
      case WHITE_PAWN:
      case WHITE_EN_PAS:
      {
        std::vector<MoveContext> whitePawnMoves = moveWhitePawn(i, j);
        possibleMoves.insert(possibleMoves.end(), whitePawnMoves.begin(), whitePawnMoves.end());
        break;
      }
      case BLACK_PAWN:
      case BLACK_EN_PAS:
      {
        std::vector<MoveContext> blackPawnMoves = moveBlackPawn(i, j);
        possibleMoves.insert(possibleMoves.end(), blackPawnMoves.begin(), blackPawnMoves.end());
        break;
      }
      case WHITE_ROOK:
      case BLACK_ROOK:
      {
        std::vector<MoveContext> rookMoves = rook_moves(i, j, engineSide);
        possibleMoves.insert(possibleMoves.end(), rookMoves.begin(), rookMoves.end());
        break;
      }
      case WHITE_BISHOP:
      case BLACK_BISHOP:
      {
        std::vector<MoveContext> bishopMoves = bishop_moves(i, j, engineSide);
        possibleMoves.insert(possibleMoves.end(), bishopMoves.begin(), bishopMoves.end());
        break;
      }
      case WHITE_KNIGHT:
      case BLACK_KNIGHT:
      {
        std::vector<MoveContext> knightMoves = moveKnight(i, j, engineSide);
        possibleMoves.insert(possibleMoves.end(), knightMoves.begin(), knightMoves.end());
        break;
      }
      case WHITE_QUEEN:
      case BLACK_QUEEN:
      {
        std::vector<MoveContext> queenMoves = queen_moves(i, j, engineSide);
        possibleMoves.insert(possibleMoves.end(), queenMoves.begin(), queenMoves.end());
        break;
      }
      case WHITE_KING:
      case BLACK_KING:
      {
        std::vector<MoveContext> kingMoves = moveKing(i, j, engineSide);
        possibleMoves.insert(possibleMoves.end(), kingMoves.begin(), kingMoves.end());
        break;
      }
      default:
        std::vector<MoveContext> crazyMoves = createCrazyHouse();
        possibleMoves.insert(possibleMoves.end(), crazyMoves.begin(), crazyMoves.end());
        break;
      }
    }
  }

  int r = rand() % possibleMoves.size();

  /* overwrite current game table */
  this->myCapturedPieces = possibleMoves[r].myCapturedPieces;
  this->shortCastle = possibleMoves[r].shortCastle;
  this->longCastle = possibleMoves[r].longCastle;
  for (int i = 1; i <= TABLE_SIZE; i++)
  {
    for (int j = 1; j <= TABLE_SIZE; j++)
    {
      this->gameBoard[i][j] = possibleMoves[r].currentBoard[i][j];
    }
  }

  return possibleMoves[r].actualMove;
}

std::string Bot::getBotName() { return Bot::BOT_NAME; }

MoveContext::MoveContext(Bot *bot, Move *move)
{
  for (int i = 1; i <= TABLE_SIZE; i++)
  {
    for (int j = 1; j <= TABLE_SIZE; j++)
    {
      this->currentBoard[i][j] = bot->gameBoard[i][j];
    }
  }

  this->shortCastle = bot->shortCastle;
  this->longCastle = bot->longCastle;

  for (auto i : bot->myCapturedPieces)
  {
    this->myCapturedPieces.push_back(i);
  }

  this->actualMove = move;
}

void MoveContext::findKing(PlaySidePiece king, int &x, int &y)
{
  for (int i = 1; i <= TABLE_SIZE; i++)
  {
    for (int j = 1; j <= TABLE_SIZE; j++)
    {
      if (currentBoard[i][j] == king)
      {
        x = i;
        y = j;
        return;
      }
    }
  }
}

bool MoveContext::checkPawnAttack(PlaySide side, PlaySidePiece pawn, int &x, int &y)
{
  if (side == BLACK)
  {
    /* check for black pawns -- they attack on a lower x value */
    for (int i = 1; i <= TABLE_SIZE; i++)
    {
      for (int j = 1; j <= TABLE_SIZE; j++)
      {
        if (currentBoard[i][j] != pawn)
        {
          continue;
        }

        if (x == (i - 1) && (y == (j - 1) || y == (j + 1)))
        {
          return true;
        }
      }
    }
  }
  else
  {
    /* check for white pawns -- they attack on a higher x value */
    for (int i = 1; i <= TABLE_SIZE; i++)
    {
      for (int j = 1; j <= TABLE_SIZE; j++)
      {
        if (currentBoard[i][j] != pawn)
        {
          continue;
        }

        if (x == (i + 1) && (y == (j - 1) || y == (j + 1)))
        {
          return true;
        }
      }
    }
  }
  return false;
}

bool MoveContext::checkRookAttack(PlaySidePiece rook, int &x, int &y)
{
  bool pieceBetween;

  for (int i = 1; i <= TABLE_SIZE; i++)
  {
    for (int j = 1; j <= TABLE_SIZE; j++)
    {
      if (currentBoard[i][j] != rook)
      {
        continue;
      }

      /* rook and king on the same row */
      if (x == i)
      {
        int start = std::min(y, j) + 1;
        int end = std::max(y, j);

        pieceBetween = false;

        for (; start < end; start++)
        {
          if (currentBoard[i][start] != NO_PIECE)
          {
            pieceBetween = true;
            break;
          }
        }

        if (!pieceBetween)
        {
          return true;
        }
      }

      /* rook and king on the same column */
      if (y == j)
      {
        int start = std::min(x, i) + 1;
        int end = std::max(x, i);

        pieceBetween = false;

        for (; start < end; start++)
        {
          if (currentBoard[start][j] != NO_PIECE)
          {
            pieceBetween = true;
            break;
          }
        }

        if (!pieceBetween)
        {
          return true;
        }
      }
    }
  }

  return false;
}

bool MoveContext::checkKnightAttack(PlaySidePiece knight, int &x, int &y)
{
  int dx[KNIGHT_MOVES] = {2, 1, -1, -2, -2, -1, 1, 2};
  int dy[KNIGHT_MOVES] = {1, 2, 2, 1, -1, -2, -2, -1};

  for (int i = 1; i <= TABLE_SIZE; i++)
  {
    for (int j = 1; j <= TABLE_SIZE; j++)
    {
      if (currentBoard[i][j] != knight)
      {
        continue;
      }

      /* Knight attack implies 8 relative positions */
      for (int k = 0; k < KNIGHT_MOVES; k++)
      {
        if ((i + dx[k]) == x && (j + dy[k]) == y)
        {
          return true;
        }
      }
    }
  }

  return false;
}

bool MoveContext::checkBishopAttack(PlaySidePiece bishop, int &x, int &y)
{
  for (int i = 1; i <= TABLE_SIZE; i++)
  {
    for (int j = 1; j <= TABLE_SIZE; j++)
    {
      if (currentBoard[i][j] != bishop)
      {
        continue;
      }

      /* check if the bishop attacks the king on 1 of the 4 diagonals */
      for (int k = 1; k < TABLE_SIZE; k++)
      {
        if ((i + k) <= TABLE_SIZE && (j + k) <= TABLE_SIZE)
        {
          if ((i + k) == x && (j + k) == y)
          {
            return true;
          }

          if (currentBoard[i + k][j + k] != NO_PIECE)
          {
            break;
          }
        }
        else
        {
          break;
        }
      }

      for (int k = 1; k < TABLE_SIZE; k++)
      {
        if ((i - k) > 0 && (j + k) <= TABLE_SIZE)
        {
          if ((i - k) == x && (j + k) == y)
          {
            return true;
          }

          if (currentBoard[i - k][j + k] != NO_PIECE)
          {
            break;
          }
        }
        else
        {
          break;
        }
      }

      for (int k = 1; k < TABLE_SIZE; k++)
      {
        if ((i - k) > 0 && (j - k) > 0)
        {
          if ((i - k) == x && (j - k) == y)
          {
            return true;
          }

          if (currentBoard[i - k][j - k] != NO_PIECE)
          {
            break;
          }
        }
        else
        {
          break;
        }
      }

      for (int k = 1; k < TABLE_SIZE; k++)
      {
        if ((i + k) <= TABLE_SIZE && (j - k) > 0)
        {
          if ((i + k) == x && (j - k) == y)
          {
            return true;
          }

          if (currentBoard[i + k][j - k] != NO_PIECE)
          {
            break;
          }
        }
        else
        {
          break;
        }
      }
    }
  }

  return false;
}

bool MoveContext::checkQueenAttack(PlaySidePiece queen, int &x, int &y)
{
  /* Queen is just a rook + bishop */
  return (checkRookAttack(queen, x, y) | checkBishopAttack(queen, x, y));
}

bool MoveContext::checkKingAttack(PlaySidePiece king, int &x, int &y)
{
  for (int i = 1; i <= TABLE_SIZE; i++)
  {
    for (int j = 1; j <= TABLE_SIZE; j++)
    {
      if (currentBoard[i][j] != king)
      {
        continue;
      }

      /* Another king needs to be in one of the
       * adjacent 9 fields on board
       */
      if (std::abs(x - i) <= 1 && std::abs(y - j) <= 1)
      {
        return true;
      }
    }
  }

  return false;
}

bool MoveContext::checkKingSafety(PlaySide sideToMove)
{
  int x_king, y_king;

  if (sideToMove == BLACK)
  {
    /* check for white king's safety */
    findKing(WHITE_KING, x_king, y_king);
    return !(checkPawnAttack(sideToMove, BLACK_PAWN, x_king, y_king) |
             checkPawnAttack(sideToMove, BLACK_EN_PAS, x_king, y_king) |
             checkRookAttack(BLACK_ROOK, x_king, y_king) |
             checkKnightAttack(BLACK_KNIGHT, x_king, y_king) |
             checkBishopAttack(BLACK_BISHOP, x_king, y_king) |
             checkQueenAttack(BLACK_QUEEN, x_king, y_king) |
             checkKingAttack(BLACK_KING, x_king, y_king));
  }
  else
  {
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
