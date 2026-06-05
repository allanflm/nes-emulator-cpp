#pragma once
// Camera ortografica 2D. Converte coordenadas de mundo (pixels) para clip space.
// Origem no canto superior esquerdo, Y crescendo pra baixo.

#include "engine/Math.h"

namespace eng {

class Camera2D {
public:
    Camera2D() = default;
    Camera2D(int viewportW, int viewportH) { resize(viewportW, viewportH); }

    void resize(int viewportW, int viewportH) {
        // top=0, bottom=height => Y pra baixo
        m_projection = Mat4::ortho(0.0f, (float)viewportW,
                                   (float)viewportH, 0.0f);
    }

    const Mat4& projection() const { return m_projection; }

private:
    Mat4 m_projection = Mat4::identity();
};

} // namespace eng
