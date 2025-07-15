#ifndef RENDER_H
#define RENDER_H

#include <glad/glad.h>

#include "image.h"
#include "text.h"
#include "utils.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    uint32_t vao;
    uint32_t vbo;
    uint32_t ebo;
    uint32_t index_count;
} Shape;

int32_t init_renderer();
Shape create_shape(float *vertices, size_t vertex_count, uint32_t *indices, size_t index_count);
void draw_shape(Shape *shape, color_t *color);
void destroy_shape(Shape shape);
void cleanup_renderer();
void draw_image(Image *img);
void draw_char(TextChar *ch);
GLuint get_shape_shader();
GLuint get_texture_shader();

#endif