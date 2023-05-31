#include "MoveNode.h"

#include <bits/stdc++.h>

extern std::ofstream fout;

MoveNode::MoveNode(MoveNode *moveNode, Move *move) {
	for (int i = 1; i <= TABLE_SIZE; i++) {
        for (int j = 1; j <= TABLE_SIZE; j++) {
            this->currentBoard[i][j] = moveNode->currentBoard[i][j];
        }
    }

    this->shortCastle = moveNode->shortCastle;
    this->longCastle  = moveNode->longCastle;
    this->kingInCheck = moveNode->kingInCheck;
    this->previousKingInCheck = moveNode->previousKingInCheck;
    this->counterMoves = moveNode->counterMoves;
    this->castleNow = false;

    for (auto piece : moveNode->myCapturedPieces) {
        this->myCapturedPieces.push_back(piece);
    }

	for (auto piece : moveNode->enemyCapturedPieces) {
        this->enemyCapturedPieces.push_back(piece);
    }

    this->move = move;
}

MoveNode::MoveNode(MoveNode *moveNode) {
	for (int i = 1; i <= TABLE_SIZE; i++) {
        for (int j = 1; j <= TABLE_SIZE; j++) {
            this->currentBoard[i][j] = moveNode->currentBoard[i][j];
        }
    }

    this->shortCastle = moveNode->shortCastle;
    this->longCastle  = moveNode->longCastle;
    this->kingInCheck = moveNode->kingInCheck;
    this->previousKingInCheck = moveNode->previousKingInCheck;
    this->counterMoves = moveNode->counterMoves;
    this->castleNow = false;

    for (auto piece : moveNode->myCapturedPieces) {
        this->myCapturedPieces.push_back(piece);
    }

	for (auto piece : moveNode->enemyCapturedPieces) {
        this->enemyCapturedPieces.push_back(piece);
    }

    this->move = moveNode->move;
}

MoveNode::MoveNode() {
    setInitialBoard();
    shortCastle = true;
    longCastle  = true;
    kingInCheck = false;
    previousKingInCheck = false;
    castleNow = false;
    counterMoves = 0;
}

void MoveNode::setInitialBoard() {
    for (int i = 1; i <= TABLE_SIZE; i++) {
        for (int j = 1; j <= TABLE_SIZE; j++) {
            currentBoard[i][j] = NO_PIECE;
        }
    }

    currentBoard[1][1] = WHITE_ROOK;
    currentBoard[1][2] = WHITE_KNIGHT;
    currentBoard[1][3] = WHITE_BISHOP;
    currentBoard[1][4] = WHITE_QUEEN;
    currentBoard[1][5] = WHITE_KING;
    currentBoard[1][6] = WHITE_BISHOP;
    currentBoard[1][7] = WHITE_KNIGHT;
    currentBoard[1][8] = WHITE_ROOK;

    for (int i = 1; i <= TABLE_SIZE; i++) {
        currentBoard[2][i] = WHITE_PAWN;
        currentBoard[7][i] = BLACK_PAWN;
    }

    currentBoard[8][1] = BLACK_ROOK;
    currentBoard[8][2] = BLACK_KNIGHT;
    currentBoard[8][3] = BLACK_BISHOP;
    currentBoard[8][4] = BLACK_QUEEN;
    currentBoard[8][5] = BLACK_KING;
    currentBoard[8][6] = BLACK_BISHOP;
    currentBoard[8][7] = BLACK_KNIGHT;
    currentBoard[8][8] = BLACK_ROOK;
}

