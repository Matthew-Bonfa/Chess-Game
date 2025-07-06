#include "render.h"

static GLuint shape_shader_program;
static GLuint texture_shader_program;

static int32_t compile_shader(uint32_t type, const char *source) {
    uint32_t shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int32_t success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "Shader compile error: %s\n", infoLog);
        return 0;
    }
    return shader;
}

int32_t init_renderer() {
    // Load shape shader
    char *vertex_shader_src = load_shader_source("./src/shaders/vertex_shader.glsl");
    char *fragment_shader_src = load_shader_source("./src/shaders/fragment_shader.glsl");

    if (!vertex_shader_src || !fragment_shader_src) {
        fprintf(stderr, "Failed to load shape shader source files\n");
        return 0;
    }

    GLuint vertexShader = compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
    GLuint fragmentShader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src);

    shape_shader_program = glCreateProgram();
    glAttachShader(shape_shader_program, vertexShader);
    glAttachShader(shape_shader_program, fragmentShader);
    glLinkProgram(shape_shader_program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    free(vertex_shader_src);
    free(fragment_shader_src);

    // Load texture shader
    char *tex_vert = load_shader_source("./src/shaders/texture_vertex_shader.glsl");
    char *tex_frag = load_shader_source("./src/shaders/texture_fragment_shader.glsl");

    if (!tex_vert || !tex_frag) {
        fprintf(stderr, "Failed to load texture shader source files\n");
        return 0;
    }

    GLuint tex_vs = compile_shader(GL_VERTEX_SHADER, tex_vert);
    GLuint tex_fs = compile_shader(GL_FRAGMENT_SHADER, tex_frag);

    texture_shader_program = glCreateProgram();
    glAttachShader(texture_shader_program, tex_vs);
    glAttachShader(texture_shader_program, tex_fs);
    glLinkProgram(texture_shader_program);

    glDeleteShader(tex_vs);
    glDeleteShader(tex_fs);
    free(tex_vert);
    free(tex_frag);

    return 1;
}

Shape create_shape(float *vertices, size_t vertex_count, uint32_t *indices, size_t index_count) {
    Shape shape;
    glGenVertexArrays(1, &shape.vao);
    glGenBuffers(1, &shape.vbo);
    glGenBuffers(1, &shape.ebo);
    shape.index_count = index_count;

    glBindVertexArray(shape.vao);

    glBindBuffer(GL_ARRAY_BUFFER, shape.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(float), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(uint32_t), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    return shape;
}

void draw_shape(Shape *shape, color_t *color) {
    glUseProgram(shape_shader_program);

    int32_t colorLoc = glGetUniformLocation(shape_shader_program, "uColor");
    glUniform3f(colorLoc, color->r, color->g, color->b);

    glBindVertexArray(shape->vao);
    glDrawElements(GL_TRIANGLES, shape->index_count, GL_UNSIGNED_INT, 0);
}

void destroy_shape(Shape shape) {
    glDeleteVertexArrays(1, &shape.vao);
    glDeleteBuffers(1, &shape.vbo);
    glDeleteBuffers(1, &shape.ebo);
}

void draw_image(Image *img) {
    glUseProgram(texture_shader_program);
    glBindTexture(GL_TEXTURE_2D, img->texture_id);
    glBindVertexArray(img->vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void cleanup_renderer() {
    glDeleteProgram(shape_shader_program);
}

GLuint get_shape_shader() {
    return shape_shader_program;
}

GLuint get_texture_shader() {
    return texture_shader_program;
}