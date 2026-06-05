#include "engine/SpriteRenderer.h"
#include "engine/Texture.h"

#include <glad/gl.h>

namespace eng {

// Shaders embutidos como raw string (evita depender de caminho de arquivo).
static const char* kVertexSrc = R"(#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

uniform mat4 uProjection;
uniform mat4 uModel;

out vec2 vUV;

void main() {
    vUV = aUV;
    gl_Position = uProjection * uModel * vec4(aPos, 0.0, 1.0);
}
)";

static const char* kFragmentSrc = R"(#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform vec4 uColor;
uniform int uUseTexture;
uniform sampler2D uTexture;

void main() {
    if (uUseTexture == 1) {
        FragColor = texture(uTexture, vUV) * uColor;
    } else {
        FragColor = uColor;
    }
}
)";

SpriteRenderer::~SpriteRenderer() {
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
}

bool SpriteRenderer::init() {
    if (!m_shader.compile(kVertexSrc, kFragmentSrc))
        return false;

    // Quad unitario (0,0)-(1,1): posicao + UV.
    float vertices[] = {
        // pos      // uv
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,

        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void*)(2 * sizeof(float)));

    glBindVertexArray(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return true;
}

void SpriteRenderer::begin(const Mat4& projection) {
    m_projection = projection;
    m_shader.use();
    m_shader.setMat4("uProjection", m_projection);
    m_shader.setInt("uTexture", 0);
}

void SpriteRenderer::draw(const Vec2& pos, const Vec2& size,
                          const Vec4& color, const Texture* texture) {
    m_shader.use();
    Mat4 model = Mat4::translate(pos) * Mat4::scale(size);
    m_shader.setMat4("uModel", model);
    m_shader.setVec4("uColor", color);

    if (texture) {
        m_shader.setInt("uUseTexture", 1);
        texture->bind(0);
    } else {
        m_shader.setInt("uUseTexture", 0);
    }

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void SpriteRenderer::drawQuad(const Vec2& pos, const Vec2& size,
                              const Vec4& color) {
    draw(pos, size, color, nullptr);
}

void SpriteRenderer::drawSprite(const Vec2& pos, const Vec2& size,
                                const Texture& texture, const Vec4& color) {
    draw(pos, size, color, &texture);
}

} // namespace eng
