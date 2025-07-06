#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "render.h"

Square *board;
Pieces *pieces;
int32_t selected = -1;

uint32_t game_state[] = {
    3401444268,
    2576980377,
    0,
    0,
    0,
    0,
    286331153,
    1110795044,
};

uint16_t game_condition = 0;
// int32_t game_condition = 288620543;

uint64_t legal_moves[BOARD_SQUARES];

char cols[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
char rows[] = {'8', '7', '6', '5', '4', '3', '2', '1'};

static void print_location(int16_t loc) {
    if (loc == -1) {
        printf("unselected ");
        return;
    }
    printf("%c%c ", cols[loc % 8], rows[loc / 8]);
}

static void print_bits_long(uint64_t num) {
    printf("------------\n");
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%d ", (num & (uint64_t)1 << (8 * i + j)) >> (8 * i + j));
        }
        printf("\n");
    }
    printf("------------\n");
}

static void key_callback(GLFWwindow *window, int32_t key, int32_t scancode, int32_t action, int32_t mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static void framebuffer_size_callback(GLFWwindow *window, int32_t width, int32_t height) {
    if (board != NULL) {
        clear_board(board, pieces);
    }
    board = init_board(window);
    pieces = init_pieces(window, board, game_state);
    glViewport(0, 0, width, height);
    // printf("Window resized: %d x %d\n", width, height);
}

static void mouse_button_callback(GLFWwindow *window, int32_t button, int32_t action, int32_t mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        convert_to_px(window, &xpos, &ypos);

        int32_t clicked_square = get_square(window, xpos, ypos);
        if (clicked_square < 0 || clicked_square > 63) {
            return;
        }
        if (selected == clicked_square) {
            selected = -1;
            return;
        }
        // if (selected != -1) {
        //     print_bits_long(selected);
        // }

        // printf("%d, %d\n", clicked_square, selected);
        print_location(clicked_square);
        print_location(selected);
        printf("%d\n", get_piece(game_state, clicked_square));
        printf("\n");

        if (make_move(game_state, &game_condition, legal_moves, &selected, &clicked_square)) {
            // update_legal_moves(game_state, &game_condition, legal_moves);
            // for (int i = 0; i < 64; i++) {
            //     print_bits_long(legal_moves[i]);
            // }
        }
        pieces = init_pieces(window, board, game_state);
        board = init_board(window);
    }
}

int32_t main() {
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
    glfwSetMouseButtonCallback(window, mouse_button_callback);

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
    pieces = init_pieces(window, board, game_state);

    for (int32_t i = 0; i < BOARD_SQUARES; i++) {
        legal_moves[i] = 0;
    }
    update_legal_moves(game_state, &game_condition, legal_moves);

    // update_legal_moves(game_state, &game_condition, legal_moves);
    //  for (int i = 0; i < 64; i++) {
    //      print_bits_long(legal_moves[i]);
    //  }

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        draw_board(board, selected);
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
