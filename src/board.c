#include "board.h"

color_t *primary_color, *secondary_color, *primary_selected, *secondary_selected = NULL;

static Square make_square(GLFWwindow *window, uint32_t x, uint32_t y, float size) {
    float square[SQUARE_EDGES];
    uint32_t square_indices[SQUARE_INDICES];
    get_square_cooridnates_px(window, x, y, size, square, square_indices);

    Square new_square;
    new_square.shape = create_shape(square, SQUARE_EDGES, square_indices, SQUARE_INDICES);
    new_square.x = x;
    new_square.y = y;
    new_square.size = size;

    return new_square;
}

Square *init_board(GLFWwindow *window) {
    Square *board = (Square *)malloc(BOARD_SQUARES * sizeof(Square));
    float x_shift, y_shift;
    float square_size = get_square_size(window, &x_shift, &y_shift);

    int32_t square_num = 0;
    for (int32_t c = 0; c < BOARD_COLS; c++) {
        for (int32_t r = 0; r < BOARD_ROWS; r++) {
            board[square_num] = make_square(window, r * square_size + x_shift, c * square_size + y_shift, square_size);
            square_num++;
        }
    }

    if (primary_color == NULL || secondary_color == NULL) {
        primary_color = malloc(sizeof(color_t));
        secondary_color = malloc(sizeof(color_t));
        primary_selected = malloc(sizeof(color_t));
        secondary_selected = malloc(sizeof(color_t));
        hex_to_rgbf(PRIMARY_COLOR, primary_color);
        hex_to_rgbf(SECONDARY_COLOR, secondary_color);
        hex_to_rgbf(PRIMARY_SELECTED, primary_selected);
        hex_to_rgbf(SECONDARY_SELECETED, secondary_selected);
    }

    return board;
}

void draw_board(Square *board, int32_t selected) {
    int32_t index = 0;
    for (int32_t r = 0; r < BOARD_ROWS; r++) {
        for (int32_t c = 0; c < BOARD_COLS; c++) {
            if ((r + c) % 2 == 0) {
                if (index == selected) {
                    draw_shape(&(board[index].shape), secondary_selected);
                } else {
                    draw_shape(&(board[index].shape), secondary_color);
                }
            } else {
                if (index == selected) {
                    draw_shape(&(board[index].shape), primary_selected);
                } else {
                    draw_shape(&(board[index].shape), primary_color);
                }
            }
            index++;
        }
    }
}

Pieces *init_pieces(GLFWwindow *window, Square *board, int32_t *game_state) {
    Pieces *pieces = (Pieces *)malloc(sizeof(Pieces));
    Image *images = (Image *)malloc(NUM_STARTING_PIECES * sizeof(Image));

    int32_t index = 0;
    for (int32_t i = 0; i < BOARD_SQUARES; i++) {
        switch (get_piece(game_state, i)) {
        case WHITE_EMPTY:
            break;
        case WHITE_PAWN:
            images[index] = load_image("./assets/white-pawn.png", window, board[i].x, board[i].y, board[i].size);
            index++;
            break;
        case WHITE_KNIGHT:
            images[index] = load_image("./assets/white-knight.png", window, board[i].x, board[i].y, board[i].size);
            index++;
            break;
        case WHITE_BISHOP:
            images[index] = load_image("./assets/white-bishop.png", window, board[i].x, board[i].y, board[i].size);
            index++;
            break;
        case WHITE_ROOK:
            images[index] = load_image("./assets/white-rook.png", window, board[i].x, board[i].y, board[i].size);
            index++;
            break;
        case WHITE_QUEEN:
            images[index] = load_image("./assets/white-queen.png", window, board[i].x, board[i].y, board[i].size);
            index++;
            break;
        case WHITE_KING:
            images[index] = load_image("./assets/white-king.png", window, board[i].x, board[i].y, board[i].size);
            index++;
            break;
        case BLACK_EMPTY:
            break;
        case BLACK_PAWN:
            images[index] = load_image("./assets/black-pawn.png", window, board[i].x, board[i].y, board[i].size);
            index++;
            break;
        case BLACK_KNIGHT:
            images[index] = load_image("./assets/black-knight.png", window, board[i].x, board[i].y, board[i].size);
            index++;
            break;
        case BLACK_BISHOP:
            images[index] = load_image("./assets/black-bishop.png", window, board[i].x, board[i].y, board[i].size);
            index++;
            break;
        case BLACK_ROOK:
            images[index] = load_image("./assets/black-rook.png", window, board[i].x, board[i].y, board[i].size);
            index++;
            break;
        case BLACK_QUEEN:
            images[index] = load_image("./assets/black-queen.png", window, board[i].x, board[i].y, board[i].size);
            index++;
            break;
        case BLACK_KING:
            images[index] = load_image("./assets/black-king.png", window, board[i].x, board[i].y, board[i].size);
            index++;
            break;
        default:
            break;
        }
    }
    pieces->pieces = images;
    pieces->length = index;
    return pieces;
}

