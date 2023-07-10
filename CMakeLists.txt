cmake_minimum_required(VERSION 3.20)
project(snake-game LANGUAGES C)

add_subdirectory(lib/glfw)
add_subdirectory(lib/glad)
add_subdirectory(lib/openal-soft)

find_package(OpenGL REQUIRED)
#find_package(OpenAL REQUIRED)

add_executable(snake-game
	src/button.c
    src/library.c
    src/main.c
    src/powerup.c
    src/snake.c
    src/sound.c
    src/text.c
)
target_include_directories(snake-game PUBLIC include)
target_link_libraries(snake-game PRIVATE glfw glad OpenGL::GL OpenAL::OpenAL)
