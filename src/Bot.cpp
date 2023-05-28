#include "Bot.h"

#include <bits/stdc++.h>

extern PlaySide engineSide;
extern PlaySide sideToMove;
std::ofstream fout("output.txt");

const std::string Bot::BOT_NAME
    = "MyBot"; /* Edit this, escaped characters are forbidden */

Bot::Bot() { /* Initialize custom fields here */
    root = MoveNode();
    threeRepeatedConfigs = false;
}

void Bot::copyCurrentConfig() {
    /* deep copy the current config using an auxiliary matrix */
    BoardConfig currentGameBoard = BoardConfig(root.currentBoard);

    pastConfigs.push_back(currentGameBoard);
}

void setSides(PlaySide engineSide, PlaySide &enemySide) {
    enemySide = (engineSide == BLACK ? WHITE : BLACK);
}

bool Bot::checkRepeatedConfigs() {
    int counterRepeatedConfigs = 0;
    bool repeatedConfig;

    for (auto& pastConfig : pastConfigs) {
        repeatedConfig = true;
        for (int i = 1; i <= TABLE_SIZE && repeatedConfig; i++) {
            for (int j = 1; j <= TABLE_SIZE && repeatedConfig; j++) {
                if (pastConfig.config[i][j] != root.currentBoard[i][j]) {
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

void Bot::recordMove(Move* move, PlaySide sideToMove) {
    /* You might find it useful to also separately
     * record last move in another custom field */

    root.updateBoard(move, sideToMove);

    if (checkRepeatedConfigs()) {
        threeRepeatedConfigs = true;
    }

    copyCurrentConfig();

    fout << "board after player move:\n";
    showBoard();
}

Move* Bot::calculateNextMove() {
    /* Play move for the side the engine is playing (Hint: Main.getEngineSide())
     * Make sure to record your move in custom structures before returning.
     *
     * Return move that you are willing to submit
     * Move is to be constructed via one of the factory methods declared in
     * Move.h */
    PlaySide enemySide;
    setSides(engineSide, enemySide);

    MoveNode bestMove;

    double alpha = -INF;
    double beta = INF;
    double score = alphaBeta(engineSide, enemySide, DEPTH, alpha, beta, root, bestMove);

    Move* sentMove = bestMove.move;

    /* overwrite current game table */
    root = MoveNode(bestMove);

    fout << "board after bot move:\n";
    showBoard();

    fout << "score: " << score << "\n";

    return sentMove;
}

std::string Bot::getBotName() { return Bot::BOT_NAME; }

void Bot::showBoard() {
    /* that's how the chess table is usually seen */
    fout << "board:\n";
    for (int i = TABLE_SIZE; i > 0; i--) {
        for (int j = 1; j <= TABLE_SIZE; j++) {
            fout << root.currentBoard[i][j] << " ";
        }
        fout << "\n";
    }

    fout << "My captured Pieces: ";
    for (auto& piece : root.myCapturedPieces) {
        fout << piece << " ";
    }

    fout << "\n";

    fout << "Enemy captured Pieces: ";
    for (auto& piece : root.enemyCapturedPieces) {
        fout << piece << " ";
    }

    fout << "\n";
}

void Bot::showBoard2(MoveNode move) {
    /* that's how the chess table is usually seen */
    fout << "board:\n";
    for (int i = TABLE_SIZE; i > 0; i--) {
        for (int j = 1; j <= TABLE_SIZE; j++) {
            fout << move.currentBoard[i][j] << " ";
        }
        fout << "\n";
    }

    fout << "My captured Pieces: ";
    for (auto& piece : move.myCapturedPieces) {
        fout << piece << " ";
    }

    fout << "\n";

    fout << "Enemy captured Pieces: ";
    for (auto& piece : move.enemyCapturedPieces) {
        fout << piece << " ";
    }

    fout << "\n";
}

double Bot::alphaBeta(PlaySide myside, PlaySide enemyside, int depth, double alpha, double beta, MoveNode current, MoveNode &bestMove) {
    //generate all possible moves for player
    current.calculateAllNextMoves(myside);

    if (depth == 0 || !current.possibleMoves.size()) {
        return evaluate_early(current.currentBoard, current.castleNow, current.possibleMoves.size(), myside, engineSide);
    }

    double best_score = -INF;

    for (auto &move : current.possibleMoves) {
        double score = -alphaBeta(enemyside, myside, depth - 1, -beta, -alpha, move, bestMove);

        if (depth == DEPTH && score > best_score) {
            bestMove = move;
        }

        if (score > best_score) {
            best_score = score;
        }

        if (best_score > alpha) {
            alpha = best_score;
        }

        if (alpha >= beta) {
            break;
        }
    }

    return best_score;
}
