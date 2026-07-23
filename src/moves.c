#include "moves.h"

int16_t knight_offsets[8] = {NORTH + NORTH_EAST, EAST + NORTH_EAST, EAST + SOUTH_EAST, SOUTH + SOUTH_EAST,
                             SOUTH + SOUTH_WEST, WEST + SOUTH_WEST, WEST + NORTH_WEST, NORTH + NORTH_WEST};
int16_t bishop_directions[4] = {NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST};
int16_t rook_directions[4] = {NORTH, EAST, SOUTH, WEST};
int16_t all_directions[8] = {NORTH, NORTH_EAST, EAST, SOUTH_EAST, SOUTH, SOUTH_WEST, WEST, NORTH_WEST};

uint64_t white_castles_king_mask = (uint64_t)6917529027641081856;
uint64_t white_castles_queen_mask = (uint64_t)1008806316530991104;
uint64_t black_castles_king_mask = (uint64_t)96;
uint64_t black_castles_queen_mask = (uint64_t)14;

static inline void safe_set_bit(uint64_t *legal_moves, int32_t index) {
    if (index >= 0 && index < BOARD_SQUARES) {
        *legal_moves |= ((uint64_t)1 << index);
    }
}

static inline void safe_unset_bit(uint64_t *legal_moves, int32_t index) {
    if (index >= 0 && index < BOARD_SQUARES) {
        *legal_moves &= ~((uint64_t)1 << index);
    }
}

static inline void apply_moves_mask(uint32_t *game_state, uint64_t *legal_moves, uint64_t mask, uint16_t color) {
    for (int i = 0; i < BOARD_SQUARES; i++) {
        if ((get_piece(game_state, i) & COLOR_BIT_MASK) == color && get_piece(game_state, i) != WHITE_KING &&
            get_piece(game_state, i) != BLACK_KING) {
            legal_moves[i] &= mask;
        }
    }
}

int16_t get_piece(uint32_t *game_state, uint16_t loc) {
    if (loc < 0 || loc >= BOARD_SQUARES) {
        return INVALID_PIECE;
    }
    uint16_t row = loc / BOARD_ROWS;
    uint16_t pos = (BOARD_COLS - 1 - loc % BOARD_COLS) * SIZE_BITS;

    return (game_state[row] & (PIECE_BIT_MASK << pos)) >> pos;
}

int16_t piece_in_direction(uint32_t *game_state, int32_t loc, Direction direction) {
    do {
        loc += direction;
        if (loc < 0 || loc >= BOARD_SQUARES) {
            return EMPTY_DIRECTION;
        } else if (loc % BOARD_COLS == FIRST_COLUMN &&
                   (direction == EAST || direction == NORTH_EAST || direction == SOUTH_EAST)) {
            return EMPTY_DIRECTION;
        } else if (loc % BOARD_COLS == LAST_COLUMN &&
                   (direction == WEST || direction == NORTH_WEST || direction == SOUTH_WEST)) {
            return EMPTY_DIRECTION;
        } else if (loc / BOARD_ROWS == FIRST_ROW &&
                   (direction == NORTH || direction == NORTH_EAST || direction == NORTH_WEST)) {
            return EMPTY_DIRECTION;
        } else if (loc / BOARD_ROWS == LAST_ROW &&
                   (direction == SOUTH || direction == SOUTH_EAST || direction == SOUTH_WEST)) {
            return EMPTY_DIRECTION;
        }
    } while (!get_piece(game_state, loc));
    return loc;
}