void MoveNode::findKing(PlaySidePiece king, int& x, int& y) {
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

PlaySidePiece MoveNode::switchSide(int x, int y) {
	if (currentBoard[x][y] == BLACK_EN_PAS 
		|| currentBoard[x][y] == P_BLACK_ROOK
        || currentBoard[x][y] == P_BLACK_BISHOP
        || currentBoard[x][y] == P_BLACK_KNIGHT
        || currentBoard[x][y] == P_BLACK_QUEEN) {
        return WHITE_PAWN;
    }

    if (currentBoard[x][y] == WHITE_EN_PAS
		|| currentBoard[x][y] == P_WHITE_ROOK
        || currentBoard[x][y] == P_WHITE_BISHOP
        || currentBoard[x][y] == P_WHITE_KNIGHT
        || currentBoard[x][y] == P_WHITE_QUEEN) {
        return BLACK_PAWN;
    }

    switch (currentBoard[x][y]) {
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
        return currentBoard[x][y];
    }
}

bool MoveNode::checkPawnAttack(PlaySide side, PlaySidePiece pawn, int& x,
    int& y) {
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

bool MoveNode::checkRookAttack(PlaySidePiece rook, int& x, int& y) {
    bool pieceBetween;

    for (int i = 1; i <= TABLE_SIZE; i++) {
        for (int j = 1; j <= TABLE_SIZE; j++) {
            if (currentBoard[i][j] != rook) {
                continue;
            }

            /* rook and king on the same row */
            if (x == i) {
                int start = std::min(y, j) + 1;
                int end   = std::max(y, j);

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
                int end   = std::max(x, i);

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

bool MoveNode::checkKnightAttack(PlaySidePiece knight, int& x, int& y) {
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

bool MoveNode::checkBishopAttack(PlaySidePiece bishop, int& x, int& y) {
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

bool MoveNode::checkQueenAttack(PlaySidePiece queen, int& x, int& y) {
    /* Queen is just a rook + bishop */
    return (checkRookAttack(queen, x, y) | checkBishopAttack(queen, x, y));
}

bool MoveNode::checkKingAttack(PlaySidePiece king, int& x, int& y) {
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

bool MoveNode::checkKingSafety(PlaySide sideToMove) {
    int x_king = 0;
    int y_king = 0;

    if (sideToMove == BLACK) {
        /* check for white king's safety */
        findKing(WHITE_KING, x_king, y_king);
        return !(checkPawnAttack(sideToMove, BLACK_PAWN, x_king, y_king)
                 | checkPawnAttack(sideToMove, BLACK_EN_PAS, x_king, y_king)
                 | checkRookAttack(BLACK_ROOK, x_king, y_king)
				 | checkRookAttack(P_BLACK_ROOK, x_king, y_king)
                 | checkKnightAttack(BLACK_KNIGHT, x_king, y_king)
				 | checkKnightAttack(P_BLACK_KNIGHT, x_king, y_king)
                 | checkBishopAttack(BLACK_BISHOP, x_king, y_king)
				 | checkBishopAttack(P_BLACK_BISHOP, x_king, y_king)
                 | checkQueenAttack(BLACK_QUEEN, x_king, y_king)
                 | checkQueenAttack(P_BLACK_QUEEN, x_king, y_king)
                 | checkKingAttack(BLACK_KING, x_king, y_king));
    } else {
        /* check for black king's safety */
        findKing(BLACK_KING, x_king, y_king);
        return !(checkPawnAttack(sideToMove, WHITE_PAWN, x_king, y_king)
                 | checkPawnAttack(sideToMove, WHITE_EN_PAS, x_king, y_king)
                 | checkRookAttack(WHITE_ROOK, x_king, y_king)
				 | checkRookAttack(P_WHITE_ROOK, x_king, y_king)
                 | checkKnightAttack(WHITE_KNIGHT, x_king, y_king)
				 | checkKnightAttack(P_WHITE_KNIGHT, x_king, y_king)
                 | checkBishopAttack(WHITE_BISHOP, x_king, y_king)
				 | checkBishopAttack(P_WHITE_BISHOP, x_king, y_king)
                 | checkQueenAttack(WHITE_QUEEN, x_king, y_king)
                 | checkQueenAttack(P_WHITE_QUEEN, x_king, y_king)
                 | checkKingAttack(WHITE_KING, x_king, y_king));
    }

    return true;
}

void coordinatesPosition(std::string position, int& x, int& y) {
    y = position[0] - 'a' + 1;
    x = position[1] - '0';
}

bool MoveNode::checkPawnOnBoard(int& x, int& y) {
    if (currentBoard[x][y] == BLACK_PAWN || currentBoard[x][y] == WHITE_PAWN
        || currentBoard[x][y] == BLACK_EN_PAS || currentBoard[x][y] == WHITE_EN_PAS) {
        return true;
    }

    return false;
}

bool MoveNode::checkSpecialCases(PlaySide sideToMove, int& x_start, int& y_start,
    int& x_end, int& y_end) {
    if (sideToMove == BLACK) {
        /* Black en passant from 7th to 5th row */
        if (x_start == 7 && x_end == 5
            && currentBoard[x_start][y_start] == BLACK_PAWN) {
            if (((y_end + 1 <= TABLE_SIZE)
                    && (currentBoard[x_end][y_end + 1] == WHITE_PAWN))
                || ((y_end - 1 > 0)
                    && (currentBoard[x_end][y_end - 1] == WHITE_PAWN))) {
                currentBoard[x_end][y_end] = BLACK_EN_PAS;
                return true;
            }
        }

        /* enemy does en passant */
        if (currentBoard[x_start][y_start] == BLACK_PAWN) {
            if (y_start + 1 <= TABLE_SIZE && y_end == y_start + 1 && x_end == x_start - 1
                && currentBoard[x_start][y_start + 1] == WHITE_EN_PAS) {
                enemyCapturedPieces.push_back(switchSide(x_start, y_start + 1));
                currentBoard[x_start][y_start + 1] = NO_PIECE;
                currentBoard[x_end][y_end] = currentBoard[x_start][y_start];
                return true;
            }

            if (y_start - 1 > 0 && y_end == y_start - 1 && x_end == x_start - 1
                && currentBoard[x_start][y_start - 1] == WHITE_EN_PAS) {
                enemyCapturedPieces.push_back(switchSide(x_start, y_start - 1));
                currentBoard[x_start][y_start - 1] = NO_PIECE;
                currentBoard[x_end][y_end] = currentBoard[x_start][y_start];
                return true;
            }
        }
    } else {
        /* White en passant from 2nd to 4th row */
        if (x_start == 2 && x_end == 4
            && currentBoard[x_start][y_start] == WHITE_PAWN) {
            if (((y_end + 1 <= TABLE_SIZE)
                    && (currentBoard[x_end][y_end + 1] == BLACK_PAWN))
                || ((y_end - 1 > 0)
                    && (currentBoard[x_end][y_end - 1] == BLACK_PAWN))) {
                currentBoard[x_end][y_end] = WHITE_EN_PAS;
                return true;
            }
        }

        /* enemy does en passant */
        if (currentBoard[x_start][y_start] == WHITE_PAWN) {
            if (y_start + 1 <= TABLE_SIZE && y_end == y_start + 1 && x_end == x_start + 1
                && currentBoard[x_start][y_start + 1] == BLACK_EN_PAS && currentBoard[x_end][y_end] == NO_PIECE) {
                enemyCapturedPieces.push_back(switchSide(x_start, y_start + 1));
                currentBoard[x_start][y_start + 1] = NO_PIECE;
                currentBoard[x_end][y_end] = currentBoard[x_start][y_start];
                return true;
            }

            if (y_start - 1 > 0 && y_end == y_start - 1 && x_end == x_start + 1
                && currentBoard[x_start][y_start - 1] == BLACK_EN_PAS && currentBoard[x_end][y_end] == NO_PIECE) {
                enemyCapturedPieces.push_back(switchSide(x_start, y_start - 1));
                currentBoard[x_start][y_start - 1] = NO_PIECE;
                currentBoard[x_end][y_end] = currentBoard[x_start][y_start];
                return true;
            }
        }
    }

    if (currentBoard[x_start][y_start] == BLACK_KING
        || currentBoard[x_start][y_start] == WHITE_KING) {
        if (y_end == y_start + 2) {
            currentBoard[x_end][y_end]         = currentBoard[x_start][y_start];
            currentBoard[x_end][y_end - 1]     = currentBoard[x_start][y_start + 3];
            currentBoard[x_start][y_start + 3] = NO_PIECE;
            return true;
        }

        if (y_end == y_start - 2) {
            currentBoard[x_end][y_end]         = currentBoard[x_start][y_start];
            currentBoard[x_end][y_end + 1]     = currentBoard[x_start][y_start - 4];
            currentBoard[x_start][y_start - 4] = NO_PIECE;
            return true;
        }
    }

    return false;
}

void MoveNode::revertEnPassantBoard(PlaySide sideToMove) {
    for (int i = 1; i <= TABLE_SIZE; i++) {
        for (int j = 1; j <= TABLE_SIZE; j++) {
            if (currentBoard[i][j] == BLACK_EN_PAS && sideToMove == BLACK) {
                currentBoard[i][j] = BLACK_PAWN;
            }

            if (currentBoard[i][j] == WHITE_EN_PAS && sideToMove == WHITE) {
                currentBoard[i][j] = WHITE_PAWN;
            }
        }
    }
}

void MoveNode::removeCapturedPiece(PlaySidePiece piece) {
    int counter = 0;

    for (auto &enemyPiece : enemyCapturedPieces) {
        if (enemyPiece == piece) {
            enemyCapturedPieces.erase(enemyCapturedPieces.begin() + counter);
            break;
        }

        counter++;
    }
}

void MoveNode::updateBoard(Move* move, PlaySide side) {
    int x_start, y_start, x_end, y_end;

    if (!move->isDropIn()) {
        coordinatesPosition(move->getSource().value(), x_start, y_start);
    }

    coordinatesPosition(move->getDestination().value(), x_end, y_end);

    counterMoves++;

    revertEnPassantBoard(side);

    if (currentBoard[x_end][y_end] != NO_PIECE) {
        enemyCapturedPieces.push_back(switchSide(x_end, y_end));
        counterMoves = 0;
    }

    if (move->isNormal()) {
        if (checkPawnOnBoard(x_start, y_start)) {
            counterMoves = 0;
        }

        if (!checkSpecialCases(side, x_start, y_start, x_end, y_end)) {
            currentBoard[x_end][y_end] = currentBoard[x_start][y_start];
        }

        currentBoard[x_start][y_start] = NO_PIECE;
    }

    if (move->isPromotion()) {
        if (checkPawnOnBoard(x_start, y_start)) {
            counterMoves = 0;
        }

        currentBoard[x_end][y_end]
            = convertRegularPiece(move->getReplacement().value(), side, false);
        currentBoard[x_start][y_start] = NO_PIECE;
    }

    if (move->isDropIn()) {
        currentBoard[x_end][y_end]
            = convertRegularPiece(move->getReplacement().value(), side, true);

        removeCapturedPiece(currentBoard[x_end][y_end]);
    }

    revertEnPassantBoard(side == WHITE ? BLACK : WHITE);

    if (!checkKingSafety(side)) {
        previousKingInCheck = true;
        kingInCheck         = true;
        shortCastle         = false;
        longCastle          = false;
    }
}

std::string stringPosition(int x, int y) {
    std::string position = "";
    position += 'a' + y - 1;
    position += '0' + x;

    return position;
}

void MoveNode::whitePawnMoves(int x, int y, PlaySide side) {
    if (side == BLACK) {
        return;
    }

    if (currentBoard[x][y] != WHITE_PAWN && currentBoard[x][y] != WHITE_EN_PAS) {
        return;
    }

	/* pawn can move 2 tiles */
    if (x == 2 && currentBoard[x + 1][y] == NO_PIECE
        && currentBoard[x + 2][y] == NO_PIECE) {
        MoveNode newMove(this,
            Move::moveTo(stringPosition(x, y), stringPosition(x + 2, y)));
        newMove.currentBoard[x][y] = NO_PIECE;
        
		/* check if the pawn becomes en passant pawn */
        if (((y - 1 > 0) && (newMove.currentBoard[x + 2][y - 1] == BLACK_PAWN))
            || ((y + 1 <= TABLE_SIZE)
                && (newMove.currentBoard[x + 2][y + 1] == BLACK_PAWN))) {
            newMove.currentBoard[x + 2][y] = WHITE_EN_PAS;
        } else {
            newMove.currentBoard[x + 2][y] = WHITE_PAWN;
        }

        if (newMove.checkKingSafety(BLACK)) {
            possibleMoves.push_back(newMove);
        }
    }

    /* promote pawn */
    if (x == 7) {
        if (currentBoard[x + 1][y] == NO_PIECE) {
            /* set current position to NO_PIECE and push back all possible
             * promotions */
            MoveNode newMoveQ(this,
                Move::promote(stringPosition(x, y), stringPosition(x + 1, y), QUEEN));
            newMoveQ.currentBoard[x][y]     = NO_PIECE;
            newMoveQ.currentBoard[x + 1][y] = P_WHITE_QUEEN;
            if (newMoveQ.checkKingSafety(BLACK)) {
                possibleMoves.push_back(newMoveQ);
            }

            MoveNode newMoveR(this,
                Move::promote(stringPosition(x, y), stringPosition(x + 1, y), ROOK));
            newMoveR.currentBoard[x][y]     = NO_PIECE;
            newMoveR.currentBoard[x + 1][y] = P_WHITE_ROOK;
            if (newMoveR.checkKingSafety(BLACK)) {
                possibleMoves.push_back(newMoveR);
            }

            MoveNode newMoveB(this,
                Move::promote(stringPosition(x, y), stringPosition(x + 1, y), BISHOP));
            newMoveB.currentBoard[x][y]     = NO_PIECE;
            newMoveB.currentBoard[x + 1][y] = P_WHITE_BISHOP;
            if (newMoveB.checkKingSafety(BLACK)) {
                possibleMoves.push_back(newMoveB);
            }

            MoveNode newMoveK(this,
                Move::promote(stringPosition(x, y), stringPosition(x + 1, y), KNIGHT));
            newMoveK.currentBoard[x][y]     = NO_PIECE;
            newMoveK.currentBoard[x + 1][y] = P_WHITE_KNIGHT;
            if (newMoveK.checkKingSafety(BLACK)) {
                possibleMoves.push_back(newMoveK);
            }
        }

        /* promotion + capture a piece */
        if (y - 1 > 0 && currentBoard[x + 1][y - 1] < NO_PIECE) {
            MoveNode newMoveQ(this, Move::promote(stringPosition(x, y),
                                           stringPosition(x + 1, y - 1), QUEEN));
            newMoveQ.myCapturedPieces.push_back(
                switchSide(x + 1, y - 1));
            newMoveQ.currentBoard[x][y]         = NO_PIECE;
            newMoveQ.currentBoard[x + 1][y - 1] = P_WHITE_QUEEN;
            if (newMoveQ.checkKingSafety(BLACK)) {
                possibleMoves.push_back(newMoveQ);
            }

            MoveNode newMoveR(this, Move::promote(stringPosition(x, y),
                                           stringPosition(x + 1, y - 1), ROOK));
            newMoveR.myCapturedPieces.push_back(
                switchSide(x + 1, y - 1));
            newMoveR.currentBoard[x][y]         = NO_PIECE;
            newMoveR.currentBoard[x + 1][y - 1] = P_WHITE_ROOK;
            if (newMoveR.checkKingSafety(BLACK)) {
                possibleMoves.push_back(newMoveR);
            }

            MoveNode newMoveB(this, Move::promote(stringPosition(x, y),
                                           stringPosition(x + 1, y - 1), BISHOP));
            newMoveB.myCapturedPieces.push_back(
                switchSide(x + 1, y - 1));
            newMoveB.currentBoard[x][y]         = NO_PIECE;
            newMoveB.currentBoard[x + 1][y - 1] = P_WHITE_BISHOP;
            if (newMoveB.checkKingSafety(BLACK)) {
                possibleMoves.push_back(newMoveB);
            }

            MoveNode newMoveK(this, Move::promote(stringPosition(x, y),
                                           stringPosition(x + 1, y - 1), KNIGHT));
            newMoveK.myCapturedPieces.push_back(
                switchSide(x + 1, y - 1));
            newMoveK.currentBoard[x][y]         = NO_PIECE;
            newMoveK.currentBoard[x + 1][y - 1] = P_WHITE_KNIGHT;
            if (newMoveK.checkKingSafety(BLACK)) {
                possibleMoves.push_back(newMoveK);
            }
        }

        /* promotion + capture a piece */
        if (y + 1 <= TABLE_SIZE && currentBoard[x + 1][y + 1] < NO_PIECE) {
            MoveNode newMoveQ(this, Move::promote(stringPosition(x, y),
                                           stringPosition(x + 1, y + 1), QUEEN));
            newMoveQ.myCapturedPieces.push_back(
                switchSide(x + 1, y + 1));
            newMoveQ.currentBoard[x][y]         = NO_PIECE;
            newMoveQ.currentBoard[x + 1][y + 1] = P_WHITE_QUEEN;
            if (newMoveQ.checkKingSafety(BLACK)) {
                possibleMoves.push_back(newMoveQ);
            }

            MoveNode newMoveR(this, Move::promote(stringPosition(x, y),
                                           stringPosition(x + 1, y + 1), ROOK));
            newMoveR.myCapturedPieces.push_back(
                switchSide(x + 1, y + 1));
            newMoveR.currentBoard[x][y]         = NO_PIECE;
            newMoveR.currentBoard[x + 1][y + 1] = P_WHITE_ROOK;
            if (newMoveR.checkKingSafety(BLACK)) {
                possibleMoves.push_back(newMoveR);
            }

            MoveNode newMoveB(this, Move::promote(stringPosition(x, y),
                                           stringPosition(x + 1, y + 1), BISHOP));
            newMoveB.myCapturedPieces.push_back(
                switchSide(x + 1, y + 1));
            newMoveB.currentBoard[x][y]         = NO_PIECE;
            newMoveB.currentBoard[x + 1][y + 1] = P_WHITE_BISHOP;
            if (newMoveB.checkKingSafety(BLACK)) {
                possibleMoves.push_back(newMoveB);
            }

            MoveNode newMoveK(this, Move::promote(stringPosition(x, y),
                                           stringPosition(x + 1, y + 1), KNIGHT));
            newMoveK.myCapturedPieces.push_back(
                switchSide(x + 1, y + 1));
            newMoveK.currentBoard[x][y]         = NO_PIECE;
            newMoveK.currentBoard[x + 1][y + 1] = P_WHITE_KNIGHT;
            if (newMoveK.checkKingSafety(BLACK)) {
                possibleMoves.push_back(newMoveK);
            }
        }
    }

    if (x + 1 <= TABLE_SIZE && x != 7) {
        /* move a single tile */
        if (currentBoard[x + 1][y] == NO_PIECE) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(x + 1, y)));
            newMove.currentBoard[x][y]     = NO_PIECE;
            newMove.currentBoard[x + 1][y] = WHITE_PAWN;
            if (newMove.checkKingSafety(BLACK)) {
                possibleMoves.push_back(newMove);
            }
        }

        /* capture a piece */
        if (y + 1 <= TABLE_SIZE && currentBoard[x + 1][y + 1] < NO_PIECE) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(x + 1, y + 1)));
            newMove.myCapturedPieces.push_back(switchSide(x + 1, y + 1));
            newMove.currentBoard[x][y]         = NO_PIECE;
            newMove.currentBoard[x + 1][y + 1] = WHITE_PAWN;
            if (newMove.checkKingSafety(BLACK)) {
                possibleMoves.push_back(newMove);
            }
        }

        /* capture a piece*/
        if (y - 1 > 0 && currentBoard[x + 1][y - 1] < NO_PIECE) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(x + 1, y - 1)));
            newMove.myCapturedPieces.push_back(switchSide(x + 1, y - 1));
            newMove.currentBoard[x][y]         = NO_PIECE;
            newMove.currentBoard[x + 1][y - 1] = WHITE_PAWN;
            if (newMove.checkKingSafety(BLACK)) {
                possibleMoves.push_back(newMove);
            }
        }

        /* capture an en passant pawn */
        if (y - 1 > 0 && currentBoard[x + 1][y] == NO_PIECE
            && currentBoard[x][y - 1] == BLACK_EN_PAS) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(x + 1, y - 1)));
            newMove.myCapturedPieces.push_back(switchSide(x, y - 1));
            newMove.currentBoard[x][y - 1]     = NO_PIECE;
            newMove.currentBoard[x][y]         = NO_PIECE;
            newMove.currentBoard[x + 1][y - 1] = WHITE_PAWN;
            if (newMove.checkKingSafety(BLACK)) {
                possibleMoves.push_back(newMove);
            }
        }

        /* capture an en passant pawn */
        if (y + 1 <= TABLE_SIZE && currentBoard[x + 1][y] == NO_PIECE
            && currentBoard[x][y + 1] == BLACK_EN_PAS) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(x + 1, y + 1)));
            newMove.myCapturedPieces.push_back(switchSide(x, y + 1));
            newMove.currentBoard[x][y + 1]     = NO_PIECE;
            newMove.currentBoard[x][y]         = NO_PIECE;
            newMove.currentBoard[x + 1][y + 1] = WHITE_PAWN;
            if (newMove.checkKingSafety(BLACK)) {
                possibleMoves.push_back(newMove);
            }
        }
    }
}

