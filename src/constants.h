#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>

#define TRUE 1
#define FALSE 0

#define BOARD_ROWS 8
#define BOARD_COLS 8
#define BOARD_SQUARES BOARD_ROWS *BOARD_COLS

#define SQUARE_EDGES 8
#define SQUARE_INDICES 6

#define PRIMARY_COLOR "#769656"
#define SECONDARY_COLOR "#eeeed2"
#define PRIMARY_SELECTED "#baca44"
#define SECONDARY_SELECETED "#ffffff"

#define WHITE_EMPTY 0
#define BLACK_EMPTY 8

#define WHITE_PAWN 1
#define WHITE_KNIGHT 2
#define WHITE_BISHOP 3
#define WHITE_ROOK 4
#define WHITE_QUEEN 5
#define WHITE_KING 6

#define BLACK_PAWN 9
#define BLACK_KNIGHT 10
#define BLACK_BISHOP 11
#define BLACK_ROOK 12
#define BLACK_QUEEN 13
#define BLACK_KING 14

#define SIZE_BITS 4
#define BIT_MASK ((uint32_t)1)
#define PIECE_BIT_MASK ((uint32_t)15)
#define COLOR_BIT_MASK ((uint32_t)8)

#define NUM_STARTING_PIECES 32

#define INVALID_SQUARE 64
#define INVALID_MOVE 0
#define VALID_MOVE 1
#define MOVE_FAIL 0
#define MOVE_SUCCESS 1

#define WHITE_TO_MOVE 0
#define BLACK_TO_MOVE 1

#define WHITE_PAWN_START 48
#define WHITE_PAWN_END 55

#define BLACK_PAWN_START 8
#define BLACK_PAWN_END 15

#define LEFT_WHITE_ROOK 56
#define RIGHT_WHITE_ROOK 63
#define WHITE_ROOK_BIT_POS 0

#define LEFT_BLACK_ROOK 0
#define RIGHT_BLACK_ROOK 7
#define BLACK_ROOK_BIT_POS 4

#define WHITE_KING_POS 60
#define WHITE_KING_BIT_POS 8

#define BLACK_KING_POS 4
#define BLACK_KING_BIT_POS 12

#define EMPTY_DIRECTION -1

#endif