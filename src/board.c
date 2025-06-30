#include "board.h"

color_t *primary_color, *secondary_color = NULL;

static Square make_square(GLFWwindow *window, unsigned int x, unsigned int y, float size) {
    float square[SQUARE_EDGES];
    unsigned int square_indices[SQUARE_INDICES];
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

    int square_num = 0;
    for (int c = 0; c < BOARD_COLS; c++) {
        for (int r = 0; r < BOARD_ROWS; r++) {
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

void clear_board(Square *board, Pieces *pieces) {
    for (int i = 0; i < BOARD_SQUARES; i++) {
        destroy_shape(board[i].shape);
    }
    // if (primary_color != NULL) {
    //     free(primary_color);
    // }
    // if (secondary_color != NULL) {
    //     free(secondary_color);
    // }
    if (pieces != NULL) {
        for (int i = 0; i < pieces->length; i++) {
            destroy_image(pieces->pieces[i]);
        }
        if (pieces->pieces != NULL) {
            free(pieces->pieces);
        }
        free(pieces);
    }
    free(board);
}

void draw_board(Square *board) {
    int index = 0;
    for (int r = 0; r < BOARD_ROWS; r++) {
        for (int c = 0; c < BOARD_COLS; c++) {
            if ((r + c) % 2 == 0) {
                draw_shape(&(board[index].shape), secondary_color);
            } else {
                draw_shape(&(board[index].shape), primary_color);
            }
            index++;
        }
    }
}

Pieces *init_pieces(GLFWwindow *window, Square *board, int *game_state) {
    Pieces *pieces = (Pieces *)malloc(sizeof(Pieces));
    Image *images = (Image *)malloc(NUM_STARTING_PIECES * sizeof(Image));

    // printf("%d\n", get_piece(game_state, 7));
    // printf("%d\n", get_piece(game_state, 15));
    // printf("%d\n", get_piece(game_state, 0));
    // printf("%d\n", get_piece(game_state, 8));

    int index = 0;
    for (int i = 0; i < BOARD_SQUARES; i++) {
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
    for (int i = 0; i < pieces->length; i++) {
        draw_image(&pieces->pieces[i]);
    }
}