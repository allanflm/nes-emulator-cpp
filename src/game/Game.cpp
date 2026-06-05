#include "game/Game.h"

#include "engine/Window.h"
#include "engine/Input.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace game {

using eng::Vec2;
using eng::Vec4;

// Constantes do "feel" do platformer (em pixels e pixels/segundo).
static constexpr float kGravity   = 2000.0f; // px/s^2
static constexpr float kMoveSpeed = 350.0f;  // px/s
static constexpr float kJumpSpeed = 750.0f;  // impulso pra cima
static constexpr float kMaxFall   = 1600.0f; // velocidade terminal

bool Game::init(eng::Window& window) {
    m_window = &window;
    m_camera.resize(window.width(), window.height());

    if (!m_renderer.init())
        return false;

    // Chao + algumas plataformas (coordenadas: Y pra baixo, origem no topo).
    const float W = (float)window.width();
    const float H = (float)window.height();

    m_platforms.push_back({{0.0f, H - 40.0f}, {W, 40.0f}});       // chao
    m_platforms.push_back({{150.0f, H - 160.0f}, {200.0f, 24.0f}});
    m_platforms.push_back({{450.0f, H - 280.0f}, {220.0f, 24.0f}});
    m_platforms.push_back({{720.0f, H - 180.0f}, {180.0f, 24.0f}});
    m_platforms.push_back({{300.0f, H - 420.0f}, {160.0f, 24.0f}});

    m_player.pos = {80.0f, H - 120.0f};
    return true;
}

void Game::onResize(int w, int h) {
    m_camera.resize(w, h);
}

void Game::fixedUpdate(float dt) {
    using eng::Input;

    if (Input::wasPressed(GLFW_KEY_ESCAPE))
        m_window->close();

    // ---- Input horizontal ----
    float dir = 0.0f;
    if (Input::isDown(GLFW_KEY_LEFT)  || Input::isDown(GLFW_KEY_A)) dir -= 1.0f;
    if (Input::isDown(GLFW_KEY_RIGHT) || Input::isDown(GLFW_KEY_D)) dir += 1.0f;
    m_player.vel.x = dir * kMoveSpeed;

    // ---- Pulo (edge-trigger, so no chao) ----
    bool jumpPressed = Input::wasPressed(GLFW_KEY_SPACE) ||
                       Input::wasPressed(GLFW_KEY_UP)   ||
                       Input::wasPressed(GLFW_KEY_W);
    if (jumpPressed && m_player.onGround) {
        m_player.vel.y = -kJumpSpeed; // Y pra baixo => negativo sobe
        m_player.onGround = false;
    }

    // ---- Gravidade ----
    m_player.vel.y += kGravity * dt;
    if (m_player.vel.y > kMaxFall) m_player.vel.y = kMaxFall;

    // ---- Integra e resolve colisao eixo a eixo ----
    m_player.onGround = false;

    // Eixo X
    m_player.pos.x += m_player.vel.x * dt;
    for (const auto& p : m_platforms) {
        if (intersects(m_player.box(), p)) {
            if (m_player.vel.x > 0.0f)
                m_player.pos.x = p.left() - m_player.size.x;
            else if (m_player.vel.x < 0.0f)
                m_player.pos.x = p.right();
            m_player.vel.x = 0.0f;
        }
    }

    // Eixo Y
    m_player.pos.y += m_player.vel.y * dt;
    for (const auto& p : m_platforms) {
        if (intersects(m_player.box(), p)) {
            if (m_player.vel.y > 0.0f) {        // caindo: pousa em cima
                m_player.pos.y = p.top() - m_player.size.y;
                m_player.onGround = true;
            } else if (m_player.vel.y < 0.0f) { // subindo: bate a cabeca
                m_player.pos.y = p.bottom();
            }
            m_player.vel.y = 0.0f;
        }
    }
}

void Game::render() {
    glClearColor(0.10f, 0.12f, 0.18f, 1.0f); // azul escuro de fundo
    glClear(GL_COLOR_BUFFER_BIT);

    m_renderer.begin(m_camera.projection());

    // Plataformas (verde).
    const Vec4 platformColor{0.30f, 0.78f, 0.45f, 1.0f};
    for (const auto& p : m_platforms)
        m_renderer.drawQuad(p.pos, p.size, platformColor);

    // Player (laranja).
    const Vec4 playerColor{0.95f, 0.55f, 0.20f, 1.0f};
    m_renderer.drawQuad(m_player.pos, m_player.size, playerColor);
}

} // namespace game