void MoveNode::blackPawnMoves(int x, int y, PlaySide side) {
    if (side == WHITE) {
        return;
    }

    if (currentBoard[x][y] != BLACK_PAWN && currentBoard[x][y] != BLACK_EN_PAS) {
        return;
    }

    /* pawn can move 2 tiles */
    if (x == 7 && currentBoard[x - 1][y] == NO_PIECE
        && currentBoard[x - 2][y] == NO_PIECE) {
        MoveNode newMove(this,
            Move::moveTo(stringPosition(x, y), stringPosition(x - 2, y)));
        newMove.currentBoard[x][y] = NO_PIECE;
        
        /* check if the pawn becomes en passant pawn */
        if (((y - 1 > 0) && (newMove.currentBoard[x - 2][y - 1] == WHITE_PAWN))
            || ((y + 1 <= TABLE_SIZE)
                && (newMove.currentBoard[x - 2][y + 1] == WHITE_PAWN))) {
            newMove.currentBoard[x - 2][y] = BLACK_EN_PAS;
        } else {
            newMove.currentBoard[x - 2][y] = BLACK_PAWN;
        }
        if (newMove.checkKingSafety(WHITE)) {
            possibleMoves.push_back(newMove);
        }
    }

    /* promote pawn */
    if (x == 2) {
        if (currentBoard[x - 1][y] == NO_PIECE) {
            MoveNode newMoveQ(this,
                Move::promote(stringPosition(x, y), stringPosition(x - 1, y), QUEEN));
            newMoveQ.currentBoard[x][y]     = NO_PIECE;
            newMoveQ.currentBoard[x - 1][y] = P_BLACK_QUEEN;
            if (newMoveQ.checkKingSafety(WHITE)) {
                possibleMoves.push_back(newMoveQ);
            }

            MoveNode newMoveR(this,
                Move::promote(stringPosition(x, y), stringPosition(x - 1, y), ROOK));
            newMoveR.currentBoard[x][y]     = NO_PIECE;
            newMoveR.currentBoard[x - 1][y] = P_BLACK_ROOK;
            if (newMoveR.checkKingSafety(WHITE)) {
                possibleMoves.push_back(newMoveR);
            }

            MoveNode newMoveB(this, Move::promote(stringPosition(x, y),
                                           stringPosition(x - 1, y), BISHOP));
            newMoveB.currentBoard[x][y]     = NO_PIECE;
            newMoveB.currentBoard[x - 1][y] = P_BLACK_BISHOP;
            if (newMoveB.checkKingSafety(WHITE)) {
                possibleMoves.push_back(newMoveB);
            }

            MoveNode newMoveK(this, Move::promote(stringPosition(x, y),
                                           stringPosition(x - 1, y), KNIGHT));
            newMoveK.currentBoard[x][y]     = NO_PIECE;
            newMoveK.currentBoard[x - 1][y] = P_BLACK_KNIGHT;
            if (newMoveK.checkKingSafety(WHITE)) {
                possibleMoves.push_back(newMoveK);
            }
        }

        /* promotion + capture a piece */
        if (y - 1 > 0 && currentBoard[x - 1][y - 1] > NO_PIECE) {
            MoveNode newMoveQ(this, Move::promote(stringPosition(x, y),
                                           stringPosition(x - 1, y - 1), QUEEN));
            newMoveQ.myCapturedPieces.push_back(
                switchSide(x - 1, y - 1));
            newMoveQ.currentBoard[x][y]         = NO_PIECE;
            newMoveQ.currentBoard[x - 1][y - 1] = P_BLACK_QUEEN;
            if (newMoveQ.checkKingSafety(WHITE)) {
                possibleMoves.push_back(newMoveQ);
            }

            MoveNode newMoveR(this, Move::promote(stringPosition(x, y),
                                           stringPosition(x - 1, y - 1), ROOK));
            newMoveR.myCapturedPieces.push_back(
                switchSide(x - 1, y - 1));
            newMoveR.currentBoard[x][y]         = NO_PIECE;
            newMoveR.currentBoard[x - 1][y - 1] = P_BLACK_ROOK;
            if (newMoveR.checkKingSafety(WHITE)) {
                possibleMoves.push_back(newMoveR);
            }

            MoveNode newMoveB(this, Move::promote(stringPosition(x, y),
                                           stringPosition(x - 1, y - 1), BISHOP));
            newMoveB.myCapturedPieces.push_back(
                switchSide(x - 1, y - 1));
            newMoveB.currentBoard[x][y]         = NO_PIECE;
            newMoveB.currentBoard[x - 1][y - 1] = P_BLACK_BISHOP;
            if (newMoveB.checkKingSafety(WHITE)) {
                possibleMoves.push_back(newMoveB);
            }

            MoveNode newMoveK(this, Move::promote(stringPosition(x, y),
                                           stringPosition(x - 1, y - 1), KNIGHT));
            newMoveK.myCapturedPieces.push_back(
                switchSide(x - 1, y - 1));
            newMoveK.currentBoard[x][y]         = NO_PIECE;
            newMoveK.currentBoard[x - 1][y - 1] = P_BLACK_KNIGHT;
            if (newMoveK.checkKingSafety(WHITE)) {
                possibleMoves.push_back(newMoveK);
            }
        }

        /* promotion + capture a piece */
        if (y + 1 <= TABLE_SIZE && currentBoard[x - 1][y + 1] > NO_PIECE) {
            MoveNode newMoveQ(this, Move::promote(stringPosition(x, y),
                                           stringPosition(x - 1, y + 1), QUEEN));
            newMoveQ.myCapturedPieces.push_back(
                switchSide(x - 1, y + 1));
            newMoveQ.currentBoard[x][y]         = NO_PIECE;
            newMoveQ.currentBoard[x - 1][y + 1] = P_BLACK_QUEEN;
            if (newMoveQ.checkKingSafety(WHITE)) {
                possibleMoves.push_back(newMoveQ);
            }

            MoveNode newMoveR(this, Move::promote(stringPosition(x, y),
                                           stringPosition(x - 1, y + 1), ROOK));
            newMoveR.myCapturedPieces.push_back(
                switchSide(x - 1, y + 1));
            newMoveR.currentBoard[x][y]         = NO_PIECE;
            newMoveR.currentBoard[x - 1][y + 1] = P_BLACK_ROOK;
            if (newMoveR.checkKingSafety(WHITE)) {
                possibleMoves.push_back(newMoveR);
            }

            MoveNode newMoveB(this, Move::promote(stringPosition(x, y),
                                           stringPosition(x - 1, y + 1), BISHOP));
            newMoveB.myCapturedPieces.push_back(
                switchSide(x - 1, y + 1));
            newMoveB.currentBoard[x][y]         = NO_PIECE;
            newMoveB.currentBoard[x - 1][y + 1] = P_BLACK_BISHOP;
            if (newMoveB.checkKingSafety(WHITE)) {
                possibleMoves.push_back(newMoveB);
            }

            MoveNode newMoveK(this, Move::promote(stringPosition(x, y),
                                           stringPosition(x - 1, y + 1), KNIGHT));
            newMoveK.myCapturedPieces.push_back(
                switchSide(x - 1, y + 1));
            newMoveK.currentBoard[x][y]         = NO_PIECE;
            newMoveK.currentBoard[x - 1][y + 1] = P_BLACK_KNIGHT;
            if (newMoveK.checkKingSafety(WHITE)) {
                possibleMoves.push_back(newMoveK);
            }
        }
    }

    if (x - 1 > 0 && x != 2) {
        /* move a single tile */
        if (currentBoard[x - 1][y] == NO_PIECE) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(x - 1, y)));
            newMove.currentBoard[x][y]     = NO_PIECE;
            newMove.currentBoard[x - 1][y] = BLACK_PAWN;
            if (newMove.checkKingSafety(WHITE)) {
                possibleMoves.push_back(newMove);
            }
        }

        /* capture a piece */
        if (y + 1 <= TABLE_SIZE && currentBoard[x - 1][y + 1] > NO_PIECE) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(x - 1, y + 1)));
            newMove.myCapturedPieces.push_back(switchSide(x - 1, y + 1));
            newMove.currentBoard[x][y]         = NO_PIECE;
            newMove.currentBoard[x - 1][y + 1] = BLACK_PAWN;
            if (newMove.checkKingSafety(WHITE)) {
                possibleMoves.push_back(newMove);
            }
        }

        /* capture a piece */
        if (y - 1 > 0 && currentBoard[x - 1][y - 1] > NO_PIECE) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(x - 1, y - 1)));
            newMove.myCapturedPieces.push_back(switchSide(x - 1, y - 1));
            newMove.currentBoard[x][y]         = NO_PIECE;
            newMove.currentBoard[x - 1][y - 1] = BLACK_PAWN;
            if (newMove.checkKingSafety(WHITE)) {
                possibleMoves.push_back(newMove);
            }
        }

        /* capture an en passant pawn */
        if (y - 1 > 0 && currentBoard[x - 1][y] == NO_PIECE
            && currentBoard[x][y - 1] == WHITE_EN_PAS) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(x - 1, y - 1)));
            newMove.myCapturedPieces.push_back(switchSide(x, y - 1));
            newMove.currentBoard[x][y - 1]     = NO_PIECE;
            newMove.currentBoard[x][y]         = NO_PIECE;
            newMove.currentBoard[x - 1][y - 1] = BLACK_PAWN;
            if (newMove.checkKingSafety(WHITE)) {
                possibleMoves.push_back(newMove);
            }
        }

        /* capture an en passant pawn */
        if (y + 1 <= TABLE_SIZE && currentBoard[x - 1][y] == NO_PIECE
            && currentBoard[x][y + 1] == WHITE_EN_PAS) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(x - 1, y + 1)));
            newMove.myCapturedPieces.push_back(switchSide(x, y + 1));
            newMove.currentBoard[x][y + 1]     = NO_PIECE;
            newMove.currentBoard[x][y]         = NO_PIECE;
            newMove.currentBoard[x - 1][y + 1] = BLACK_PAWN;
            if (newMove.checkKingSafety(WHITE)) {
                possibleMoves.push_back(newMove);
            }
        }
    }
}

