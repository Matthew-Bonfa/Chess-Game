#include "stb_image.h"

#include "text.h"

GLuint load_font_texture(const char *filepath, int *width, int *height) {
    int channels;
    unsigned char *data = stbi_load(filepath, width, height, &channels, STBI_rgb_alpha);

    if (!data) {
        fprintf(stderr, "Failed to load font texture: %s\n", filepath);
        exit(1);
    }

    GLuint tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    return tex_id;
}

TextChar create_char_quad(GLFWwindow *window, GLuint font_tex, int atlas_width, int atlas_height, char c, uint32_t x_px, uint32_t y_px, float size_px) {
    TextChar ch;
    ch.texture_id = font_tex;

    const int cols = 32;
    const int rows = 3;
    const float glyph_w = 1.0f / cols;
    const float glyph_h = 1.0f / rows;

    int index = (uint8_t)c - 32;
    if (index < 0 || index >= cols * rows)
        index = 0;

    int col = index % cols;
    int row = index / cols;

    float u_min = col * glyph_w;
    float u_max = (col + 1) * glyph_w;
    float v_max = row * glyph_h;
    float v_min = (row + 1) * glyph_h;

    float vertices[SQUARE_EDGES];
    uint32_t indices[SQUARE_INDICES];
    get_square_coordinates_px(window, x_px, y_px, size_px, vertices, indices);
    for (int i = 0; i < 4; i++) {
        vertices[i * 2 + 1] = -vertices[i * 2 + 1];
    }

    float interleaved[2 * SQUARE_EDGES];
    for (int i = 0; i < 4; ++i) {
        interleaved[i * 4 + 0] = vertices[i * 2 + 0];
        interleaved[i * 4 + 1] = vertices[i * 2 + 1];

        switch (i) {
        case 0:
            interleaved[i * 4 + 2] = u_min; // bottom-left
            interleaved[i * 4 + 3] = v_min;
            break;
        case 1:
            interleaved[i * 4 + 2] = u_max; // bottom-right
            interleaved[i * 4 + 3] = v_min;
            break;
        case 2:
            interleaved[i * 4 + 2] = u_max; // top-right
            interleaved[i * 4 + 3] = v_max;
            break;
        case 3:
            interleaved[i * 4 + 2] = u_min; // top-left
            interleaved[i * 4 + 3] = v_max;
            break;
        }
    }

    glGenVertexArrays(1, &ch.vao);
    glGenBuffers(1, &ch.vbo);
    glGenBuffers(1, &ch.ebo);

    glBindVertexArray(ch.vao);
    glBindBuffer(GL_ARRAY_BUFFER, ch.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(interleaved), interleaved, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ch.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    return ch;
}

void destroy_char(TextChar *ch) {
    if (!ch) {
        return;
    }
    glDeleteVertexArrays(1, &ch->vao);
    glDeleteBuffers(1, &ch->vbo);
    glDeleteBuffers(1, &ch->ebo);
}