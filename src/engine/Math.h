#pragma once
// Mini biblioteca de matematica 2D — sem dependencia externa (sem glm).
// So o necessario pra um engine 2D: Vec2 e Mat4 (ortho/translate/scale).

#include <cmath>

namespace eng {

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    Vec2() = default;
    Vec2(float x_, float y_) : x(x_), y(y_) {}

    Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    Vec2 operator*(float s) const { return {x * s, y * s}; }

    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
    Vec2& operator-=(const Vec2& o) { x -= o.x; y -= o.y; return *this; }
};

struct Vec4 {
    float r = 0.0f, g = 0.0f, b = 0.0f, a = 1.0f;
    Vec4() = default;
    Vec4(float r_, float g_, float b_, float a_ = 1.0f) : r(r_), g(g_), b(b_), a(a_) {}
};

// Matriz 4x4 em column-major (mesmo layout que o OpenGL espera).
// m[col*4 + row]
struct Mat4 {
    float m[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    static Mat4 identity() { return Mat4{}; }

    // Projecao ortografica. left/right/bottom/top em pixels.
    // Aqui usamos top=0 e bottom=height, ou seja Y cresce pra BAIXO
    // (origem no canto superior esquerdo, como coordenadas de tela).
    static Mat4 ortho(float left, float right, float bottom, float top,
                      float near_ = -1.0f, float far_ = 1.0f) {
        Mat4 r;
        r.m[0]  = 2.0f / (right - left);
        r.m[5]  = 2.0f / (top - bottom);
        r.m[10] = -2.0f / (far_ - near_);
        r.m[12] = -(right + left) / (right - left);
        r.m[13] = -(top + bottom) / (top - bottom);
        r.m[14] = -(far_ + near_) / (far_ - near_);
        r.m[15] = 1.0f;
        return r;
    }

    static Mat4 translate(const Vec2& t) {
        Mat4 r;
        r.m[12] = t.x;
        r.m[13] = t.y;
        return r;
    }

    static Mat4 scale(const Vec2& s) {
        Mat4 r;
        r.m[0] = s.x;
        r.m[5] = s.y;
        return r;
    }

    // Multiplicacao de matrizes: this * o
    Mat4 operator*(const Mat4& o) const {
        Mat4 r;
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k)
                    sum += m[k * 4 + row] * o.m[col * 4 + k];
                r.m[col * 4 + row] = sum;
            }
        }
        return r;
    }
};

} // namespace eng
