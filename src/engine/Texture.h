#pragma once
// Textura 2D OpenGL. Carrega PNG/JPG via stb_image (opcional no prototipo —
// o jogo comeca com quads coloridos, mas o suporte ja fica pronto).
// Tambem serve de framebuffer dinamico (createEmpty + update), usado pelo
// emulador NES pra subir os 256x240 pixels gerados pela PPU a cada frame.

#include <cstdint>

namespace eng {

class Texture {
public:
    Texture() = default;
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // Carrega de arquivo. Retorna false em erro.
    bool loadFromFile(const char* path);

    // Cria uma textura RGBA8 vazia (framebuffer dinamico), filtro NEAREST.
    bool createEmpty(int w, int h);

    // Atualiza todos os pixels (RGBA, 8 bits por canal). Requer createEmpty antes.
    // 'pixels' deve ter width*height elementos.
    void update(const uint32_t* pixels);

    void bind(unsigned int unit = 0) const;

    int width() const { return m_width; }
    int height() const { return m_height; }
    bool valid() const { return m_id != 0; }

private:
    unsigned int m_id = 0;
    int m_width = 0;
    int m_height = 0;
};

} // namespace eng
