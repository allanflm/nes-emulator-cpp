cmake_minimum_required(VERSION 3.16)
project(engine-cpp C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Coloca o executavel em build/ (raiz do diretorio de build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(FetchContent)

# ---- GLFW: janela, contexto OpenGL e input ----
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
    GIT_SHALLOW    TRUE
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# ---- glad: loader das funcoes do OpenGL 3.3 core ----
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v2.0.8
    GIT_SHALLOW    TRUE
    SOURCE_SUBDIR  cmake
)
FetchContent_MakeAvailable(glad)
glad_add_library(glad_gl_core_33 STATIC API gl:core=3.3)

# ---- Executavel do engine ----
add_executable(engine
    src/main.cpp
    src/engine/Window.cpp
    src/engine/Input.cpp
    src/engine/Shader.cpp
    src/engine/Texture.cpp
    src/engine/SpriteRenderer.cpp
    src/game/Physics.cpp
    src/game/Game.cpp
)

target_include_directories(engine PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/vendor
)

target_link_libraries(engine PRIVATE glfw glad_gl_core_33)

if (WIN32)
    target_link_libraries(engine PRIVATE opengl32)
endif()

# ---- Executavel do emulador NES (reusa a camada de video do engine) ----
add_executable(nes
    src/nes_main.cpp
    src/engine/Window.cpp
    src/engine/Input.cpp
    src/engine/Shader.cpp
    src/engine/Texture.cpp
    src/engine/SpriteRenderer.cpp
    src/nes/Bus.cpp
    src/nes/Cpu6502.cpp
    src/nes/Cartridge.cpp
    src/nes/mappers/Mapper000.cpp
)

target_include_directories(nes PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/vendor
)

target_link_libraries(nes PRIVATE glfw glad_gl_core_33)

if (WIN32)
    target_link_libraries(nes PRIVATE opengl32)
endif()
