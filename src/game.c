#include "game.h"

static void print_bits_long(uint64_t num) {
    printf("------------\n");
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%d ", (num & (uint64_t)1 << (8 * i + j)) >> (8 * i + j));
        }
        printf("\n");
    }
    printf("------------\n");
}

int16_t get_piece(uint32_t *game_state, uint16_t loc) {
    if (loc < 0 || loc > 63) {
        return INVALID_PIECE;
    }
    uint16_t row = loc / BOARD_ROWS;
    uint16_t pos = (BOARD_COLS - 1 - loc % BOARD_COLS) * SIZE_BITS;

    return (game_state[row] & (PIECE_BIT_MASK << pos)) >> pos;
}

void move_piece(uint32_t *game_state, uint16_t loc, uint16_t dest) {
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

void do_castles(uint32_t *game_state, uint16_t loc, uint16_t dest) {
    switch (dest) {
    case BLACK_KING_POS + 2 * WEST:
        move_piece(game_state, loc, 2);
        move_piece(game_state, LEFT_BLACK_ROOK, 3);
        break;
    case BLACK_KING_POS + 2 * EAST:
        move_piece(game_state, loc, 6);
        move_piece(game_state, RIGHT_BLACK_ROOK, 5);
        break;
    case WHITE_KING_POS + 2 * WEST:
        move_piece(game_state, loc, 58);
        move_piece(game_state, LEFT_WHITE_ROOK, 59);
        break;
    case WHITE_KING_POS + 2 * EAST:
        move_piece(game_state, loc, 62);
        move_piece(game_state, RIGHT_WHITE_ROOK, 61);
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
            *game_condition |= (BIT_MASK << WHITE_ROOK_BIT_POS + 1);
        }
        break;
    case BLACK_ROOK:
        if (location == LEFT_BLACK_ROOK) {
            *game_condition |= (BIT_MASK << BLACK_ROOK_BIT_POS);
        } else if (location == RIGHT_BLACK_ROOK) {
            *game_condition |= (BIT_MASK << BLACK_ROOK_BIT_POS + 1);
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
        if (loc < 0 || loc > 63) {
            return EMPTY_DIRECTION;
        }
    } while (!get_piece(game_state, loc));
    return loc;
}

uint64_t get_legal_moves(uint32_t *game_state, uint16_t *game_condition, uint16_t loc) {
    uint64_t legal_moves = 0;

    switch (get_piece(game_state, loc)) {
    case WHITE_EMPTY:
    case BLACK_EMPTY:
        return legal_moves;
    case WHITE_PAWN:
        get_white_pawn_legal_moves(game_state, loc, &legal_moves);
        break;
    case BLACK_PAWN:
        get_black_pawn_legal_moves(game_state, loc, &legal_moves);
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

void update_legal_moves(uint32_t *game_state, uint16_t *game_condition, uint64_t *legal_moves) {
    for (int32_t i = 0; i < BOARD_SQUARES; i++) {
        legal_moves[i] = get_legal_moves(game_state, game_condition, i);
    }
    do_checks(game_state, legal_moves, WHITE);
    do_checks(game_state, legal_moves, BLACK);
}

uint16_t check_valid_move(uint32_t *game_state, uint16_t *game_condition, uint64_t *legal_moves, int32_t loc, int32_t dest) {
    if (legal_moves[loc] & ((uint64_t)1 << dest)) {
        return VALID_MOVE;
    }
    // else if (get_piece(game_state, loc) == WHITE_KING && get_piece(game_state, dest) == WHITE_ROOK) {
    //     if (check_castles_king_side(game_state, game_condition, WHITE) || check_castles_queen_side(game_state, game_condition, WHITE)) {
    //         return VALID_MOVE;
    //     }
    // } else if (get_piece(game_state, loc) == BLACK_KING && get_piece(game_state, dest) == BLACK_ROOK) {
    //     if (check_castles_king_side(game_state, game_condition, BLACK) || check_castles_queen_side(game_state, game_condition, BLACK)) {
    //         return VALID_MOVE;
    //     }
    // }

    // int16_t piece_at_dest = get_piece(game_state, dest);
    // if (piece_at_dest != WHITE_EMPTY && piece_at_dest != BLACK_EMPTY && (piece & COLOR_BIT_MASK) == (piece_at_dest & COLOR_BIT_MASK)) {
    //     return INVALID_MOVE;
    // }

    return INVALID_MOVE;
}

uint16_t make_move(uint32_t *game_state, uint16_t *game_condition, uint64_t *legal_moves, int32_t *selected_square, int32_t *clicked_square) {
    int16_t clicked_piece = get_piece(game_state, *clicked_square);

    if (*selected_square == -1) {
        if (clicked_piece != WHITE_EMPTY && clicked_piece != BLACK_EMPTY) {
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
        if (selected_piece == WHITE_KING && abs(*clicked_square - *selected_square) == 2 || selected_piece == BLACK_KING && abs(*clicked_square - *selected_square) == 2) {
            do_castles(game_state, *selected_square, *clicked_square);
        } else {
            move_piece(game_state, *selected_square, *clicked_square);
        }

        if (selected_piece == WHITE_ROOK || selected_piece == BLACK_ROOK || selected_piece == WHITE_KING || selected_piece == BLACK_KING) {
            set_piece_moved(game_condition, selected_piece, *selected_square);
        }

        // printf("%d\n", *game_condition);

        update_legal_moves(game_state, game_condition, legal_moves);
        // for (int i = 0; i < 64; i++) {
        //     print_bits_long(legal_moves[i]);
        // }

        *selected_square = -1;
        return MOVE_SUCCESS;
    } else {
        *selected_square = -1;
        return MOVE_FAIL;
    }
}