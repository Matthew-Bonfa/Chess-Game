#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "image.h"

const float uvs[8] = {
    0.0f, 0.0f, // bottom-left
    1.0f, 0.0f, // bottom-right
    1.0f, 1.0f, // top-right
    0.0f, 1.0f  // top-left
};

Image load_image(const char *filepath, GLFWwindow *window, uint32_t x, uint32_t y, float size) {
    Image img;

    unsigned char *data = stbi_load(filepath, &img.width, &img.height, &img.channels, STBI_rgb_alpha);
    img.channels = 4;

    if (!data) {
        fprintf(stderr, "Failed to load image: %s\n", filepath);
        exit(1);
    }

    glGenTextures(1, &img.texture_id);
    glBindTexture(GL_TEXTURE_2D, img.texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format;
    GLenum internal_format;

    switch (img.channels) {
    case 1:
        format = internal_format = GL_RED;
        break;
    case 2:
        format = internal_format = GL_RG;
        break;
    case 3:
        format = GL_RGB;
        internal_format = GL_RGB8;
        break;
    case 4:
        format = GL_RGBA;
        internal_format = GL_RGBA8;
        break;
    default:
        fprintf(stderr, "Unsupported image channel count: %d\n", img.channels);
        stbi_image_free(data);
        exit(1);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, img.width, img.height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    float vertices[SQUARE_EDGES];
    uint32_t indices[SQUARE_INDICES];
    get_square_cooridnates_px(window, x, y, size, vertices, indices);

    float interleaved[2 * SQUARE_EDGES];

    for (int32_t i = 0; i < 4; i++) {
        interleaved[i * 4 + 0] = vertices[i * 2 + 0];
        interleaved[i * 4 + 1] = vertices[i * 2 + 1];
        interleaved[i * 4 + 2] = uvs[i * 2 + 0];
        interleaved[i * 4 + 3] = uvs[i * 2 + 1];
    }

    glGenVertexArrays(1, &img.vao);
    glGenBuffers(1, &img.vbo);
    glGenBuffers(1, &img.ebo);

    glBindVertexArray(img.vao);

    glBindBuffer(GL_ARRAY_BUFFER, img.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(interleaved), interleaved, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, img.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute (location = 0)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // texture coord attribute (location = 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    return img;
}

void destroy_image(Image img) {
    glDeleteTextures(1, &img.texture_id);
    glDeleteVertexArrays(1, &img.vao);
    glDeleteBuffers(1, &img.vbo);
    glDeleteBuffers(1, &img.ebo);
}