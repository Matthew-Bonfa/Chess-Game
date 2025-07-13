#include "game.h"

int16_t get_piece(uint32_t *game_state, uint16_t loc) {
    if (loc < 0 || loc >= BOARD_SQUARES) {
        return INVALID_PIECE;
    }
    uint16_t row = loc / BOARD_ROWS;
    uint16_t pos = (BOARD_COLS - 1 - loc % BOARD_COLS) * SIZE_BITS;

    return (game_state[row] & (PIECE_BIT_MASK << pos)) >> pos;
}

static void move_piece(uint32_t *game_state, uint16_t loc, uint16_t dest) {
    int32_t piece = (int32_t)get_piece(game_state, loc);
    if (piece == WHITE_EMPTY || piece == BLACK_EMPTY) {
        return;
    }

    uint16_t src_row = loc / BOARD_ROWS;
    uint16_t src_pos = (BOARD_COLS - 1 - loc % BOARD_COLS) * SIZE_BITS;
    game_state[src_row] &= ~((uint32_t)PIECE_BIT_MASK << src_pos);

    uint16_t dest_row = dest / BOARD_ROWS;
    uint16_t dest_pos = (BOARD_COLS - 1 - dest % BOARD_COLS) * SIZE_BITS;
    game_state[dest_row] &= ~((uint32_t)PIECE_BIT_MASK << dest_pos);
    game_state[dest_row] |= (piece << dest_pos);
}

void set_piece(uint32_t *game_state, uint16_t loc, uint16_t piece) {
    uint16_t dest_row = loc / BOARD_ROWS;
    uint16_t dest_pos = (BOARD_COLS - 1 - loc % BOARD_COLS) * SIZE_BITS;
    game_state[dest_row] &= ~((uint32_t)PIECE_BIT_MASK << dest_pos);
    game_state[dest_row] |= (piece << dest_pos);
}

static void do_castles(uint32_t *game_state, uint16_t loc, uint16_t dest) {
    switch (dest) {
    case BLACK_KING_POS + 2 * WEST:
        move_piece(game_state, loc, BLACK_KING_CASTLES_KING_POS);
        move_piece(game_state, LEFT_BLACK_ROOK, BLACK_KING_CASTLES_ROOK_POS);
        break;
    case BLACK_KING_POS + 2 * EAST:
        move_piece(game_state, loc, BLACK_QUEEN_CASTLES_KING_POS);
        move_piece(game_state, RIGHT_BLACK_ROOK, BLACK_QUEEN_CASTLES_ROOK_POS);
        break;
    case WHITE_KING_POS + 2 * WEST:
        move_piece(game_state, loc, WHITE_KING_CASTLES_KING_POS);
        move_piece(game_state, LEFT_WHITE_ROOK, WHITE_KING_CASTLES_ROOK_POS);
        break;
    case WHITE_KING_POS + 2 * EAST:
        move_piece(game_state, loc, WHITE_QUEEN_CASTLES_KING_POS);
        move_piece(game_state, RIGHT_WHITE_ROOK, WHITE_QUEEN_CASTLES_ROOK_POS);
        break;
    }
}

static void do_en_passant(uint32_t *game_state, uint16_t loc, uint16_t dest, uint16_t color) {
    printf("ISDJFIOSD");
    switch (color) {
    case WHITE:
        if (dest == loc + NORTH_WEST) {
            move_piece(game_state, loc + WEST, loc + NORTH_WEST);
        } else if (dest == loc + NORTH_EAST) {
            move_piece(game_state, loc + EAST, loc + NORTH_EAST);
        }
        break;
    case BLACK:
        if (dest == loc + SOUTH_WEST) {
            move_piece(game_state, loc + WEST, loc + SOUTH_WEST);
        } else if (dest == loc + SOUTH_EAST) {
            move_piece(game_state, loc + EAST, loc + SOUTH_EAST);
        }
        break;
    }
    move_piece(game_state, loc, dest);
}

