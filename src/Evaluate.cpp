#include "Evaluate.h"

#include <bits/stdc++.h>

bool check_file_isolated(BoardConfig config, int fileToCheck, PlaySide side) {
	if (side == WHITE) {
		for (int i = 1; i <= 8; i++) {
			if (config.config[i][fileToCheck] == WHITE_EN_PAS || config.config[i][fileToCheck] == WHITE_PAWN) {
				return false;
			}
		}
	} else {
		for (int i = 1; i <= 8; i++) {
			if (config.config[i][fileToCheck] == BLACK_EN_PAS || config.config[i][fileToCheck] == BLACK_PAWN) {
				return false;
			}
		}
	}

	return true;
}

double evaluate_basic(BoardConfig config, bool canCastle, int possible_moves, PlaySide side) {
	double score = 0;

	for (int i = 1; i <= 8; i++) {
		for (int j = 1; j <= 8; j++) {
			if (config.config[i][j] != 0) {
				switch (config.config[i][j])
				{
				case WHITE_PAWN:
				case WHITE_EN_PAS:
					score += PAWN_S;

					bool isolatedRight, isolatedLeft;
					if (j - 1 >= 1) {
						isolatedLeft = check_file_isolated(config, j - 1, WHITE);
					}
					if (j + 1 <= 8) {
						isolatedRight = check_file_isolated(config, j + 1, WHITE);
					}

					if (isolatedLeft && isolatedRight) {
						score -= ISOLATED_PAWNS_S;
					}
					break;
				case BLACK_PAWN:
				case BLACK_EN_PAS:
					score -= PAWN_S;
					break;
				case WHITE_ROOK:
					score += ROOK_S;
					break;
				case BLACK_ROOK:
					score -= ROOK_S;
					break;
				case WHITE_KNIGHT:
					score += KNIGHT_S;
					break;
				case BLACK_KNIGHT:
					score -= KNIGHT_S;
					break;
				case WHITE_BISHOP:
					score += BISHOP_S;
					break;
				case BLACK_BISHOP:
					score -= BISHOP_S;
					break;
				case WHITE_QUEEN:
					score += QUEEN_S;
					break;
				case BLACK_QUEEN:
					score -= QUEEN_S;
					break;
				case WHITE_KING:
					score += KING_S;
					break;
				case BLACK_KING:
					score -= KING_S;
					break;
				case P_WHITE_QUEEN:
					score += P_QUEEN_S;
					break;
				case P_BLACK_QUEEN:
					score -= P_QUEEN_S;
					break;
				case P_WHITE_KNIGHT:
					score += P_KNIGHT_S;
					break;
				case P_BLACK_KNIGHT:
					score -= P_KNIGHT_S;
					break;
				default:
					break;
				}
			}
		}
	}

	if (canCastle) {
		score += CASTLE_S;
	}

	score += MOBILITY_S * possible_moves;

	return score;
}

double evaluate_early(BoardConfig config, bool canCastle, int side) {
	return 0;
}

double evaluate_late(BoardConfig config, bool canCastle, int side) {
	return 0;
}