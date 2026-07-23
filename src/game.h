#ifndef GAME_H
#define GAME_H

#include "constants.h"
#include "moves.h"
#include <stdio.h>
#include <string.h>

void set_piece(uint32_t *game_state, uint16_t loc, uint16_t piece);
void set_piece_moved(uint16_t *game_condition, uint16_t piece, int32_t location);
uint16_t check_valid_move(uint32_t *game_state, uint16_t *game_condition, uint64_t *legal_moves, int32_t loc, int32_t dest);
void update_legal_moves(uint32_t *game_state, uint16_t *game_condition, uint64_t *legal_moves, uint32_t *prev_game_state, uint16_t color_to_move);
void move_piece(uint32_t *game_state, uint16_t loc, uint16_t dest);
uint16_t make_move(uint32_t *game_state, uint16_t *game_condition, uint64_t *legal_moves, int32_t *selected_square, int32_t *clicked_square, uint16_t *color_to_move, uint32_t *prev_game_state);

#endif