void MoveNode::rookMoves(int x, int y, PlaySide side) {
    if ((side == WHITE && currentBoard[x][y] < NO_PIECE)
        || (side == BLACK && currentBoard[x][y] > NO_PIECE)) {
        return;
    }

    /* up of rook */
    for (int i = x + 1; i <= TABLE_SIZE; i++) {
        if (currentBoard[i][y] != NO_PIECE
            && currentBoard[i][y] * currentBoard[x][y] < 0) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(i, y)));
            newMove.myCapturedPieces.push_back(switchSide(i, y));
            newMove.currentBoard[i][y] = newMove.currentBoard[x][y];
            newMove.currentBoard[x][y] = NO_PIECE;

            if (y == 1) {
                newMove.longCastle = false;
            }

            if (y == TABLE_SIZE) {
                newMove.shortCastle = false;
            }

            if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                possibleMoves.push_back(newMove);
            }

            break;
        } else if (currentBoard[i][y] == NO_PIECE) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(i, y)));
            newMove.currentBoard[i][y] = newMove.currentBoard[x][y];
            newMove.currentBoard[x][y] = NO_PIECE;

            if (y == 1) {
                newMove.longCastle = false;
            }

            if (y == TABLE_SIZE) {
                newMove.shortCastle = false;
            }

            if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                possibleMoves.push_back(newMove);
            }
        } else {
            break;
        }
    }

    /* down of rook */
    for (int i = x - 1; i > 0; i--) {
        if (currentBoard[i][y] != NO_PIECE
            && currentBoard[i][y] * currentBoard[x][y] < 0) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(i, y)));
            newMove.myCapturedPieces.push_back(switchSide(i, y));
            newMove.currentBoard[i][y] = newMove.currentBoard[x][y];
            newMove.currentBoard[x][y] = NO_PIECE;

            if (y == 1) {
                newMove.longCastle = false;
            }

            if (y == TABLE_SIZE) {
                newMove.shortCastle = false;
            }

            if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                possibleMoves.push_back(newMove);
            }

            break;
        } else if (currentBoard[i][y] == NO_PIECE) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(i, y)));
            newMove.currentBoard[i][y] = newMove.currentBoard[x][y];
            newMove.currentBoard[x][y] = NO_PIECE;

            if (y == 1) {
                newMove.longCastle = false;
            }

            if (y == TABLE_SIZE) {
                newMove.shortCastle = false;
            }

            if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                possibleMoves.push_back(newMove);
            }
        } else {
            break;
        }
    }

    /* left of rook */

    for (int i = y - 1; i > 0; i--) {
        if (currentBoard[x][i] != NO_PIECE
            && currentBoard[x][i] * currentBoard[x][y] < 0) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(x, i)));
            newMove.myCapturedPieces.push_back(switchSide(x, i));
            newMove.currentBoard[x][i] = newMove.currentBoard[x][y];
            newMove.currentBoard[x][y] = NO_PIECE;

            if (y == 1) {
                newMove.longCastle = false;
            }

            if (y == TABLE_SIZE) {
                newMove.shortCastle = false;
            }

            if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                possibleMoves.push_back(newMove);
            }
            break;
        } else if (currentBoard[x][i] == NO_PIECE) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(x, i)));
            newMove.currentBoard[x][i] = newMove.currentBoard[x][y];
            newMove.currentBoard[x][y] = NO_PIECE;

            if (y == 1) {
                newMove.longCastle = false;
            }

            if (y == TABLE_SIZE) {
                newMove.shortCastle = false;
            }

            if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                possibleMoves.push_back(newMove);
            }
        } else {
            break;
        }
    }

    /* right of rook */

    for (int i = y + 1; i <= TABLE_SIZE; i++) {
        if (currentBoard[x][i] != NO_PIECE
            && currentBoard[x][i] * currentBoard[x][y] < 0) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(x, i)));
            newMove.myCapturedPieces.push_back(switchSide(x, i));
            newMove.currentBoard[x][i] = newMove.currentBoard[x][y];
            newMove.currentBoard[x][y] = NO_PIECE;

            if (y == 1) {
                newMove.longCastle = false;
            }

            if (y == TABLE_SIZE) {
                newMove.shortCastle = false;
            }

            if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                possibleMoves.push_back(newMove);
            }
            break;
        } else if (currentBoard[x][i] == NO_PIECE) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(x, i)));
            newMove.currentBoard[x][i] = newMove.currentBoard[x][y];
            newMove.currentBoard[x][y] = NO_PIECE;

            if (y == 1) {
                newMove.longCastle = false;
            }

            if (y == TABLE_SIZE) {
                newMove.shortCastle = false;
            }

            if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                possibleMoves.push_back(newMove);
            }
        } else {
            break;
        }
    }
}

