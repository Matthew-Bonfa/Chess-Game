#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "image.h"

const float uvs[8] = {
    0.0f, 0.0f, // bottom-left
    1.0f, 0.0f, // bottom-right
    1.0f, 1.0f, // top-right
    0.0f, 1.0f  // top-left
};

static GLuint textures[NUM_TEXTURES];

static GLuint load_texture(const char *filepath) {
    int width, height, channels;
    unsigned char *data = stbi_load(filepath, &width, &height, &channels, STBI_rgb_alpha);
    if (!data) {
        fprintf(stderr, "Failed to load texture: %s\n", filepath);
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = GL_RGBA;
    GLenum internal_format = GL_RGBA8;
    if (channels == 3) {
        format = GL_RGB;
        internal_format = GL_RGB8;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    return texture;
}

void load_all_textures() {
    textures[TEX_WHITE_PAWN] = load_texture("./assets/white-pawn.png");
    textures[TEX_WHITE_KNIGHT] = load_texture("./assets/white-knight.png");
    textures[TEX_WHITE_BISHOP] = load_texture("./assets/white-bishop.png");
    textures[TEX_WHITE_ROOK] = load_texture("./assets/white-rook.png");
    textures[TEX_WHITE_QUEEN] = load_texture("./assets/white-queen.png");
    textures[TEX_WHITE_KING] = load_texture("./assets/white-king.png");
    textures[TEX_BLACK_PAWN] = load_texture("./assets/black-pawn.png");
    textures[TEX_BLACK_KNIGHT] = load_texture("./assets/black-knight.png");
    textures[TEX_BLACK_BISHOP] = load_texture("./assets/black-bishop.png");
    textures[TEX_BLACK_ROOK] = load_texture("./assets/black-rook.png");
    textures[TEX_BLACK_QUEEN] = load_texture("./assets/black-queen.png");
    textures[TEX_BLACK_KING] = load_texture("./assets/black-king.png");
    textures[TEX_DOT] = load_texture("./assets/dot.png");
    textures[TEX_RING] = load_texture("./assets/ring.png");
    textures[TEX_FONT] = load_texture("./assets/font.png");
}

static GLuint get_texture(TextureType type) { return textures[type]; }

Image create_image(TextureType texture_id, GLFWwindow *window, uint32_t x, uint32_t y, float size) {
    Image img;
    img.texture_id = get_texture(texture_id);

    float vertices[SQUARE_EDGES];
    uint32_t indices[SQUARE_INDICES];
    get_square_coordinates_px(window, x, y, size, vertices, indices);

    glGenVertexArrays(1, &img.vao);
    glGenBuffers(1, &img.vbo);
    glGenBuffers(1, &img.ebo);
    glBindVertexArray(img.vao);

    float interleaved[2 * SQUARE_EDGES];

    for (int32_t i = 0; i < 4; i++) {
        interleaved[i * 4 + 0] = vertices[i * 2 + 0];
        interleaved[i * 4 + 1] = vertices[i * 2 + 1];
        interleaved[i * 4 + 2] = uvs[i * 2 + 0];
        interleaved[i * 4 + 3] = uvs[i * 2 + 1];
    }

    glBindBuffer(GL_ARRAY_BUFFER, img.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(interleaved), interleaved, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, img.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    return img;
}

void destroy_image(Image *img) {
    glDeleteVertexArrays(1, &img->vao);
    glDeleteBuffers(1, &img->vbo);
    glDeleteBuffers(1, &img->ebo);
}

void cleanup_textures() {
    for (int i = 0; i < NUM_TEXTURES; i++) {
        glDeleteTextures(1, &textures[i]);
    }
}
