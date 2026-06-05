#pragma once
// Renderer de quads 2D. Reusa um unico quad unitario (VAO/VBO) e o posiciona
// via matriz model = translate(pos) * scale(size). Desenha cor solida ou textura.

#include "engine/Math.h"
#include "engine/Shader.h"

namespace eng {

class Texture;

class SpriteRenderer {
public:
    SpriteRenderer() = default;
    ~SpriteRenderer();

    bool init();

    void begin(const Mat4& projection);

    // Quad colorido. pos = canto superior esquerdo, size em pixels.
    void drawQuad(const Vec2& pos, const Vec2& size, const Vec4& color);

    // Quad texturizado (modulado por color; use branco pra cor original).
    void drawSprite(const Vec2& pos, const Vec2& size,
                    const Texture& texture, const Vec4& color = {1, 1, 1, 1});

private:
    void draw(const Vec2& pos, const Vec2& size, const Vec4& color,
              const Texture* texture);

    Shader m_shader;
    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    Mat4 m_projection;
};

} // namespace eng
