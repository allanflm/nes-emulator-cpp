#include "engine/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/gl.h>
#include <cstdio>

namespace eng {

Texture::~Texture() {
    if (m_id) glDeleteTextures(1, &m_id);
}

bool Texture::loadFromFile(const char* path) {
    stbi_set_flip_vertically_on_load(false);
    int channels = 0;
    unsigned char* data = stbi_load(path, &m_width, &m_height, &channels, 4);
    if (!data) {
        std::fprintf(stderr, "[Texture] Falha ao carregar '%s'\n", path);
        return false;
    }

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    return true;
}

bool Texture::createEmpty(int w, int h) {
    if (m_id) { glDeleteTextures(1, &m_id); m_id = 0; }
    m_width = w;
    m_height = h;

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    return true;
}

void Texture::update(const uint32_t* pixels) {
    if (!m_id) return;
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height,
                    GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

void Texture::bind(unsigned int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

} // namespace eng
