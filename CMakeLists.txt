cmake_minimum_required(VERSION 3.14) # Bumped to 3.14 for FetchContent support
project(ChessGame C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)

# 1. Include FetchContent
include(FetchContent)

# 2. Fetch GLFW automatically during build
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8
)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# 3. Find system OpenGL (provided by the OS)
find_package(OpenGL REQUIRED)

include_directories("./src/include")

add_executable(chess
    src/main.c
    src/client.c
    src/moves.c
    src/game.c
    src/render.c
    src/image.c
    src/text.c
    src/board.c
    src/utils.c
    src/glad.c  # <-- Added glad.c to your source files
)

# 4. Link the fetched GLFW and system OpenGL
target_link_libraries(chess
    glfw
    OpenGL::GL
)
