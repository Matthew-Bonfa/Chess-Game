cmake_minimum_required(VERSION 3.10)
project(ChessGame C)

set(CMAKE_C_STANDARD 11)

include_directories("./src/include")

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(chess
    src/main.c
    src/render.c
    src/image.c
    src/board.c
    src/utils.c
)

target_link_libraries(chess
    glad::glad
    glfw
    OpenGL::GL
)