static void do_pawn_promotion(uint32_t *game_state, uint16_t loc, uint16_t dest) {
    char line[100];
    char value;
    printf("Promote to: (q, r, b, k)\n");
    while (1) {
        if (!fgets(line, sizeof(line), stdin)) {
            value = 'q';
            break;
        }
        if (sscanf(line, " %c", &value) == 1) {
            if (value == 'q' || value == 'r' || value == 'b' || value == 'k') {
                break;
            }
        }
        printf("Please enter a valid piece to promote to (q, r, b, k)\n");
    }

    set_piece(game_state, loc, WHITE_EMPTY);
    switch (value) {
    case 'q':
        if (dest / BOARD_ROWS == FIRST_ROW) {
            set_piece(game_state, dest, WHITE_QUEEN);
        } else {
            set_piece(game_state, dest, BLACK_QUEEN);
        }
        break;
    case 'r':
        if (dest / BOARD_ROWS == FIRST_ROW) {
            set_piece(game_state, dest, WHITE_ROOK);
        } else {
            set_piece(game_state, dest, BLACK_ROOK);
        }
        break;
    case 'b':
        if (dest / BOARD_ROWS == FIRST_ROW) {
            set_piece(game_state, dest, WHITE_BISHOP);
        } else {
            set_piece(game_state, dest, BLACK_BISHOP);
        }
        break;
    case 'k':
        if (dest / BOARD_ROWS == FIRST_ROW) {
            set_piece(game_state, dest, WHITE_KNIGHT);
        } else {
            set_piece(game_state, dest, BLACK_KNIGHT);
        }
        break;
    }
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

void set_piece_moved(uint16_t *game_condition, uint16_t piece, int32_t location) {
    switch (piece) {
    case WHITE_ROOK:
        if (location == LEFT_WHITE_ROOK) {
            *game_condition |= (BIT_MASK << WHITE_ROOK_BIT_POS);
        } else if (location == RIGHT_WHITE_ROOK) {
            *game_condition |= (BIT_MASK << (WHITE_ROOK_BIT_POS + 1));
        }
        break;
    case BLACK_ROOK:
        if (location == LEFT_BLACK_ROOK) {
            *game_condition |= (BIT_MASK << BLACK_ROOK_BIT_POS);
        } else if (location == RIGHT_BLACK_ROOK) {
            *game_condition |= (BIT_MASK << (BLACK_ROOK_BIT_POS + 1));
        }
        break;
    case WHITE_KING:
        if (location == WHITE_KING_POS) {
            *game_condition |= (BIT_MASK << WHITE_KING_BIT_POS);
        }
        break;
    case BLACK_KING:
        if (location == BLACK_KING_POS) {
            *game_condition |= (BIT_MASK << BLACK_KING_BIT_POS);
        }
        break;
    }
}

int16_t piece_in_direction(uint32_t *game_state, int32_t loc, Direction direction) {
    do {
        loc += direction;
        if (loc < 0 || loc >= BOARD_SQUARES) {
            return EMPTY_DIRECTION;
        } else if (loc % BOARD_COLS == FIRST_COLUMN && (direction == EAST || direction == NORTH_EAST || direction == SOUTH_EAST)) {
            return EMPTY_DIRECTION;
        } else if (loc % BOARD_COLS == LAST_COLUMN && (direction == WEST || direction == NORTH_WEST || direction == SOUTH_WEST)) {
            return EMPTY_DIRECTION;
        } else if (loc / BOARD_ROWS == FIRST_ROW && (direction == NORTH || direction == NORTH_EAST || direction == NORTH_WEST)) {
            return EMPTY_DIRECTION;
        } else if (loc / BOARD_ROWS == LAST_ROW && (direction == SOUTH || direction == SOUTH_EAST || direction == SOUTH_WEST)) {
            return EMPTY_DIRECTION;
        }
    } while (!get_piece(game_state, loc));
    return loc;
}

static uint64_t get_legal_moves(uint32_t *game_state, uint16_t *game_condition, uint16_t loc, uint32_t *prev_game_state) {
    uint64_t legal_moves = 0;

    switch (get_piece(game_state, loc)) {
    case WHITE_EMPTY:
    case BLACK_EMPTY:
        return legal_moves;
    case WHITE_PAWN:
        get_white_pawn_legal_moves(game_state, loc, &legal_moves, prev_game_state);
        break;
    case BLACK_PAWN:
        get_black_pawn_legal_moves(game_state, loc, &legal_moves, prev_game_state);
        break;
    case WHITE_KNIGHT:
    case BLACK_KNIGHT:
        get_knight_legal_moves(game_state, loc, &legal_moves);
        break;
    case WHITE_BISHOP:
    case BLACK_BISHOP:
        get_bishop_legal_moves(game_state, loc, &legal_moves);
        break;
    case WHITE_ROOK:
    case BLACK_ROOK:
        get_rook_legal_moves(game_state, loc, &legal_moves);
        break;
    case WHITE_QUEEN:
    case BLACK_QUEEN:
        get_bishop_legal_moves(game_state, loc, &legal_moves);
        get_rook_legal_moves(game_state, loc, &legal_moves);
        break;
    case WHITE_KING:
    case BLACK_KING:
        get_king_legal_moves(game_state, game_condition, loc, &legal_moves);
        break;
    }
    return legal_moves;
}

void update_legal_moves(uint32_t *game_state, uint16_t *game_condition, uint64_t *legal_moves, uint32_t *prev_game_state, uint16_t color_to_move) {
    for (int32_t i = 0; i < BOARD_SQUARES; i++) {
        legal_moves[i] = get_legal_moves(game_state, game_condition, i, prev_game_state);
    }

    if (color_to_move == WHITE) {
        do_checks(game_state, legal_moves, BLACK);
    } else {
        do_checks(game_state, legal_moves, WHITE);
    }
}

uint16_t check_valid_move(uint32_t *game_state, uint16_t *game_condition, uint64_t *legal_moves, int32_t loc, int32_t dest) {
    if (legal_moves[loc] & ((uint64_t)1 << dest)) {
        return VALID_MOVE;
    }

    return INVALID_MOVE;
}

uint16_t make_move(uint32_t *game_state, uint16_t *game_condition, uint64_t *legal_moves, int32_t *selected_square, int32_t *clicked_square, uint16_t *color_to_move, uint32_t *prev_game_state) {
    int16_t clicked_piece = get_piece(game_state, *clicked_square);

    if (*selected_square == UNSELECTED) {
        if (clicked_piece != WHITE_EMPTY && clicked_piece != BLACK_EMPTY && (clicked_piece & COLOR_BIT_MASK) == *color_to_move) {
            *selected_square = *clicked_square;
        }
        return MOVE_FAIL;
    }

    int16_t selected_piece = get_piece(game_state, *selected_square);

    if (clicked_piece != WHITE_EMPTY && clicked_piece != BLACK_EMPTY && (selected_piece & COLOR_BIT_MASK) == (clicked_piece & COLOR_BIT_MASK)) {
        *selected_square = *clicked_square;
        return MOVE_FAIL;
    }

    if (check_valid_move(game_state, game_condition, legal_moves, *selected_square, *clicked_square)) {
        memcpy(prev_game_state, game_state, BOARD_ROWS * sizeof(int));
        if (selected_piece == WHITE_KING && abs(*clicked_square - *selected_square) == 2 || selected_piece == BLACK_KING && abs(*clicked_square - *selected_square) == 2) {
            do_castles(game_state, *selected_square, *clicked_square);
        } else if (selected_piece == WHITE_PAWN && (*clicked_square == *selected_square + NORTH_WEST || *clicked_square == *selected_square + NORTH_EAST) && (clicked_piece == WHITE_EMPTY || clicked_piece == BLACK_EMPTY)) {
            do_en_passant(game_state, *selected_square, *clicked_square, WHITE);
        } else if (selected_piece == BLACK_PAWN && (*clicked_square == *selected_square + SOUTH_WEST || *clicked_square == *selected_square + SOUTH_EAST) && (clicked_piece == WHITE_EMPTY || clicked_piece == BLACK_EMPTY)) {
            do_en_passant(game_state, *selected_square, *clicked_square, BLACK);
        } else if (selected_piece == WHITE_PAWN && *selected_square / BOARD_ROWS == FIRST_ROW + 1 && *clicked_square / BOARD_ROWS == FIRST_ROW) {
            do_pawn_promotion(game_state, *selected_square, *clicked_square);
        } else if (selected_piece == BLACK_PAWN && *selected_square / BOARD_ROWS == LAST_ROW - 1 && *clicked_square / BOARD_ROWS == LAST_ROW) {
            do_pawn_promotion(game_state, *selected_square, *clicked_square);
        } else {
            move_piece(game_state, *selected_square, *clicked_square);
        }

        if (selected_piece == WHITE_ROOK || selected_piece == BLACK_ROOK || selected_piece == WHITE_KING || selected_piece == BLACK_KING) {
            set_piece_moved(game_condition, selected_piece, *selected_square);
        }

        update_legal_moves(game_state, game_condition, legal_moves, prev_game_state, *color_to_move);

        *selected_square = UNSELECTED;
        return MOVE_SUCCESS;
    } else {
        *selected_square = UNSELECTED;
        return MOVE_FAIL;
    }
}