void MoveNode::knightMoves(int x, int y, PlaySide side) {
    if ((side == WHITE && currentBoard[x][y] < 0)
        || (side == BLACK && currentBoard[x][y] > 0)) {
        return;
    }

    int dx[KNIGHT_MOVES] = {2, 1, -1, -2, -2, -1, 1, 2};
    int dy[KNIGHT_MOVES] = {1, 2, 2, 1, -1, -2, -2, -1};

    for (int i = 0; i < KNIGHT_MOVES; i++) {
        if (x + dx[i] <= 0 || x + dx[i] > TABLE_SIZE || y + dy[i] <= 0
            || y + dy[i] > TABLE_SIZE) {
            continue;
        }

        MoveNode newMove(this,
            Move::moveTo(stringPosition(x, y), stringPosition(x + dx[i], y + dy[i])));

        if (currentBoard[x + dx[i]][y + dy[i]] != NO_PIECE) {
            /* a piece on the desired spot => check if it can be captured or not
             */
            if (side == WHITE && currentBoard[x + dx[i]][y + dy[i]] < NO_PIECE) {
                newMove.myCapturedPieces.push_back(
                    switchSide(x + dx[i], y + dy[i]));
                newMove.currentBoard[x][y]                 = NO_PIECE;
                newMove.currentBoard[x + dx[i]][y + dy[i]] = WHITE_KNIGHT;
                if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                    possibleMoves.push_back(newMove);
                }
            }

            if (side == BLACK && currentBoard[x + dx[i]][y + dy[i]] > NO_PIECE) {
                newMove.myCapturedPieces.push_back(
                    switchSide(x + dx[i], y + dy[i]));
                newMove.currentBoard[x][y]                 = NO_PIECE;
                newMove.currentBoard[x + dx[i]][y + dy[i]] = BLACK_KNIGHT;
                if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                    possibleMoves.push_back(newMove);
                }
            }
        } else {
            /* no piece on the desired spot => free to move there */
            newMove.currentBoard[x + dx[i]][y + dy[i]]
                = newMove.currentBoard[x][y];
            newMove.currentBoard[x][y] = NO_PIECE;
            if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                possibleMoves.push_back(newMove);
            }
        }
    }
}

