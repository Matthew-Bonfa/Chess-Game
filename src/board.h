#ifndef BOARD_H
#define BOARD_H

#include "constants.h"
#include "render.h"
#include "utils.h"

Shape *init_board(GLFWwindow *window);
void clear_board(Shape *board);
void draw_board(Shape *board);

#endif