#ifndef GAME_H
#define GAME_H

#include "constants.h"
#include "moves.h"
#include <stdio.h>

typedef enum Color {
    WHITE = 0,
    BLACK = 8
} Color;

typedef enum Direction {
    NORTH = -8,
    NORTH_EAST = -7,
    EAST = 1,
    SOUTH_EAST = 9,
    SOUTH = 8,
    SOUTH_WEST = 7,
    WEST = -1,
    NORTH_WEST = -9
} Direction;

int16_t get_piece(uint32_t *game_state, uint16_t loc);
uint32_t check_game_condition(uint16_t *game_condition, uint16_t piece, int32_t location);
void set_piece_moved(uint16_t *game_condition, uint16_t piece, int32_t location);
int16_t piece_in_direction(uint32_t *game_state, int32_t loc, Direction direction);
uint16_t check_valid_move(uint32_t *game_state, uint16_t *game_condition, uint64_t *legal_moves, int32_t loc, int32_t dest);
void update_legal_moves(uint32_t *game_state, uint16_t *game_condition, uint64_t *legal_moves);
void move_piece(uint32_t *game_state, uint16_t loc, uint16_t dest);
uint16_t make_move(uint32_t *game_state, uint16_t *game_condition, uint64_t *legal_moves, int32_t *selected_square, int32_t *clicked_square);

#endif