void MoveNode::bishopMoves(int x, int y, PlaySide side) {
    if ((side == WHITE && currentBoard[x][y] < NO_PIECE)
        || (side == BLACK && currentBoard[x][y] > NO_PIECE)) {
        return;
    }

    /* up right */
    for (int i = 1; i + x <= TABLE_SIZE && i + y <= TABLE_SIZE;
         i++) {
        if (currentBoard[x + i][y + i] != NO_PIECE
            && currentBoard[x + i][y + i] * currentBoard[x][y] < 0) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(x + i, y + i)));
            newMove.myCapturedPieces.push_back(switchSide(x + i, y + i));
            newMove.currentBoard[x + i][y + i] = newMove.currentBoard[x][y];
            newMove.currentBoard[x][y]         = NO_PIECE;

            if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                possibleMoves.push_back(newMove);
            }

            break;
        } else if (currentBoard[x + i][y + i] == NO_PIECE) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(x + i, y + i)));
            newMove.currentBoard[x + i][y + i] = newMove.currentBoard[x][y];
            newMove.currentBoard[x][y]         = NO_PIECE;

            if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                possibleMoves.push_back(newMove);
            }
        } else {
            break;
        }
    }

    /* down right */
    for (int i = 1; x - i > 0 && y + i <= TABLE_SIZE; i++) {
        if (currentBoard[x - i][y + i] != NO_PIECE
            && currentBoard[x - i][y + i] * currentBoard[x][y] < 0) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(x - i, y + i)));
            newMove.myCapturedPieces.push_back(switchSide(x - i, y + i));
            newMove.currentBoard[x - i][y + i] = newMove.currentBoard[x][y];
            newMove.currentBoard[x][y]         = NO_PIECE;

            if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                possibleMoves.push_back(newMove);
            }

            break;
        } else if (currentBoard[x - i][y + i] == NO_PIECE) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(x - i, y + i)));
            newMove.currentBoard[x - i][y + i] = newMove.currentBoard[x][y];
            newMove.currentBoard[x][y]         = NO_PIECE;

            if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                possibleMoves.push_back(newMove);
            }
        } else {
            break;
        }
    }

    /* up left */
    for (int i = 1; x + i <= TABLE_SIZE && y - i > 0; i++) {
        if (currentBoard[x + i][y - i] != NO_PIECE
            && currentBoard[x + i][y - i] * currentBoard[x][y] < 0) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(x + i, y - i)));
            newMove.myCapturedPieces.push_back(switchSide(x + i, y - i));
            newMove.currentBoard[x + i][y - i] = newMove.currentBoard[x][y];
            newMove.currentBoard[x][y]         = NO_PIECE;
            
            if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                possibleMoves.push_back(newMove);
            }

            break;
        } else if (currentBoard[x + i][y - i] == NO_PIECE) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(x + i, y - i)));
            newMove.currentBoard[x + i][y - i] = newMove.currentBoard[x][y];
            newMove.currentBoard[x][y]         = NO_PIECE;
            
            if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                possibleMoves.push_back(newMove);
            }
        } else {
            break;
        }
    }

    /* down left */
    for (int i = 1; x - i > 0 && y - i > 0; i++) {
        if (currentBoard[x - i][y - i] != NO_PIECE
            && currentBoard[x - i][y - i] * currentBoard[x][y] < 0) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(x - i, y - i)));
            newMove.myCapturedPieces.push_back(switchSide(x - i, y - i));
            newMove.currentBoard[x - i][y - i] = newMove.currentBoard[x][y];
            newMove.currentBoard[x][y]         = NO_PIECE;
            
            if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                possibleMoves.push_back(newMove);
            }
            
            break;
        } else if (currentBoard[x - i][y - i] == NO_PIECE) {
            MoveNode newMove(this,
                Move::moveTo(stringPosition(x, y), stringPosition(x - i, y - i)));
            newMove.currentBoard[x - i][y - i] = newMove.currentBoard[x][y];
            newMove.currentBoard[x][y]         = NO_PIECE;
            
            if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                possibleMoves.push_back(newMove);
            }
        } else {
            break;
        }
    }
}

