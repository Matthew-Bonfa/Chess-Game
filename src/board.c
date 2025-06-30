#include "board.h"

color_t *primary_color, *secondary_color = NULL;

static Shape make_square(GLFWwindow *window, unsigned int x, unsigned int y, float size) {
    float square[SQUARE_EDGES];
    unsigned int square_indices[SQUARE_INDICES];
    get_square_cooridnates_px(window, x, y, size, square, square_indices);

    return create_shape(square, SQUARE_EDGES, square_indices, SQUARE_INDICES);
}

Shape *init_board(GLFWwindow *window) {
    Shape *board = (Shape *)malloc(BOARD_SQUARES * sizeof(Shape));
    float x_shift, y_shift;
    float square_size = get_square_size(window, &x_shift, &y_shift);

    int square_num = 0;
    for (int r = 0; r < BOARD_ROWS; r++) {
        for (int c = 0; c < BOARD_COLS; c++) {
            board[square_num] = make_square(window, r * square_size + x_shift, c * square_size + y_shift, square_size);
            square_num++;
        }
    }

    if (primary_color == NULL || secondary_color == NULL) {
        primary_color = malloc(sizeof(color_t));
        secondary_color = malloc(sizeof(color_t));
        hex_to_rgbf(PRIMARY_COLOR, primary_color);
        hex_to_rgbf(SECONDARY_COLOR, secondary_color);
    }

    return board;
}

void clear_board(Shape *board) {
    for (int i = 0; i < BOARD_SQUARES; i++) {
        destroy_shape(board[i]);
    }
    if (primary_color != NULL) {
        free(primary_color);
    }
    if (secondary_color != NULL) {
        free(secondary_color);
    }
}

void draw_board(Shape *board) {
    int index = 0;
    for (int r = 0; r < BOARD_ROWS; r++) {
        for (int c = 0; c < BOARD_COLS; c++) {
            if ((r + c) % 2 == 0) {
                draw_shape(&board[index], secondary_color);
            } else {
                draw_shape(&board[index], primary_color);
            }
            index++;
        }
    }
}