#ifndef MOVENODE_H
#define MOVENODE_H

#include <bits/stdc++.h>

#include "Move.h"
#include "PlaySide.h"

#define TABLE_SIZE 8
#define KNIGHT_MOVES 8
#define KING_MOVES 9

enum PlaySidePiece {
  WHITE_PAWN = 1, WHITE_ROOK = 2,
  WHITE_KNIGHT = 3, WHITE_BISHOP = 4,
  WHITE_QUEEN = 5, WHITE_KING = 6,
  WHITE_EN_PAS = 11, P_WHITE_ROOK = 12,
  P_WHITE_KNIGHT = 13, P_WHITE_BISHOP = 14, 
  P_WHITE_QUEEN = 15,
  BLACK_PAWN = -1, BLACK_ROOK = -2,
  BLACK_KNIGHT = -3, BLACK_BISHOP = -4,
  BLACK_QUEEN = -5, BLACK_KING = -6,
  BLACK_EN_PAS = -11, P_BLACK_ROOK = -12,
  P_BLACK_KNIGHT = -13, P_BLACK_BISHOP = -14,
  P_BLACK_QUEEN = -15,
  NO_PIECE = 0
};

class BoardConfig {
	public:

	PlaySidePiece config[TABLE_SIZE + 1][TABLE_SIZE + 1];

	/* copy constructor for board configuration */
	BoardConfig(PlaySidePiece currentConfig[TABLE_SIZE + 1][TABLE_SIZE + 1]);
};

class MoveNode {
	public:
		PlaySidePiece currentBoard[TABLE_SIZE + 1][TABLE_SIZE + 1];
		
		bool shortCastle;
		bool longCastle;
		bool kingInCheck;
  		bool previousKingInCheck;
		int counterMoves;
		
		std::vector<PlaySidePiece> myCapturedPieces;
		std::vector<PlaySidePiece> enemyCapturedPieces;

		Move *move;

		int score;

		std::vector<MoveNode> possibleMoves;

		MoveNode(MoveNode *moveNode, Move *move);
		MoveNode(MoveNode *moveNode);
		MoveNode();

		/* set the initial board of the game */
		void setInitialBoard();

		/* change the color of a piece when it was taken by the opponent */
		PlaySidePiece switchSide(int x, int y);

		/* find the king on the current board */
		void findKing(PlaySidePiece king, int &x, int &y);

		/* check if a pawn attacks the king on the current board */
		bool checkPawnAttack(PlaySide side, PlaySidePiece pawn, int &x, int &y);

		/* check if a rook attacks the king on the current board */
		bool checkRookAttack(PlaySidePiece rook, int &x, int &y);

		/* check if a knight attacks the king on the current board */
		bool checkKnightAttack(PlaySidePiece knight, int &x, int &y);

		/* check if a bishop attacks the king on the current board */
		bool checkBishopAttack(PlaySidePiece bishop, int &x, int &y);

		/* check if a queen attacks the king on the current board */
		bool checkQueenAttack(PlaySidePiece queen, int &x, int &y);

		/* check if the other king attacks the king on the current board */
		bool checkKingAttack(PlaySidePiece king, int &x, int &y);

		/* check king safety, summing up all the attacks above */
		bool checkKingSafety(PlaySide sideToMove);

		bool checkPawnOnBoard(int &x, int &y);

		bool checkSpecialCases(PlaySide sideToMove, int &x_start, int &y_start, int &x_end, int &y_end);

		PlaySidePiece revertEnPassant(int &x, int &y);

		void removeCapturedPiece(PlaySidePiece piece);

		void updateBoard(Move* move, PlaySide side);

		/* determine all possible moves for white pawns */
		void whitePawnMoves(int x, int y, PlaySide side);

		/* determine all possible moves for black pawns */
		void blackPawnMoves(int x, int y, PlaySide side);

		/* determine all possible moves for rook */
		void rookMoves(int x, int y, PlaySide side);

		/* determine all possible moves for knight */
		void knightMoves(int x, int y, PlaySide side);

		/* determine all possible moves for bishop */
		void bishopMoves(int x, int y, PlaySide side);

		/* determine all possible moves for queen */
		void queenMoves(int x, int y, PlaySide side);

		/* determine all possible moves for king */
		void kingMoves(int x, int y, PlaySide side);

		/* determine all possible Crazy House moves */
		void crazyHouseMoves(int x, int y, PlaySide side);

		void calculateAllNextMoves(PlaySide engineSide);
};

std::string stringPosition(int x, int y);
void coordinatesPosition(std::string position, int &x, int &);

Piece convertPlaySidePiece(PlaySidePiece piece);
PlaySidePiece convertRegularPiece(Piece piece, PlaySide side, bool type);

#endif 