void MoveNode::queenMoves(int x, int y, PlaySide side) {
    if ((side == WHITE && currentBoard[x][y] < NO_PIECE)
        || (side == BLACK && currentBoard[x][y] > NO_PIECE)) {
        return;
    }

    rookMoves(x, y, side);
    bishopMoves(x, y, side);
}

void MoveNode::kingMoves(int x, int y, PlaySide side) {
    if ((side == WHITE && currentBoard[x][y] != WHITE_KING)
        || (side == BLACK && currentBoard[x][y] != BLACK_KING)) {
        return;
    }

    int dx[KING_MOVES] = {1, 0, -1, -1, -1, 0, 1, 1};
    int dy[KING_MOVES] = {1, 1, 1, 0, -1, -1, -1, 0};

    for (int i = 0; i < KING_MOVES; i++) {
        if (x + dx[i] <= 0 || x + dx[i] > TABLE_SIZE || y + dy[i] <= 0
            || y + dy[i] > TABLE_SIZE) {
            continue;
        }

        MoveNode newMove(this,
            Move::moveTo(stringPosition(x, y), stringPosition(x + dx[i], y + dy[i])));

        newMove.shortCastle = false;
        newMove.longCastle  = false;

        if (currentBoard[x + dx[i]][y + dy[i]] != NO_PIECE) {
            if (side == WHITE && currentBoard[x + dx[i]][y + dy[i]] < NO_PIECE) {
                newMove.myCapturedPieces.push_back(
                    switchSide(x + dx[i], y + dy[i]));
                newMove.currentBoard[x + dx[i]][y + dy[i]]
                    = newMove.currentBoard[x][y];
                newMove.currentBoard[x][y] = NO_PIECE;

                if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                    possibleMoves.push_back(newMove);
                }
            }

            if (side == BLACK && currentBoard[x + dx[i]][y + dy[i]] > NO_PIECE) {
                newMove.myCapturedPieces.push_back(
                    switchSide(x + dx[i], y + dy[i]));

                newMove.currentBoard[x + dx[i]][y + dy[i]]
                    = newMove.currentBoard[x][y];
                newMove.currentBoard[x][y] = NO_PIECE;

                if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                    possibleMoves.push_back(newMove);
                }
            }
        } else {
            newMove.currentBoard[x + dx[i]][y + dy[i]]
                = newMove.currentBoard[x][y];
            newMove.currentBoard[x][y] = NO_PIECE;

            if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                possibleMoves.push_back(newMove);
            }
        }
    }

    if (shortCastle && currentBoard[x][y + 1] == NO_PIECE
        && currentBoard[x][y + 2] == NO_PIECE) {
        MoveNode newMove(this,
            Move::moveTo(stringPosition(x, y), stringPosition(x, y + 2)));
        newMove.currentBoard[x][y + 1] = newMove.currentBoard[x][y];
        newMove.currentBoard[x][y]     = NO_PIECE;

        if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
            
            newMove.currentBoard[x][y + 2] = newMove.currentBoard[x][y + 1];
            newMove.currentBoard[x][y + 1] = newMove.currentBoard[x][TABLE_SIZE];
            newMove.currentBoard[x][TABLE_SIZE] = NO_PIECE;

            newMove.shortCastle = false;
            newMove.longCastle  = false;

            if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                possibleMoves.push_back(newMove);
                castleMoveIndex = possibleMoves.size() - 1;
                castleNow = true;
            }
        }
    }

    if (longCastle && currentBoard[x][y - 1] == NO_PIECE
        && currentBoard[x][y - 2] == NO_PIECE && currentBoard[x][y - 3] == NO_PIECE) {
        MoveNode newMove(this,
            Move::moveTo(stringPosition(x, y), stringPosition(x, y - 2)));
        newMove.currentBoard[x][y - 1] = newMove.currentBoard[x][y];
        newMove.currentBoard[x][y]     = NO_PIECE;

        if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
            
            newMove.currentBoard[x][y - 2] = newMove.currentBoard[x][y - 1];
            newMove.currentBoard[x][y - 1] = newMove.currentBoard[x][1];
            newMove.currentBoard[x][1]     = NO_PIECE;

            newMove.shortCastle = false;
            newMove.longCastle  = false;

            if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
                possibleMoves.push_back(newMove);
                castleMoveIndex = possibleMoves.size() - 1;
                castleNow = true;
            }
        }
    }
}

