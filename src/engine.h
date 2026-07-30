#ifndef ENGINE_H
#define ENGINE_H

#include "utils.h"
#include "constants.h"
#include "moves.h"
#include <stdint.h>
#include <stdlib.h>

typedef enum {
    PAWN = 1,
    KNIGHT = 3,
    BISHOP = 3,
    ROOK = 5,
    QUEEN = 9,
} piece_values;

#define PAWN_COL_W 1
#define PAWN_ROW_W 2

#define KNIGHT_COL_W 3
#define KNIGHT_ROW_W 2

#define BISHOP_DIAG_W 3

#endif

