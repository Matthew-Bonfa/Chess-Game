#ifndef IMAGE_H
#define IMAGE_H
#include <glad/glad.h>

#include "utils.h"

typedef struct {
    int32_t width, height, channels;
    GLuint texture_id;
    GLuint vao, vbo, ebo;
} Image;

Image load_image(const char *filepath, GLFWwindow *window, uint32_t x, uint32_t y, float size);
void destroy_image(Image img);

#endif
