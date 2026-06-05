#pragma once
// Controle de tempo: delta entre frames + acumulador pra fixed timestep.
// O loop principal consome o acumulador em passos fixos (ex.: 1/60s) pra
// manter a fisica estavel e independente do FPS de render.

#include <GLFW/glfw3.h>

namespace eng {

class Time {
public:
    explicit Time(double fixedStep = 1.0 / 60.0)
        : m_fixedStep(fixedStep), m_last(glfwGetTime()) {}

    // Chamar uma vez por frame. Atualiza delta e o acumulador.
    void tick() {
        double now = glfwGetTime();
        m_delta = now - m_last;
        m_last = now;
        // Evita "espiral da morte" se um frame travar muito.
        if (m_delta > 0.25) m_delta = 0.25;
        m_accumulator += m_delta;
    }

    // Consome um passo fixo do acumulador. Use em while(consumeFixed()).
    bool consumeFixed() {
        if (m_accumulator >= m_fixedStep) {
            m_accumulator -= m_fixedStep;
            return true;
        }
        return false;
    }

    float fixedStep() const { return (float)m_fixedStep; }
    float delta() const { return (float)m_delta; }

private:
    double m_fixedStep;
    double m_last;
    double m_delta = 0.0;
    double m_accumulator = 0.0;
};

} // namespace eng
