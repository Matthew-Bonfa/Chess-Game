#ifndef IMAGE_H
#define IMAGE_H
#include <glad/glad.h>

#include "utils.h"

typedef enum {
    TEX_WHITE_PAWN,
    TEX_WHITE_KNIGHT,
    TEX_WHITE_BISHOP,
    TEX_WHITE_ROOK,
    TEX_WHITE_QUEEN,
    TEX_WHITE_KING,
    TEX_BLACK_PAWN,
    TEX_BLACK_KNIGHT,
    TEX_BLACK_BISHOP,
    TEX_BLACK_ROOK,
    TEX_BLACK_QUEEN,
    TEX_BLACK_KING,
    TEX_DOT,
    TEX_RING,
    TEX_FONT,
    NUM_TEXTURES
} TextureType;

typedef struct {
    int32_t width, height, channels;
    GLuint texture_id;
    GLuint vao, vbo, ebo;
} Image;

void load_all_textures();
Image create_image(TextureType texture_id, GLFWwindow *window, uint32_t x, uint32_t y, float size);
void destroy_image(Image *img);
void cleanup_textures();

#endif
