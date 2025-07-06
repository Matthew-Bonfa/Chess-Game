#ifndef BOARD_H
#define BOARD_H

#include "constants.h"
#include "game.h"
#include "image.h"
#include "render.h"
#include "utils.h"

typedef struct {
    Shape shape;
    uint32_t x, y;
    float size;
} Square;

typedef struct {
    Image *pieces;
    int32_t length;
} Pieces;

Square *init_board(GLFWwindow *window);
void clear_board(Square *board, Pieces *pieces);
void draw_board(Square *board, int32_t selected);
Pieces *init_pieces(GLFWwindow *window, Square *board, int32_t *game_state);
void draw_pieces(Pieces *pieces);
uint16_t get_square(GLFWwindow *window, float xpos, float ypos);

#endif