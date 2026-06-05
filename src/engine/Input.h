#pragma once
// Estado do teclado. Diferencia "segurando" (isDown) de "acabou de apertar"
// neste frame (wasPressed) — util pra acoes como pulo (edge-trigger).

namespace eng {

class Input {
public:
    // Chamado pelo callback do GLFW (ver Window.cpp).
    static void setKey(int key, bool down);

    // Deve ser chamado no fim de cada frame, depois do update do jogo.
    static void endFrame();

    static bool isDown(int key);      // tecla segurada
    static bool wasPressed(int key);  // tecla apertada neste frame
    static bool wasReleased(int key); // tecla solta neste frame

private:
    static constexpr int kMaxKeys = 512;
    static bool s_down[kMaxKeys];
    static bool s_prev[kMaxKeys];
};

} // namespace eng
