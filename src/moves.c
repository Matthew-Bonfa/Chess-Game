#include "moves.h"

int16_t knight_offsets[8] = {NORTH + NORTH_EAST, EAST + NORTH_EAST, EAST + SOUTH_EAST, SOUTH + SOUTH_EAST, SOUTH + SOUTH_WEST, WEST + SOUTH_WEST, WEST + NORTH_WEST, NORTH + NORTH_WEST};
int16_t bishop_directions[4] = {NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST};
int16_t rook_directions[4] = {NORTH, EAST, SOUTH, WEST};

static inline void safe_set_bit(uint64_t *legal_moves, int32_t index) {
    if (index >= 0 && index <= 63) {
        *legal_moves |= ((uint64_t)1 << index);
    }
}

uint16_t check_if_pawn_moved(int16_t piece, uint16_t loc) {
    if (piece & COLOR_BIT_MASK) {
        if (loc >= BLACK_PAWN_START && loc <= BLACK_PAWN_END) {
            return FALSE;
        } else {
            return TRUE;
        }
    } else {
        if (loc >= WHITE_PAWN_START && loc <= WHITE_PAWN_END) {
            return FALSE;
        } else {
            return TRUE;
        }
    }
}

int32_t check_castles_king_side(uint32_t *game_state, uint16_t *game_condition, uint16_t color) {
    if (color == WHITE) {
        if (!check_game_condition(game_condition, WHITE_KING, WHITE_KING_BIT_POS) && !check_game_condition(game_condition, WHITE_ROOK, RIGHT_WHITE_ROOK)) {
            if (get_piece(game_state, piece_in_direction(game_state, WHITE_KING_POS, EAST)) == WHITE_ROOK) {
                return TRUE;
            }
        }
    } else {
        if (!check_game_condition(game_condition, BLACK_KING, BLACK_KING_BIT_POS) && !check_game_condition(game_condition, BLACK_ROOK, RIGHT_BLACK_ROOK)) {
            if (get_piece(game_state, piece_in_direction(game_state, BLACK_KING_POS, EAST)) == BLACK_ROOK) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

int32_t check_castles_queen_side(uint32_t *game_state, uint16_t *game_condition, uint16_t color) {
    if (color == WHITE) {
        if (!check_game_condition(game_condition, WHITE_KING, WHITE_KING_BIT_POS) && !check_game_condition(game_condition, WHITE_ROOK, LEFT_WHITE_ROOK)) {
            if (get_piece(game_state, piece_in_direction(game_state, WHITE_KING_POS, WEST)) == WHITE_ROOK) {
                return TRUE;
            }
        }
    } else {
        if (!check_game_condition(game_condition, BLACK_KING, BLACK_KING_BIT_POS) && !check_game_condition(game_condition, BLACK_ROOK, LEFT_BLACK_ROOK)) {
            if (get_piece(game_state, piece_in_direction(game_state, BLACK_KING_POS, WEST)) == BLACK_ROOK) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

void get_white_pawn_legal_moves(uint32_t *game_state, uint16_t loc, uint64_t *legal_moves) {
    int16_t current_piece = get_piece(game_state, loc);
    uint16_t north_piece_loc = piece_in_direction(game_state, loc, NORTH);

    if ((get_piece(game_state, loc + NORTH_WEST) & COLOR_BIT_MASK) == BLACK && get_piece(game_state, loc + NORTH_WEST) != BLACK_EMPTY) {
        safe_set_bit(legal_moves, loc + NORTH_WEST);
    }
    if ((get_piece(game_state, loc + NORTH_EAST) & COLOR_BIT_MASK) == BLACK && get_piece(game_state, loc + NORTH_EAST) != BLACK_EMPTY) {
        safe_set_bit(legal_moves, loc + NORTH_EAST);
    }
    if (loc - north_piece_loc > abs(NORTH)) {
        safe_set_bit(legal_moves, loc + NORTH);
    }
    if (loc - north_piece_loc > abs(2 * NORTH) && !check_if_pawn_moved(WHITE_PAWN, loc)) {
        safe_set_bit(legal_moves, loc + 2 * NORTH);
    }
}

void get_black_pawn_legal_moves(uint32_t *game_state, uint16_t loc, uint64_t *legal_moves) {
    int16_t current_piece = get_piece(game_state, loc);
    uint16_t south_piece_loc = piece_in_direction(game_state, loc, SOUTH);

    if ((get_piece(game_state, loc + SOUTH_WEST) & COLOR_BIT_MASK) == WHITE && get_piece(game_state, loc + SOUTH_WEST) != WHITE_EMPTY) {
        safe_set_bit(legal_moves, loc + SOUTH_WEST);
    }
    if ((get_piece(game_state, loc + SOUTH_EAST) & COLOR_BIT_MASK) == WHITE && get_piece(game_state, loc + SOUTH_EAST) != WHITE_EMPTY) {
        safe_set_bit(legal_moves, loc + SOUTH_EAST);
    }
    if (south_piece_loc - loc > SOUTH) {
        safe_set_bit(legal_moves, loc + SOUTH);
    }
    if (south_piece_loc - loc > 2 * SOUTH && !check_if_pawn_moved(BLACK_PAWN, loc)) {
        safe_set_bit(legal_moves, loc + 2 * SOUTH);
    }
}

void get_knight_legal_moves(uint32_t *game_state, uint16_t loc, uint64_t *legal_moves) {
    int16_t current_piece = get_piece(game_state, loc);

    for (int i = 0; i < 8; i++) {
        if (loc + knight_offsets[i] < 0 || loc + knight_offsets[i] > 63)
            continue;

        int src_col = loc % 8;
        int dst_col = (loc + knight_offsets[i]) % 8;
        int col_diff = abs(src_col - dst_col);
        if (col_diff != 1 && col_diff != 2)
            continue;

        int16_t target_piece = get_piece(game_state, loc + knight_offsets[i]);

        if (target_piece == WHITE_EMPTY || target_piece == BLACK_EMPTY ||
            ((target_piece & COLOR_BIT_MASK) != (current_piece & COLOR_BIT_MASK))) {
            safe_set_bit(legal_moves, loc + knight_offsets[i]);
        }
    }
}

void get_bishop_legal_moves(uint32_t *game_state, uint16_t loc, uint64_t *legal_moves) {
    int16_t current_piece = get_piece(game_state, loc);

    for (int i = 0; i < 4; i++) {
        int pos = loc + bishop_directions[i];
        while (pos >= 0 && pos <= 63) {
            int col_diff = abs((pos % 8) - (loc % 8));
            int row_diff = abs((pos / 8) - (loc / 8));
            if (col_diff != row_diff)
                break;

            int16_t piece = get_piece(game_state, pos);
            if (piece == WHITE_EMPTY || piece == BLACK_EMPTY) {
                safe_set_bit(legal_moves, pos);
            } else {
                if ((piece & COLOR_BIT_MASK) != (current_piece & COLOR_BIT_MASK)) {
                    safe_set_bit(legal_moves, pos);
                }
                break;
            }
            pos += bishop_directions[i];
        }
    }
}

void get_rook_legal_moves(uint32_t *game_state, uint16_t loc, uint64_t *legal_moves) {
    int16_t current_piece = get_piece(game_state, loc);

    for (int i = 0; i < 4; i++) {
        int pos = loc + rook_directions[i];
        while (pos >= 0 && pos <= 63) {
            int src_row = loc / BOARD_ROWS, dst_row = pos / BOARD_COLS;
            if ((rook_directions[i] == EAST || rook_directions[i] == WEST) && src_row != dst_row)
                break;

            int16_t piece = get_piece(game_state, pos);
            if (piece == WHITE_EMPTY || piece == BLACK_EMPTY) {
                safe_set_bit(legal_moves, pos);
            } else {
                if ((piece & COLOR_BIT_MASK) != (current_piece & COLOR_BIT_MASK)) {
                    safe_set_bit(legal_moves, pos);
                }
                break;
            }
            pos += rook_directions[i];
        }
    }
}

void get_king_legal_moves(uint32_t *game_state, uint16_t *game_condition, uint16_t loc, uint64_t *legal_moves) {
    int16_t current_piece = get_piece(game_state, loc);
    int directions[] = {NORTH, NORTH_EAST, EAST, SOUTH_EAST, SOUTH, SOUTH_WEST, WEST, NORTH_WEST};

    for (int i = 0; i < 8; i++) {
        int dest = loc + directions[i];
        if (dest < 0 || dest > 63) {
            continue;
        }

        int src_row = loc / 8, dst_row = dest / 8;
        if (abs((dest % 8) - (loc % 8)) > 1 || abs(dst_row - src_row) > 1) {
            continue;
        }

        int16_t target_piece = get_piece(game_state, dest);
        if (target_piece == WHITE_EMPTY || target_piece == BLACK_EMPTY ||
            ((target_piece & COLOR_BIT_MASK) != (current_piece & COLOR_BIT_MASK))) {
            safe_set_bit(legal_moves, dest);
        }
    }
    if (check_castles_king_side(game_state, game_condition, current_piece & COLOR_BIT_MASK)) {
        safe_set_bit(legal_moves, loc + 2 * EAST);
    }
    if (check_castles_queen_side(game_state, game_condition, current_piece & COLOR_BIT_MASK)) {
        safe_set_bit(legal_moves, loc + 2 * WEST);
    }
}
