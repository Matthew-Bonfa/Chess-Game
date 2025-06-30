#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "render.h"

Shape *board;

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    if (board != NULL) {
        // clear_board(board);
        free(board);
    }
    board = init_board(window);
    glViewport(0, 0, width, height);
    // printf("Window resized: %d x %d\n", width, height);
}

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    GLFWwindow *window = glfwCreateWindow(1000, 1000, "Chess Game", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (!init_renderer()) {
        fprintf(stderr, "Renderer failed to initialize\n");
        return -1;
    }

    board = init_board(window);
    float x_shift, y_shift;
    float size = get_square_size(window, &x_shift, &y_shift);
    Image piece_image = load_image("./assets/white-pawn.png", window, 0 + x_shift, 0 + y_shift, size);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        draw_board(board);
        draw_image(&piece_image);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanup_renderer();
    clear_board(board);
    free(board);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
