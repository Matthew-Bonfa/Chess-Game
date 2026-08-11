#include "client.h"
#include "GLFW/glfw3.h"
#include "board.h"

Square *board;
Pieces *pieces;
Moves *dots;
Legends *legends;
int32_t selected = UNSELECTED;

uint32_t game_state[] = {
    3401444268, 2576980377, 0, 0, 0, 0, 286331153, 1110795044,
};

uint32_t prev_game_state[8];
uint16_t game_condition = 0;
uint64_t legal_moves[BOARD_SQUARES];

uint16_t color_to_move = WHITE;

typedef struct Client_t {
    GLFWwindow *window;
} Client_t;

static void key_callback(GLFWwindow *window, int32_t key, int32_t scancode, int32_t action, int32_t mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static void framebuffer_size_callback(GLFWwindow *window, int32_t width, int32_t height) {
    if (board != NULL) {
        clear_board(board, pieces, dots, legends);
    }
    board = init_board(window);
    pieces = init_pieces(window, board, game_state);
    dots = init_dots(window, board);
    legends = init_legends(window, board);
    glViewport(0, 0, width, height);
}

static void mouse_button_callback(GLFWwindow *window, int32_t button, int32_t action, int32_t mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        convert_to_px(window, &xpos, &ypos);

        int32_t clicked_square = get_square(window, xpos, ypos);
        if (clicked_square < 0 || clicked_square >= BOARD_SQUARES) {
            return;
        }
        if (selected == clicked_square) {
            selected = UNSELECTED;
            return;
        }

        // print_location(clicked_square);
        // print_location(selected);
        // printf("%d\n", get_piece(game_state, clicked_square));
        // printf("\n");

        if (make_move(game_state, &game_condition, legal_moves, &selected, &clicked_square, &color_to_move,
                      prev_game_state)) {
            if (color_to_move == WHITE) {
                color_to_move = BLACK;
            } else {
                color_to_move = WHITE;
            }
        }
        pieces = init_pieces(window, board, game_state);
        board = init_board(window);
    }
}

void free_client(Client_t *client) {
    cleanup_renderer();
    clear_board(board, pieces, dots, legends);
    free_colors();

    if (client->window != NULL)
        glfwDestroyWindow(client->window);
    glfwTerminate();
    free(client);
    return;
}

Client_t *init_client() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return NULL;
    }

    // hints for mac
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // REQUIRED FOR MAC!
#endif

    GLFWwindow *window = glfwCreateWindow(1000, 1000, "Chess Game", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return NULL;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return NULL;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (!init_renderer()) {
        fprintf(stderr, "Renderer failed to initialize\n");
        return NULL;
    }

    load_all_textures();

    board = init_board(window);
    pieces = init_pieces(window, board, game_state);
    dots = init_dots(window, board);
    legends = init_legends(window, board);

    for (int32_t i = 0; i < BOARD_SQUARES; i++) {
        legal_moves[i] = 0;
    }
    update_legal_moves(game_state, &game_condition, legal_moves, prev_game_state, color_to_move);

    Client_t *client = malloc(sizeof(Client_t));
    if (client == NULL)
        return NULL;
    client->window = window;
    return client;
}

void run_client(Client_t *client) {
    while (!glfwWindowShouldClose(client->window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        draw_board(board, selected);
        draw_legends(legends);
        if (selected != -1) {
            draw_dots(dots, game_state, legal_moves, selected);
        }
        draw_pieces(pieces);

        glfwSwapBuffers(client->window);
        glfwPollEvents();
    }
    free_client(client);
}
