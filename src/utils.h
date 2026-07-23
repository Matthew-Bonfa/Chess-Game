#ifndef UTILS_H
#define UTILS_H

#include "constants.h"
#include "game.h"
#include <GLFW/glfw3.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct color {
    float r, g, b;
} color_t;

char *load_shader_source(const char *filename);
void convert_to_px(GLFWwindow *window, float *x, float *y);
void convert_to_ndc(GLFWwindow *window, float *x, float *y);
void hex_to_rgbf(const char *hex, color_t *color);
void get_square_coordinates_px(GLFWwindow *window, unsigned int x, unsigned int y, float size, float *vertices,
                               unsigned int *indices);
int get_board_size(GLFWwindow *window, float *x_shift, float *y_shift);
float get_square_size(GLFWwindow *window, float *x_shift, float *y_shift);
void clear_game_state(uint32_t *game_state);
uint8_t char_to_piece(char c);
void load_fen_position(const char *fen, uint32_t *game_state);
void print_location(int16_t loc);
void print_bits_long(uint64_t num);
void print_board(uint32_t *game_state);

#endif