uint32_t check_game_condition(uint16_t *game_condition, uint16_t piece, int32_t location) {
    switch (piece) {
    case WHITE_ROOK:
        if (location == LEFT_WHITE_ROOK) {
            return *game_condition & (BIT_MASK << WHITE_ROOK_BIT_POS);
        } else if (location == RIGHT_WHITE_ROOK) {
            return *game_condition & (BIT_MASK << (WHITE_ROOK_BIT_POS + 1));
        }
        break;
    case BLACK_ROOK:
        if (location == LEFT_BLACK_ROOK) {
            return *game_condition & (BIT_MASK << BLACK_ROOK_BIT_POS);
        } else if (location == RIGHT_BLACK_ROOK) {
            return *game_condition & (BIT_MASK << (BLACK_ROOK_BIT_POS + 1));
        }
        break;
    case WHITE_KING:
        if (location == WHITE_KING_POS) {
            return *game_condition & (BIT_MASK << WHITE_KING_BIT_POS);
        }
        break;
    case BLACK_KING:
        if (location == BLACK_KING_POS) {
            return *game_condition & (BIT_MASK << BLACK_KING_BIT_POS);
        }
        break;
    }
    return 0;
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

uint16_t check_castles_king_side(uint32_t *game_state, uint16_t *game_condition, uint16_t color) {
    if (color == WHITE) {
        if (!check_game_condition(game_condition, WHITE_KING, WHITE_KING_POS) &&
            !check_game_condition(game_condition, WHITE_ROOK, RIGHT_WHITE_ROOK)) {
            if (get_piece(game_state, piece_in_direction(game_state, WHITE_KING_POS, EAST)) == WHITE_ROOK) {
                return TRUE;
            }
        }
    } else {
        if (!check_game_condition(game_condition, BLACK_KING, BLACK_KING_POS) &&
            !check_game_condition(game_condition, BLACK_ROOK, RIGHT_BLACK_ROOK)) {
            if (get_piece(game_state, piece_in_direction(game_state, BLACK_KING_POS, EAST)) == BLACK_ROOK) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

uint16_t check_castles_queen_side(uint32_t *game_state, uint16_t *game_condition, uint16_t color) {
    if (color == WHITE) {
        if (!check_game_condition(game_condition, WHITE_KING, WHITE_KING_POS) &&
            !check_game_condition(game_condition, WHITE_ROOK, LEFT_WHITE_ROOK)) {
            if (get_piece(game_state, piece_in_direction(game_state, WHITE_KING_POS, WEST)) == WHITE_ROOK) {
                return TRUE;
            }
        }
    } else {
        if (!check_game_condition(game_condition, BLACK_KING, BLACK_KING_POS) &&
            !check_game_condition(game_condition, BLACK_ROOK, LEFT_BLACK_ROOK)) {
            if (get_piece(game_state, piece_in_direction(game_state, BLACK_KING_POS, WEST)) == BLACK_ROOK) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

static inline void set_pawn_attacked_square(uint64_t *attacked_squares, uint16_t loc, uint16_t color) {
    switch (color) {
    case (WHITE):
        if (loc % BOARD_COLS != LAST_COLUMN) {
            safe_set_bit(attacked_squares, loc + SOUTH_EAST);
        }
        if (loc % BOARD_COLS != FIRST_COLUMN) {
            safe_set_bit(attacked_squares, loc + SOUTH_WEST);
        }
        break;
    case (BLACK):
        if (loc % BOARD_COLS != LAST_COLUMN) {
            safe_set_bit(attacked_squares, loc + NORTH_EAST);
        }
        if (loc % BOARD_COLS != FIRST_COLUMN) {
            safe_set_bit(attacked_squares, loc + NORTH_WEST);
        }
        break;
    }
}

uint64_t get_attacked_squares(uint32_t *game_state, uint64_t *legal_moves, uint16_t color) {
    uint64_t attacked_squares = (uint64_t)0;

    int16_t current_piece;
    for (int piece = 0; piece < BOARD_SQUARES; piece++) {
        current_piece = get_piece(game_state, piece);

        if ((current_piece & COLOR_BIT_MASK) == color || current_piece == WHITE_EMPTY || current_piece == BLACK_EMPTY) {
            continue;
        } else if (current_piece == WHITE_PAWN || current_piece == BLACK_PAWN) {
            set_pawn_attacked_square(&attacked_squares, piece, color);
            continue;
        }

        for (int square = 0; square < BOARD_SQUARES; square++) {
            if (legal_moves[piece] & ((uint64_t)1 << square)) {
                safe_set_bit(&attacked_squares, square);
            }
        }
    }
    return attacked_squares;
}

void get_checked_pieces(uint32_t *game_state, uint16_t loc, int16_t *pieces) {
    if (get_piece(game_state, loc) != WHITE_KING && get_piece(game_state, loc) != BLACK_KING) {
        return;
    }
    uint16_t index = 0;
    uint16_t color = get_piece(game_state, loc) & COLOR_BIT_MASK;

    int16_t piece_location;
    int16_t piece;
    for (int i = 0; i < 4; i++) {
        piece_location = piece_in_direction(game_state, loc, bishop_directions[i]);
        piece = get_piece(game_state, piece_location);
        if (piece == WHITE_EMPTY || piece == BLACK_EMPTY || piece == INVALID_PIECE ||
            (piece & COLOR_BIT_MASK) == color) {
            continue;
        }
        if (piece == WHITE_BISHOP || piece == BLACK_BISHOP || piece == WHITE_QUEEN || piece == BLACK_QUEEN) {
            pieces[index] = piece_location;
            index++;
        }
    }

    for (int i = 0; i < 4; i++) {
        piece_location = piece_in_direction(game_state, loc, rook_directions[i]);
        piece = get_piece(game_state, piece_location);
        if (piece == WHITE_EMPTY || piece == BLACK_EMPTY || piece == INVALID_PIECE ||
            (piece & COLOR_BIT_MASK) == color) {
            continue;
        }
        if (piece == WHITE_ROOK || piece == BLACK_ROOK || piece == WHITE_QUEEN || piece == BLACK_QUEEN) {
            pieces[index] = piece_location;
            index++;
        }
    }

    for (int i = 0; i < 8; i++) {
        piece = get_piece(game_state, loc + knight_offsets[i]);
        if (piece == WHITE_EMPTY || piece == BLACK_EMPTY || piece == INVALID_PIECE) {
            continue;
        }
        if ((piece & COLOR_BIT_MASK) != color && (piece == WHITE_KNIGHT || piece == BLACK_KNIGHT)) {
            pieces[index] = loc + knight_offsets[i];
            index++;
        }
    }

    if (color == WHITE && get_piece(game_state, loc + NORTH_EAST) == BLACK_PAWN && loc % BOARD_COLS != LAST_COLUMN) {
        pieces[index] = loc + NORTH_EAST;
        index++;
    }
    if (color == WHITE && get_piece(game_state, loc + NORTH_WEST) == BLACK_PAWN && loc % BOARD_COLS != FIRST_COLUMN) {
        pieces[index] = loc + NORTH_WEST;
        index++;
    }
    if (color == BLACK && get_piece(game_state, loc + SOUTH_EAST) == WHITE_PAWN && loc % BOARD_COLS != LAST_COLUMN) {
        pieces[index] = loc + SOUTH_EAST;
        index++;
    }
    if (color == BLACK && get_piece(game_state, loc + SOUTH_WEST) == WHITE_PAWN && loc % BOARD_COLS != FIRST_COLUMN) {
        pieces[index] = loc + SOUTH_WEST;
        index++;
    }
}

void get_white_pawn_legal_moves(uint32_t *game_state, uint16_t loc, uint64_t *legal_moves, uint32_t *prev_game_state) {
    int16_t current_piece = get_piece(game_state, loc);

    if ((get_piece(game_state, loc + NORTH_WEST) & COLOR_BIT_MASK) == BLACK &&
        get_piece(game_state, loc + NORTH_WEST) != BLACK_EMPTY && (loc % BOARD_COLS) != FIRST_COLUMN) {
        safe_set_bit(legal_moves, loc + NORTH_WEST);
    }
    if ((get_piece(game_state, loc + NORTH_EAST) & COLOR_BIT_MASK) == BLACK &&
        get_piece(game_state, loc + NORTH_EAST) != BLACK_EMPTY && (loc % BOARD_COLS) != LAST_COLUMN) {
        safe_set_bit(legal_moves, loc + NORTH_EAST);
    }
    if (get_piece(game_state, loc + NORTH) == WHITE_EMPTY || get_piece(game_state, loc + NORTH) == BLACK_EMPTY) {
        safe_set_bit(legal_moves, loc + NORTH);
        if ((get_piece(game_state, loc + 2 * NORTH) == WHITE_EMPTY ||
             get_piece(game_state, loc + 2 * NORTH) == BLACK_EMPTY) &&
            !check_if_pawn_moved(WHITE_PAWN, loc)) {
            safe_set_bit(legal_moves, loc + 2 * NORTH);
        }
    }

    if (loc / BOARD_ROWS == WHITE_EN_PASSANT_ROW && get_piece(game_state, loc + EAST) == BLACK_PAWN &&
        get_piece(prev_game_state, loc + EAST + 2 * NORTH) == BLACK_PAWN) {
        safe_set_bit(legal_moves, loc + NORTH_EAST);
    }
    if (loc / BOARD_ROWS == WHITE_EN_PASSANT_ROW && get_piece(game_state, loc + WEST) == BLACK_PAWN &&
        get_piece(prev_game_state, loc + WEST + 2 * NORTH) == BLACK_PAWN) {
        safe_set_bit(legal_moves, loc + NORTH_WEST);
    }
}

void get_black_pawn_legal_moves(uint32_t *game_state, uint16_t loc, uint64_t *legal_moves, uint32_t *prev_game_state) {
    int16_t current_piece = get_piece(game_state, loc);

    if ((get_piece(game_state, loc + SOUTH_WEST) & COLOR_BIT_MASK) == WHITE &&
        get_piece(game_state, loc + SOUTH_WEST) != WHITE_EMPTY && (loc % BOARD_COLS) != FIRST_COLUMN) {
        safe_set_bit(legal_moves, loc + SOUTH_WEST);
    }
    if ((get_piece(game_state, loc + SOUTH_EAST) & COLOR_BIT_MASK) == WHITE &&
        get_piece(game_state, loc + SOUTH_EAST) != WHITE_EMPTY && (loc % BOARD_COLS) != LAST_COLUMN) {
        safe_set_bit(legal_moves, loc + SOUTH_EAST);
    }
    if (get_piece(game_state, loc + SOUTH) == WHITE_EMPTY || get_piece(game_state, loc + SOUTH) == BLACK_EMPTY) {
        safe_set_bit(legal_moves, loc + SOUTH);
        if ((get_piece(game_state, loc + 2 * SOUTH) == WHITE_EMPTY ||
             get_piece(game_state, loc + 2 * SOUTH) == BLACK_EMPTY) &&
            !check_if_pawn_moved(BLACK_PAWN, loc)) {
            safe_set_bit(legal_moves, loc + 2 * SOUTH);
        }
    }

    if (loc / BOARD_ROWS == BLACK_EN_PASSANT_ROW && get_piece(game_state, loc + EAST) == WHITE_PAWN &&
        get_piece(prev_game_state, loc + EAST + 2 * SOUTH) == WHITE_PAWN) {
        safe_set_bit(legal_moves, loc + SOUTH_EAST);
    }
    if (loc / BOARD_ROWS == BLACK_EN_PASSANT_ROW && get_piece(game_state, loc + WEST) == WHITE_PAWN &&
        get_piece(prev_game_state, loc + WEST + 2 * SOUTH) == WHITE_PAWN) {
        safe_set_bit(legal_moves, loc + SOUTH_WEST);
    }
}

void get_knight_legal_moves(uint32_t *game_state, uint16_t loc, uint64_t *legal_moves) {
    int16_t current_piece = get_piece(game_state, loc);

    for (int i = 0; i < 8; i++) {
        if (loc + knight_offsets[i] < 0 || loc + knight_offsets[i] >= BOARD_SQUARES)
            continue;

        int src_col = loc % BOARD_COLS;
        int dst_col = (loc + knight_offsets[i]) % BOARD_COLS;
        int col_diff = abs(src_col - dst_col);
        if (col_diff != 1 && col_diff != 2)
            continue;

        safe_set_bit(legal_moves, loc + knight_offsets[i]);
    }
}

void get_bishop_legal_moves(uint32_t *game_state, uint16_t loc, uint64_t *legal_moves) {
    int16_t current_piece = get_piece(game_state, loc);

    for (int i = 0; i < 4; i++) {
        int pos = loc + bishop_directions[i];
        while (pos >= 0 && pos < BOARD_SQUARES) {
            int col_diff = abs((pos % BOARD_COLS) - (loc % BOARD_COLS));
            int row_diff = abs((pos / BOARD_ROWS) - (loc / BOARD_ROWS));
            if (col_diff != row_diff)
                break;

            int16_t piece = get_piece(game_state, pos);
            if (piece == WHITE_EMPTY || piece == BLACK_EMPTY) {
                safe_set_bit(legal_moves, pos);
            } else {
                safe_set_bit(legal_moves, pos);

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
        while (pos >= 0 && pos < BOARD_SQUARES) {
            int src_row = loc / BOARD_ROWS, dst_row = pos / BOARD_COLS;
            if ((rook_directions[i] == EAST || rook_directions[i] == WEST) && src_row != dst_row)
                break;

            int16_t piece = get_piece(game_state, pos);
            if (piece == WHITE_EMPTY || piece == BLACK_EMPTY) {
                safe_set_bit(legal_moves, pos);
            } else {
                safe_set_bit(legal_moves, pos);
                break;
            }
            pos += rook_directions[i];
        }
    }
}

void get_king_legal_moves(uint32_t *game_state, uint16_t *game_condition, uint16_t loc, uint64_t *legal_moves) {
    int16_t current_piece = get_piece(game_state, loc);

    for (int i = 0; i < 8; i++) {
        int dest = loc + all_directions[i];
        if (dest < 0 || dest >= BOARD_SQUARES) {
            continue;
        }

        int src_row = loc / BOARD_ROWS;
        int dst_row = dest / BOARD_ROWS;
        if (abs((dest % BOARD_COLS) - (loc % BOARD_COLS)) > 1 || abs(dst_row - src_row) > 1) {
            continue;
        }

        safe_set_bit(legal_moves, dest);
    }
    if (check_castles_king_side(game_state, game_condition, current_piece & COLOR_BIT_MASK)) {
        safe_set_bit(legal_moves, loc + 2 * EAST);
    }
    if (check_castles_queen_side(game_state, game_condition, current_piece & COLOR_BIT_MASK)) {
        safe_set_bit(legal_moves, loc + 2 * WEST);
    }
}

static uint64_t get_check_mask(uint32_t *game_state, uint64_t *legal_moves, uint16_t king, uint16_t check_pos) {
    uint64_t mask = (uint64_t)0;

    int16_t checking_piece = get_piece(game_state, check_pos);
    if (checking_piece == WHITE_KNIGHT || checking_piece == BLACK_KNIGHT) {
        safe_set_bit(&mask, check_pos);
        return mask;
    }

    int16_t direction;
    uint16_t distance = 0;

    if (king % BOARD_COLS == check_pos % BOARD_COLS) {
        if (king > check_pos) {
            direction = NORTH;
            safe_unset_bit(&legal_moves[king], king + SOUTH);
        } else {
            direction = SOUTH;
            safe_unset_bit(&legal_moves[king], king + NORTH);
        }
        distance = abs(king / BOARD_ROWS - check_pos / BOARD_ROWS);
    } else if (king / BOARD_ROWS == check_pos / BOARD_ROWS) {
        if (king > check_pos) {
            direction = WEST;
            safe_unset_bit(&legal_moves[king], king + EAST);
        } else {
            direction = EAST;
            safe_unset_bit(&legal_moves[king], king + WEST);
        }
        distance = abs(king - check_pos);
    } else if (king > check_pos) {
        if (king % BOARD_COLS > check_pos % BOARD_COLS) {
            direction = NORTH_WEST;
            if (checking_piece != WHITE_PAWN && checking_piece != BLACK_PAWN) {
                safe_unset_bit(&legal_moves[king], king + SOUTH_EAST);
            }
        } else {
            direction = NORTH_EAST;
            if (checking_piece != WHITE_PAWN && checking_piece != BLACK_PAWN) {
                safe_unset_bit(&legal_moves[king], king + SOUTH_WEST);
            }
        }
        distance = abs(king / BOARD_ROWS - check_pos / BOARD_ROWS);
    } else {
        if (king % BOARD_COLS > check_pos % BOARD_COLS) {
            direction = SOUTH_WEST;
            if (checking_piece != WHITE_PAWN && checking_piece != BLACK_PAWN) {
                safe_unset_bit(&legal_moves[king], king + NORTH_EAST);
            }
        } else {
            direction = SOUTH_EAST;
            if (checking_piece != WHITE_PAWN && checking_piece != BLACK_PAWN) {
                safe_unset_bit(&legal_moves[king], king + NORTH_WEST);
            }
        }
        distance = abs(king / BOARD_ROWS - check_pos / BOARD_ROWS);
    }

    for (int i = 1; i <= distance; i++) {
        safe_set_bit(&mask, king + i * direction);
    }
    return mask;
}

static inline uint64_t get_direction_mask(uint16_t start, uint16_t end, Direction direction) {
    uint64_t mask = (uint64_t)0;

    int start_row = start / BOARD_COLS;
    int start_col = start % BOARD_COLS;

    int end_row = end / BOARD_COLS;
    int end_col = end % BOARD_COLS;

    int delta_row = 0;
    int delta_col = 0;

    switch (direction) {
    case NORTH:
        delta_row = -1;
        delta_col = 0;
        break;
    case SOUTH:
        delta_row = 1;
        delta_col = 0;
        break;
    case EAST:
        delta_row = 0;
        delta_col = 1;
        break;
    case WEST:
        delta_row = 0;
        delta_col = -1;
        break;
    case NORTH_EAST:
        delta_row = -1;
        delta_col = 1;
        break;
    case NORTH_WEST:
        delta_row = -1;
        delta_col = -1;
        break;
    case SOUTH_EAST:
        delta_row = 1;
        delta_col = 1;
        break;
    case SOUTH_WEST:
        delta_row = 1;
        delta_col = -1;
        break;
    }

    int r = start_row + delta_row;
    int c = start_col + delta_col;
    while (r >= 0 && r < BOARD_ROWS && c >= 0 && c < BOARD_COLS) {
        safe_set_bit(&mask, r * BOARD_COLS + c);

        if (r * BOARD_COLS + c == end) {
            break;
        }

        r += delta_row;
        c += delta_col;
    }

    return mask;
}

static void check_pin(uint32_t *game_state, uint64_t *legal_moves, int16_t loc, Direction direction, uint16_t color) {
    int16_t piece_location = piece_in_direction(game_state, loc, direction);

    if ((get_piece(game_state, piece_location) & COLOR_BIT_MASK) == color) {
        int16_t next_location = piece_in_direction(game_state, piece_location, direction);
        int16_t next_piece = get_piece(game_state, next_location);

        if ((next_piece & COLOR_BIT_MASK) != color) {
            if (next_piece == WHITE_QUEEN || next_piece == BLACK_QUEEN) {
                uint64_t pin_mask = get_direction_mask(loc, next_location, direction);
                legal_moves[piece_location] &= pin_mask;
            } else if ((next_piece == WHITE_ROOK || next_piece == BLACK_ROOK) &&
                       (direction == NORTH || direction == EAST || direction == SOUTH || direction == WEST)) {
                uint64_t pin_mask = get_direction_mask(loc, next_location, direction);
                legal_moves[piece_location] &= pin_mask;
            } else if ((next_piece == WHITE_BISHOP || next_piece == BLACK_BISHOP) &&
                       (direction == NORTH_EAST || direction == SOUTH_EAST || direction == SOUTH_WEST ||
                        direction == NORTH_WEST)) {
                uint64_t pin_mask = get_direction_mask(loc, next_location, direction);
                legal_moves[piece_location] &= pin_mask;
            }
        }
    }
}

void do_checks(uint32_t *game_state, uint64_t *legal_moves, uint16_t color) {
    int16_t current_piece;

    for (int loc = 0; loc < BOARD_SQUARES; loc++) {
        current_piece = get_piece(game_state, loc);
        if ((current_piece & COLOR_BIT_MASK) == color && (current_piece == WHITE_KING || current_piece == BLACK_KING)) {
            uint64_t attacked_squares = get_attacked_squares(game_state, legal_moves, color);
            int16_t checked_pieces[MAX_CHECKED_PIECES] = {INVALID_PIECE, INVALID_PIECE};
            get_checked_pieces(game_state, loc, checked_pieces);

            for (int i = 0; i < 8; i++) {
                if (loc + all_directions[i] >= 0 && loc + all_directions[i] < BOARD_SQUARES) {
                    uint64_t bit_mask = (uint64_t)1 << (loc + all_directions[i]);
                    if (legal_moves[loc] & bit_mask && attacked_squares & bit_mask) {
                        legal_moves[loc] ^= bit_mask;
                    }
                }
                check_pin(game_state, legal_moves, loc, all_directions[i], color);
            }

            if ((legal_moves[loc] & ((uint64_t)1 << (loc + 2 * EAST))) &&
                (loc == WHITE_KING_POS || loc == BLACK_KING_POS)) {
                if (checked_pieces[FIRST_CHECKED_PIECE] != INVALID_PIECE ||
                    color == WHITE && attacked_squares & white_castles_king_mask ||
                    color == BLACK && attacked_squares & black_castles_king_mask) {
                    legal_moves[loc] ^= (uint64_t)1 << (loc + 2 * EAST);
                }
            }

            if ((legal_moves[loc] & ((uint64_t)1 << (loc + 2 * WEST))) &&
                (loc == WHITE_KING_POS || loc == BLACK_KING_POS)) {
                if (checked_pieces[FIRST_CHECKED_PIECE] != INVALID_PIECE ||
                    color == WHITE && attacked_squares & white_castles_queen_mask ||
                    color == BLACK && attacked_squares & black_castles_queen_mask) {
                    legal_moves[loc] ^= (uint64_t)1 << (loc + 2 * WEST);
                }
            }

            if (checked_pieces[FIRST_CHECKED_PIECE] == INVALID_PIECE) {
                return;
            } else if (checked_pieces[FIRST_CHECKED_PIECE] != INVALID_PIECE &&
                       checked_pieces[SECOND_CHECKED_PIECE] == INVALID_PIECE) {
                uint64_t check_mask = get_check_mask(game_state, legal_moves, loc, checked_pieces[FIRST_CHECKED_PIECE]);
                apply_moves_mask(game_state, legal_moves, check_mask, color);
            } else {
                apply_moves_mask(game_state, legal_moves, (uint64_t)0, color);
            }
            break;
        }
    }
}
