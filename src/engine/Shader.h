#pragma once
// Compila e linka um shader program (vertex + fragment) e ajuda a setar uniforms.

#include "engine/Math.h"

namespace eng {

class Shader {
public:
    Shader() = default;
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Compila a partir de codigo-fonte (strings). Retorna false em erro.
    bool compile(const char* vertexSrc, const char* fragmentSrc);

    void use() const;

    void setMat4(const char* name, const Mat4& mat) const;
    void setVec4(const char* name, const Vec4& v) const;
    void setInt(const char* name, int value) const;

    unsigned int id() const { return m_id; }

private:
    unsigned int m_id = 0;
};

} // namespace eng
