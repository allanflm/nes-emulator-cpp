#pragma once
// Prototipo de plataforma 2D: um player que anda, sofre gravidade, pula e
// colide com plataformas estaticas. Separa update (fisica, passo fixo) de
// render (por frame).

#include "engine/Math.h"
#include "engine/Camera2D.h"
#include "engine/SpriteRenderer.h"
#include "game/Physics.h"

#include <vector>

namespace eng { class Window; }

namespace game {

struct Player {
    eng::Vec2 pos{100.0f, 100.0f};
    eng::Vec2 vel{0.0f, 0.0f};
    eng::Vec2 size{32.0f, 48.0f};
    bool onGround = false;

    AABB box() const { return {pos, size}; }
};

class Game {
public:
    bool init(eng::Window& window);

    // Passo fixo de fisica (dt = fixedStep). Le input aqui.
    void fixedUpdate(float dt);

    // Desenha o frame atual.
    void render();

    void onResize(int w, int h);

private:
    void resolveCollisions();

    eng::Window* m_window = nullptr;
    eng::Camera2D m_camera;
    eng::SpriteRenderer m_renderer;

    Player m_player;
    std::vector<AABB> m_platforms;
};

} // namespace game
