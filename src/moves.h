#ifndef MOVES_H
#define MOVES_H

#include "constants.h"
#include "game.h"
#include <math.h>

uint16_t check_if_pawn_moved(int16_t piece, uint16_t loc);
uint16_t check_castles_king_side(uint32_t *game_state, uint16_t *game_condition, uint16_t color);
uint16_t check_castles_queen_side(uint32_t *game_state, uint16_t *game_condition, uint16_t color);
uint64_t get_attacked_squares(uint32_t *game_state, uint64_t *legal_moves, uint16_t color);
void get_checked_pieces(uint32_t *game_state, uint16_t loc, int16_t *pieces);
void get_white_pawn_legal_moves(uint32_t *game_state, uint16_t loc, uint64_t *legal_moves, uint32_t *prev_game_state);
void get_black_pawn_legal_moves(uint32_t *game_state, uint16_t loc, uint64_t *legal_moves, uint32_t *prev_game_state);
void get_knight_legal_moves(uint32_t *game_state, uint16_t loc, uint64_t *legal_moves);
void get_bishop_legal_moves(uint32_t *game_state, uint16_t loc, uint64_t *legal_moves);
void get_rook_legal_moves(uint32_t *game_state, uint16_t loc, uint64_t *legal_moves);
void get_king_legal_moves(uint32_t *game_state, uint16_t *game_condition, uint16_t loc, uint64_t *legal_moves);
void do_checks(uint32_t *game_state, uint64_t *legal_moves, uint16_t color);

#endif