PlaySidePiece convertRegularPiece(Piece piece, PlaySide mySide, bool type) {
    /* type = false -> promotion */
    if (!type) {
        if (mySide == BLACK) {
            switch (piece) {
                case QUEEN:
                    return P_BLACK_QUEEN;
                case ROOK:
                    return P_BLACK_ROOK;
                case BISHOP:
                    return P_BLACK_BISHOP;
                case KNIGHT:
                    return P_BLACK_KNIGHT;
                default:
                    return NO_PIECE;
            }
        } else {
            switch (piece) {
                case QUEEN:
                    return P_WHITE_QUEEN;
                case ROOK:
                    return P_WHITE_ROOK;
                case BISHOP:
                    return P_WHITE_BISHOP;
                case KNIGHT:
                    return P_WHITE_KNIGHT;
                default:
                    return NO_PIECE;
            }
        }
    }

    /* type = true -> drop in*/
    switch (mySide) {
    case BLACK:
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

    case WHITE:
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

Piece convertPlaySidePiece(PlaySidePiece piece) {
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

void MoveNode::crazyHouseMoves(int x, int y, PlaySide side) {
    int counter = -1;
    std::map<PlaySidePiece, bool> usedPieces;

    for (auto& piece : myCapturedPieces) {
        counter++;
        
        if (usedPieces.find(piece) == usedPieces.end()) {
            usedPieces[piece] = true;
        } else {
            continue;
        }

        if ((x == 1 || x == 8)
            && (piece == WHITE_PAWN || piece == BLACK_PAWN)) {
            continue;
        }

        MoveNode newMove(this,
            Move::dropIn(stringPosition(x, y), convertPlaySidePiece(piece)));
        newMove.currentBoard[x][y] = piece;
        newMove.myCapturedPieces.erase(
            newMove.myCapturedPieces.begin() + counter);

        if (newMove.checkKingSafety(side == WHITE ? BLACK : WHITE)) {
            possibleMoves.push_back(newMove);
        }
    }
}

void MoveNode::calculateAllNextMoves(PlaySide engineSide) {
    castleNow = false;
    
    for (int i = 1; i <= TABLE_SIZE; i++) {
        for (int j = 1; j <= TABLE_SIZE; j++) {
            switch (currentBoard[i][j]) {
            case WHITE_PAWN:
            case WHITE_EN_PAS: {
                whitePawnMoves(i, j, engineSide);
                break;
            }
            case BLACK_PAWN:
            case BLACK_EN_PAS: {
                blackPawnMoves(i, j, engineSide);
                break;
            }
            case P_WHITE_ROOK:
            case P_BLACK_ROOK:
            case WHITE_ROOK:
            case BLACK_ROOK: {
                rookMoves(i, j, engineSide);
                break;
            }
            case P_WHITE_KNIGHT:
            case P_BLACK_KNIGHT:
            case WHITE_KNIGHT:
            case BLACK_KNIGHT: {
                knightMoves(i, j, engineSide);
                break;
            }
            case P_WHITE_BISHOP:
            case P_BLACK_BISHOP:
            case WHITE_BISHOP:
            case BLACK_BISHOP: {
                bishopMoves(i, j, engineSide);
                break;
            }
            case WHITE_QUEEN:
            case BLACK_QUEEN:
            case P_WHITE_QUEEN:
            case P_BLACK_QUEEN: {
                queenMoves(i, j, engineSide);
                break;
            }
            case WHITE_KING:
            case BLACK_KING: {
                kingMoves(i, j, engineSide);
                break;
            }
            default:
                crazyHouseMoves(i, j, engineSide);
                break;
            }
        }
    }
}