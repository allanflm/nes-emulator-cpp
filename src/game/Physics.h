#pragma once
// Fisica 2D minima: AABB (axis-aligned bounding box) e teste/resolucao de colisao.

#include "engine/Math.h"

namespace game {

struct AABB {
    eng::Vec2 pos;   // canto superior esquerdo
    eng::Vec2 size;  // largura, altura

    float left()   const { return pos.x; }
    float right()  const { return pos.x + size.x; }
    float top()    const { return pos.y; }
    float bottom() const { return pos.y + size.y; }
};

// Verdadeiro se as duas caixas se sobrepoem.
bool intersects(const AABB& a, const AABB& b);

} // namespace game
