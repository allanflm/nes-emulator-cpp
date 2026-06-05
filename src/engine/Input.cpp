#include "engine/Input.h"

namespace eng {

bool Input::s_down[Input::kMaxKeys] = {false};
bool Input::s_prev[Input::kMaxKeys] = {false};

void Input::setKey(int key, bool down) {
    if (key < 0 || key >= kMaxKeys) return;
    s_down[key] = down;
}

void Input::endFrame() {
    for (int i = 0; i < kMaxKeys; ++i)
        s_prev[i] = s_down[i];
}

bool Input::isDown(int key) {
    if (key < 0 || key >= kMaxKeys) return false;
    return s_down[key];
}

bool Input::wasPressed(int key) {
    if (key < 0 || key >= kMaxKeys) return false;
    return s_down[key] && !s_prev[key];
}

bool Input::wasReleased(int key) {
    if (key < 0 || key >= kMaxKeys) return false;
    return !s_down[key] && s_prev[key];
}

} // namespace eng
