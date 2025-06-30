#ifndef UTILS_H
#define UTILS_H

#include "constants.h"
#include <GLFW/glfw3.h>
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
void get_square_cooridnates_px(GLFWwindow *window, unsigned int x, unsigned int y, float size, float *vertices, unsigned int *indices);
float get_square_size(GLFWwindow *window, float *x_shift, float *y_shift);

#endif