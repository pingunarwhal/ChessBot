#include "BoardHelpers.h"

#include <bits/stdc++.h>

BoardConfig::BoardConfig(
    PlaySidePiece currentConfig[TABLE_SIZE + 1][TABLE_SIZE + 1]) {
    for (int i = 1; i <= TABLE_SIZE; i++) {
        for (int j = 1; j <= TABLE_SIZE; j++) {
            this->config[i][j] = currentConfig[i][j];
        }
    }
}