#ifndef TEXT_H
#define TEXT_H

#include <glad/glad.h>

#include "utils.h"

typedef struct {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint texture_id;
    int width;
    int height;
    int channels;
    float u_min;
    float v_min;
    float u_max;
    float v_max;
    int advance; // optional: if using bitmap font metrics
} TextChar;

GLuint load_font_texture(const char *filepath, int *width, int *height);
TextChar create_char_quad(GLFWwindow *window, GLuint font_tex, int atlas_width, int atlas_height, char c, uint32_t x_px, uint32_t y_px, float size_px);
void destroy_char(TextChar *ch);

#endif