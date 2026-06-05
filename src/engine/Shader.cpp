#include "engine/Shader.h"

#include <glad/gl.h>
#include <cstdio>

namespace eng {

static unsigned int compileStage(GLenum type, const char* src) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::fprintf(stderr, "[Shader] Erro ao compilar %s:\n%s\n",
                     type == GL_VERTEX_SHADER ? "vertex" : "fragment", log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

Shader::~Shader() {
    if (m_id) glDeleteProgram(m_id);
}

bool Shader::compile(const char* vertexSrc, const char* fragmentSrc) {
    unsigned int vs = compileStage(GL_VERTEX_SHADER, vertexSrc);
    unsigned int fs = compileStage(GL_FRAGMENT_SHADER, fragmentSrc);
    if (!vs || !fs) {
        if (vs) glDeleteShader(vs);
        if (fs) glDeleteShader(fs);
        return false;
    }

    m_id = glCreateProgram();
    glAttachShader(m_id, vs);
    glAttachShader(m_id, fs);
    glLinkProgram(m_id);

    int ok = 0;
    glGetProgramiv(m_id, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(m_id, sizeof(log), nullptr, log);
        std::fprintf(stderr, "[Shader] Erro ao linkar:\n%s\n", log);
        glDeleteProgram(m_id);
        m_id = 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return m_id != 0;
}

void Shader::use() const {
    glUseProgram(m_id);
}

void Shader::setMat4(const char* name, const Mat4& mat) const {
    int loc = glGetUniformLocation(m_id, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, mat.m);
}

void Shader::setVec4(const char* name, const Vec4& v) const {
    int loc = glGetUniformLocation(m_id, name);
    glUniform4f(loc, v.r, v.g, v.b, v.a);
}

void Shader::setInt(const char* name, int value) const {
    int loc = glGetUniformLocation(m_id, name);
    glUniform1i(loc, value);
}

} // namespace eng
