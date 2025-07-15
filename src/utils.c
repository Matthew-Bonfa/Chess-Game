#include "utils.h"

char cols[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
char rows[] = {'8', '7', '6', '5', '4', '3', '2', '1'};

const uint32_t square_indices[SQUARE_INDICES] = {
    0, 1, 2,
    2, 3, 0};

char *load_shader_source(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open shader file: %s\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    int64_t length = ftell(file);
    rewind(file);

    char *buffer = (char *)malloc(length + 1);
    if (!buffer) {
        fprintf(stderr, "Failed to allocate memory for shader: %s\n", filename);
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);
    return buffer;
}

void convert_to_px(GLFWwindow *window, float *x, float *y) {
    int32_t window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);

    *x = (*x + 1.0f) * 0.5f * window_width;
    *y = (*y + 1.0f) * 0.5f * window_height;
    *y = window_height - *y;
}

void convert_to_ndc(GLFWwindow *window, float *x, float *y) {
    int32_t window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);

    *x = (*x / window_width) * 2.0f - 1.0f;
    *y = 1.0f - (*y / window_height) * 2.0f;
}

void hex_to_rgbf(const char *hex, color_t *color) {
    if (hex[0] == '#')
        hex++;

    if (strlen(hex) != 6) {
        fprintf(stderr, "Invalid hex color format: %s\n", hex);
        color->r = color->g = color->b = 0.0f;
        return;
    }

    char rs[3] = {hex[0], hex[1], '\0'};
    char gs[3] = {hex[2], hex[3], '\0'};
    char bs[3] = {hex[4], hex[5], '\0'};

    int32_t ri = (int32_t)strtol(rs, NULL, 16);
    int32_t gi = (int32_t)strtol(gs, NULL, 16);
    int32_t bi = (int32_t)strtol(bs, NULL, 16);

    color->r = ri / 255.0f;
    color->g = gi / 255.0f;
    color->b = bi / 255.0f;
}

void get_square_coordinates_px(GLFWwindow *window, uint32_t x, uint32_t y, float size, float *vertices, uint32_t *indices) {
    float square[SQUARE_EDGES] = {
        x, y,
        x + size, y,
        x + size, y + size,
        x, y + size};

    for (int32_t i = 0; i < SQUARE_EDGES / 2; ++i) {
        convert_to_ndc(window, square + i * 2, square + i * 2 + 1);
    }

    for (int32_t i = 0; i < SQUARE_EDGES; i++) {
        vertices[i] = square[i];
    }

    for (int32_t i = 0; i < SQUARE_INDICES; i++) {
        indices[i] = square_indices[i];
    }
}

int32_t get_board_size(GLFWwindow *window, float *x_shift, float *y_shift) {
    int32_t window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);

    if (window_width > window_height) {
        *x_shift = (float)(window_width - window_height) / 2;
        *y_shift = 0;
        return window_height;
    }
    *x_shift = 0;
    *y_shift = (float)(window_height - window_width) / 2;
    return window_width;
}

float get_square_size(GLFWwindow *window, float *x_shift, float *y_shift) {
    int32_t board_size = get_board_size(window, x_shift, y_shift);
    return (float)board_size / 8;
}

void clear_game_state(uint32_t *game_state) {
    for (int i = 0; i < BOARD_ROWS; i++) {
        game_state[i] = 0;
    }
}

uint8_t char_to_piece(char c) {
    int is_white = isupper(c);
    char piece = tolower(c);

    switch (piece) {
    case 'p':
        return is_white ? WHITE_PAWN : BLACK_PAWN;
    case 'n':
        return is_white ? WHITE_KNIGHT : BLACK_KNIGHT;
    case 'b':
        return is_white ? WHITE_BISHOP : BLACK_BISHOP;
    case 'r':
        return is_white ? WHITE_ROOK : BLACK_ROOK;
    case 'q':
        return is_white ? WHITE_QUEEN : BLACK_QUEEN;
    case 'k':
        return is_white ? WHITE_KING : BLACK_KING;
    default:
        return WHITE_EMPTY;
    }
}

void load_fen_position(const char *fen, uint32_t *game_state) {
    clear_game_state(game_state);

    int row = 0;
    int col = 0;
    const char *ptr = fen;

    while (*ptr && row < BOARD_ROWS) {
        if (*ptr == '/') {
            row++;
            col = 0;
            ptr++;
            continue;
        }

        if (isdigit(*ptr)) {
            col += *ptr - '0';
            ptr++;
        } else {
            uint8_t piece = char_to_piece(*ptr);
            if (col < BOARD_COLS) {
                int square = row * BOARD_COLS + col;
                set_piece(game_state, square, piece);
                col++;
            }
            ptr++;
        }
    }
}

void print_location(int16_t loc) {
    if (loc == INVALID_PIECE) {
        printf("unselected ");
        return;
    }
    printf("%c%c ", cols[loc % BOARD_COLS], rows[loc / BOARD_ROWS]);
}

void print_bits_long(uint64_t num) {
    printf("------------\n");
    for (int i = 0; i < BOARD_ROWS; i++) {
        for (int j = 0; j < BOARD_COLS; j++) {
            printf("%I64d ", (num & (uint64_t)1 << (BOARD_ROWS * i + j)) >> (BOARD_ROWS * i + j));
        }
        printf("\n");
    }
    printf("------------\n");
}

void print_board(uint32_t *game_state) {
    printf("------------\n");
    for (int i = 0; i < BOARD_ROWS; i++) {
        for (int j = 0; j < BOARD_COLS; j++) {
            printf("%d ", get_piece(game_state, BOARD_ROWS * i + j));
        }
        printf("\n");
    }
    printf("------------\n");
}