void draw_pieces(Pieces *pieces) {
    for (int32_t i = 0; i < pieces->length; i++) {
        draw_image(&pieces->pieces[i]);
    }
}

Moves *init_dots(GLFWwindow *window, Square *board) {
    Image *dots = (Image *)malloc(BOARD_SQUARES * sizeof(Image));
    Image *rings = (Image *)malloc(BOARD_SQUARES * sizeof(Image));
    Moves *moves = (Moves *)malloc(sizeof(Moves));
    for (int i = 0; i < BOARD_SQUARES; i++) {
        dots[i] = load_image("./assets/dot.png", window, board[i].x, board[i].y, board[i].size);
        rings[i] = load_image("./assets/ring.png", window, board[i].x, board[i].y, board[i].size);
    }
    moves->dots = dots;
    moves->rings = rings;
    return moves;
}

void draw_dots(Moves *moves, uint32_t *game_state, uint64_t *legal_moves, uint16_t loc) {
    int16_t piece = get_piece(game_state, loc);
    for (int i = 0; i < BOARD_SQUARES; i++) {
        if (legal_moves[loc] & (uint64_t)1 << i) {
            if (get_piece(game_state, i) == WHITE_EMPTY || get_piece(game_state, i) == BLACK_EMPTY) {
                draw_image(&(moves->dots[i]));
            } else if ((get_piece(game_state, i) & COLOR_BIT_MASK) != (piece & COLOR_BIT_MASK)) {
                draw_image(&(moves->rings[i]));
            }
        }
    }
}

uint16_t get_square(GLFWwindow *window, float xpos, float ypos) {
    float x_shift, y_shift;
    int32_t board_size = get_board_size(window, &x_shift, &y_shift);

    if (xpos < x_shift || xpos > board_size + x_shift || ypos < y_shift || ypos > board_size + y_shift) {
        return INVALID_SQUARE;
    }

    float square_size = (float)board_size / BOARD_ROWS;

    int32_t row = (int32_t)(ypos - y_shift) / square_size;
    int32_t col = (int32_t)(xpos - x_shift) / square_size;

    return BOARD_ROWS * row + col;
}

void clear_board(Square *board, Pieces *pieces, Moves *dots) {
    for (int32_t i = 0; i < BOARD_SQUARES; i++) {
        destroy_shape(board[i].shape);
    }
    if (pieces != NULL) {
        for (int32_t i = 0; i < pieces->length; i++) {
            destroy_image(pieces->pieces[i]);
        }
        if (pieces->pieces != NULL) {
            free(pieces->pieces);
        }
        free(pieces);
    }
    if (dots != NULL) {
        for (int i = 0; i < BOARD_SQUARES; i++) {
            destroy_image(dots->dots[i]);
            destroy_image(dots->rings[i]);
        }
        free(dots);
    }
    free(board);
}

void free_colors() {
    if (primary_color != NULL) {
        free(primary_color);
    }
    if (secondary_color != NULL) {
        free(secondary_color);
    }
    if (primary_selected != NULL) {
        free(primary_selected);
    }
    if (secondary_selected != NULL) {
        free(secondary_selected);
    }
}