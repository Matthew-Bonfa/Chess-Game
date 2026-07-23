#ifndef BOARD_H
#define BOARD_H

#include "constants.h"
#include "image.h"
#include "render.h"
#include "text.h"

typedef struct {
    Shape shape;
    uint32_t x, y;
    float size;
} Square;

typedef struct {
    Image *pieces;
    int32_t length;
} Pieces;

typedef struct {
    Image *dots;
    Image *rings;
} Moves;

typedef struct {
    TextChar files[BOARD_COLS];
    TextChar ranks[BOARD_ROWS];
    GLuint font_texture;
} Legends;

Square *init_board(GLFWwindow *window);
void draw_board(Square *board, int32_t selected);
Pieces *init_pieces(GLFWwindow *window, Square *board, int32_t *game_state);
void draw_pieces(Pieces *pieces);
Moves *init_dots(GLFWwindow *window, Square *board);
void draw_dots(Moves *dots, uint32_t *game_state, uint64_t *legal_moves, uint16_t loc);
Legends *init_legends(GLFWwindow *window, Square *board);
void draw_legends(Legends *legends);
uint16_t get_square(GLFWwindow *window, float xpos, float ypos);
void clear_board(Square *board, Pieces *pieces, Moves *dots, Legends *legends);
void free_colors();

#endif
