#ifndef RENDER_H
#define RENDER_H

#include <glad/glad.h>

#include "image.h"
#include "utils.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    unsigned int index_count;
} Shape;

int init_renderer();
Shape create_shape(float *vertices, size_t vertex_count, unsigned int *indices, size_t index_count);
void destroy_shape(Shape shape);
void cleanup_renderer();
void draw_image(const Image *img);
GLuint get_shape_shader();
GLuint get_texture_shader();

#endif