#include <bits/stdc++.h>

#include "Bot.h"
#include "Move.h"
#include "Piece.h"
#include "PlaySide.h"

std::vector<MoveContext> moveCreiziChess(std::vector<PlaySidePiece> myCapturedPieces, PlaySidePiece table[TABLE_SIZE + 1][TABLE_SIZE + 1], PlaySide side) {
//   if ((side == WHITE && gameBoard[x][y] != WHITE_QUEEN) || (side == BLACK && gameBoard[x][y] != BLACK_QUEEN))
//   {
//     return std::vector<MoveContext>();
//   }
//   auto all_moves = rook_moves(x, y, side);
//   auto moves_bishop = bishop_moves(x, y, side);
//   all_moves.insert(all_moves.end(), moves_bishop.begin(), moves_bishop.end());
//   return all_moves;

    std::vector<MoveContext> possibleMoves;

    for (auto &piece : myCapturedPieces) {
        for (int i = 1; i <= TABLE_SIZE; ++i)
            for (int j = 1; j <= TABLE_SIZE; ++j) {
                if ((i == 1 || i == 8) && (piece == WHITE_PAWN || piece == BLACK_PAWN)) {
                    continue;
                }

                if (table[i][j] == NO_PIECE) {
                    switch (piece) {
                        case WHITE_PAWN:
                            std::vector<MoveContext> whitePawnMoves =  moveWhitePawn(i, j);
                            possibleMoves.insert(possibleMoves.end(), whitePawnMoves.begin(), whitePawnMoves.end());
                            break;
                        case BLACK_PAWN:
                            std::vector<MoveContext> blackPawnMoves = moveBlackPawn(i, j);
                            possibleMoves.insert(possibleMoves.end(), blackPawnMoves.begin(), blackPawnMoves.end());
                            break;
                        case WHITE_ROOK:
                            std::vector<MoveContext> whiteRookMoves =  rook_moves(i, j, side);
                            possibleMoves.insert(possibleMoves.end(), whiteRookMoves.begin(), whiteRookMoves.end());
                            break;
                        case BLACK_ROOK:
                            std::vector<MoveContext> blackRookMoves =  rook_moves(i, j, side);
                            possibleMoves.insert(possibleMoves.end(), blackRookMoves.begin(), blackRookMoves.end());
                            break;
                        case WHITE_BISHOP:
                            std::vector<MoveContext> whiteBishopMoves =  bishop_moves(i, j, side);
                            possibleMoves.insert(possibleMoves.end(), whiteBishopMoves.begin(), whiteBishopMoves.end());
                            break;
                        case BLACK_BISHOP:
                            std::vector<MoveContext> blackBishopMoves =  bishop_moves(i, j, side);
                            possibleMoves.insert(possibleMoves.end(), blackBishopMoves.begin(), blackBishopMoves.end());
                            break;
                        case WHITE_KNIGHT:
                            /* idk ce sa fac cu captured aici*/

                            std::vector<Move*> whiteKnightMoves = moveKnight(i, j, side, std::vector<PlaySidePiece>);
                            possibleMoves.insert(possibleMoves.end(), whiteKnightMoves.begin(), whiteKnightMoves.end());
                            break;
                        case BLACK_KNIGHT:
                            std::vector<Move*> blackKnightMoves = moveKnight(i, j, side, std::vector<PlaySidePiece>);
                            possibleMoves.insert(possibleMoves.end(), blackKnightMoves.begin(), blackKnightMoves.end());
                            break;
                        case WHITE_QUEEN:
                            std::vector<MoveContext> whiteQueenMoves =  queen_moves(i, j, side);
                            possibleMoves.insert(possibleMoves.end(), whiteQueenMoves.begin(), whiteQueenMoves.end());
                            break;
                        case BLACK_QUEEN:
                            std::vector<MoveContext> blackQueenMoves =  queen_moves(i, j, side);
                            possibleMoves.insert(possibleMoves.end(), blackQueenMoves.begin(), blackQueenMoves.end());
                            break;
                    }
                }
            }
    }
}