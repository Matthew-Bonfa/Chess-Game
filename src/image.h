#ifndef IMAGE_H
#define IMAGE_H
#include <glad/glad.h>

#include "utils.h"

typedef struct {
    int width, height, channels;
    GLuint texture_id;
    GLuint vao, vbo, ebo;
} Image;

Image load_image(const char *filepath, GLFWwindow *window, unsigned int x, unsigned int y, float size);
void destroy_image(Image img);

#endif
