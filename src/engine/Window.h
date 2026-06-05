#pragma once
// Wrapper fino sobre o GLFW: cria janela + contexto OpenGL 3.3 core,
// carrega o glad, e expoe o loop basico (poll/swap/should-close).

struct GLFWwindow;

namespace eng {

class Window {
public:
    Window(int width, int height, const char* title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() const;
    void pollEvents() const;
    void swapBuffers() const;
    void close() const;

    int width() const { return m_width; }
    int height() const { return m_height; }
    bool ok() const { return m_handle != nullptr; }

    GLFWwindow* handle() const { return m_handle; }

private:
    GLFWwindow* m_handle = nullptr;
    int m_width = 0;
    int m_height = 0;
};

} // namespace eng
