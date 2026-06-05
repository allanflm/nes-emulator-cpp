// Ponto de entrada do engine. Cria a janela, monta o jogo e roda o loop
// principal com fixed-timestep pra fisica e render por frame.

#include "engine/Window.h"
#include "engine/Input.h"
#include "engine/Time.h"
#include "game/Game.h"

#include <cstdio>

int main() {
    eng::Window window(960, 540, "engine-cpp :: prototipo de plataforma");
    if (!window.ok()) {
        std::fprintf(stderr, "Nao foi possivel iniciar a janela. Encerrando.\n");
        return 1;
    }

    game::Game gameInstance;
    if (!gameInstance.init(window)) {
        std::fprintf(stderr, "Falha ao iniciar o jogo (shaders?). Encerrando.\n");
        return 1;
    }

    std::printf("Controles: A/D ou setas = mover | Espaco/W/Cima = pular | Esc = sair\n");

    eng::Time clock(1.0 / 60.0);

    while (!window.shouldClose()) {
        window.pollEvents();
        clock.tick();

        // Fisica em passos fixos (estavel e independente do FPS).
        while (clock.consumeFixed())
            gameInstance.fixedUpdate(clock.fixedStep());

        gameInstance.render();

        // Marca o fim do frame de input (pra edge-trigger de wasPressed).
        eng::Input::endFrame();

        window.swapBuffers();
    }

    return 0;
}
