#ifndef BOARD_H
#define BOARD_H

#include "constants.h"
#include "game.h"
#include "image.h"
#include "render.h"
#include "utils.h"

typedef struct {
    Shape shape;
    unsigned int x, y;
    float size;
} Square;

typedef struct {
    Image *pieces;
    int length;
} Pieces;

Square *init_board(GLFWwindow *window);
void clear_board(Square *board, Pieces *pieces);
void draw_board(Square *board);
Pieces *init_pieces(GLFWwindow *window, Square *board, int *game_state);
void draw_pieces(Pieces *pieces);

#endif