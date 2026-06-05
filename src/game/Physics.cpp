#include "game/Physics.h"

namespace game {

bool intersects(const AABB& a, const AABB& b) {
    return a.left()   < b.right() &&
           a.right()  > b.left()  &&
           a.top()    < b.bottom() &&
           a.bottom() > b.top();
}

} // namespace game
