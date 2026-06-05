#include "engine/Window.h"
#include "engine/Input.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <cstdio>

namespace eng {

static void keyCallback(GLFWwindow* /*win*/, int key, int /*scancode*/,
                        int action, int /*mods*/) {
    if (action == GLFW_PRESS)
        Input::setKey(key, true);
    else if (action == GLFW_RELEASE)
        Input::setKey(key, false);
}

static void framebufferSizeCallback(GLFWwindow* /*win*/, int w, int h) {
    glViewport(0, 0, w, h);
}

Window::Window(int width, int height, const char* title)
    : m_width(width), m_height(height) {
    if (!glfwInit()) {
        std::fprintf(stderr, "[Window] Falha ao iniciar GLFW\n");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_handle = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_handle) {
        std::fprintf(stderr, "[Window] Falha ao criar a janela\n");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_handle);

    if (!gladLoadGL(glfwGetProcAddress)) {
        std::fprintf(stderr, "[Window] Falha ao carregar OpenGL (glad)\n");
        glfwDestroyWindow(m_handle);
        m_handle = nullptr;
        glfwTerminate();
        return;
    }

    glfwSetKeyCallback(m_handle, keyCallback);
    glfwSetFramebufferSizeCallback(m_handle, framebufferSizeCallback);
    glfwSwapInterval(1); // vsync

    glViewport(0, 0, width, height);
    std::printf("[Window] OpenGL %s\n", glGetString(GL_VERSION));
}

Window::~Window() {
    if (m_handle) {
        glfwDestroyWindow(m_handle);
    }
    glfwTerminate();
}

bool Window::shouldClose() const {
    return !m_handle || glfwWindowShouldClose(m_handle);
}

void Window::pollEvents() const {
    glfwPollEvents();
}

void Window::swapBuffers() const {
    glfwSwapBuffers(m_handle);
}

void Window::close() const {
    if (m_handle) glfwSetWindowShouldClose(m_handle, GLFW_TRUE);
}

} // namespace eng
