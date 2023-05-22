#ifndef BOARD_HELPERS_
#define BOARD_HELPERS_

#define TABLE_SIZE 8

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

#endif