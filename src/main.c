#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "render.h"

Square *board;
Pieces *pieces;
int *game_state;

int start_position[] = {
    3401505708,
    2576980377,
    0,
    0,
    0,
    0,
    286331153,
    1110795044,
};

// int start_position[] = {150746081, 952998129, 208069536, 533331935, 865146542, 276471749, 321237119, 616952554};

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    if (board != NULL) {
        clear_board(board, pieces);
    }
    board = init_board(window);
    pieces = init_pieces(window, board, start_position);
    glViewport(0, 0, width, height);
    printf("Window resized: %d x %d\n", width, height);
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

    // int start_position[] = {
    //     -893461588,
    //     -1717986919,
    //     0,
    //     0,
    //     0,
    //     0,
    //     286331153,
    //     1110856484,
    // };

    board = init_board(window);
    pieces = init_pieces(window, board, start_position);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        draw_board(board);
        draw_pieces(pieces);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanup_renderer();
    clear_board(board, pieces);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
