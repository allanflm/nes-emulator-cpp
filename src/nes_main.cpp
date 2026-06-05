// Entrada do nes.exe — camada de apresentacao do emulador, reusando o engine.
//
// M0: por enquanto so prova o pipeline de video — gera um framebuffer 256x240
// (xadrez + gradiente com a paleta do NES) e o desenha na janela em escala
// inteira. Nas proximas milestones, a PPU passa a preencher esse framebuffer.

#include "engine/Window.h"
#include "engine/Input.h"
#include "engine/Texture.h"
#include "engine/SpriteRenderer.h"
#include "engine/Camera2D.h"
#include "nes/Palette.h"
#include "nes/Bus.h"
#include "nes/Cartridge.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>

static constexpr int kNesW = 256;
static constexpr int kNesH = 240;

// Preenche um padrao de teste no framebuffer (sera substituido pela PPU).
static void fillTestPattern(std::vector<uint32_t>& fb, double t) {
    int scroll = (int)(t * 20.0);
    for (int y = 0; y < kNesH; ++y) {
        for (int x = 0; x < kNesW; ++x) {
            // xadrez de 16x16 + gradiente animado usando a paleta real do NES.
            bool checker = (((x >> 4) + (y >> 4)) & 1) != 0;
            uint8_t paletteIdx = (uint8_t)(((x + scroll) >> 3) & 0x3F);
            uint32_t color = checker ? nes::nesColor(paletteIdx)
                                     : nes::nesColor((paletteIdx + 0x20) & 0x3F);
            fb[y * kNesW + x] = color;
        }
    }
}

// Calcula posicao/tamanho pra desenhar o framebuffer centralizado, com a maior
// escala INTEIRA que cabe na janela (mantem pixels nitidos e proporcao 256x240).
static void computeViewport(int winW, int winH, eng::Vec2& pos, eng::Vec2& size) {
    int scaleX = winW / kNesW;
    int scaleY = winH / kNesH;
    int scale = scaleX < scaleY ? scaleX : scaleY;
    if (scale < 1) scale = 1;
    float w = (float)(kNesW * scale);
    float h = (float)(kNesH * scale);
    pos = { (winW - w) * 0.5f, (winH - h) * 0.5f };
    size = { w, h };
}

// --------- Validacao da CPU contra o nestest.log (modo headless) ----------

// Extrai um campo hex como "A:1F" do log. Retorna -1 se nao achar.
static long logHex(const std::string& line, const char* key) {
    size_t p = line.find(key);
    if (p == std::string::npos) return -1;
    return std::strtol(line.c_str() + p + std::strlen(key), nullptr, 16);
}
static long logDec(const std::string& line, const char* key) {
    size_t p = line.find(key);
    if (p == std::string::npos) return -1;
    return std::strtol(line.c_str() + p + std::strlen(key), nullptr, 10);
}

static int runNestest(const char* romPath, const char* logPath) {
    nes::Cartridge cart(romPath);
    if (!cart.valid()) return 1;

    std::ifstream log(logPath);
    if (!log.is_open()) {
        std::fprintf(stderr, "Nao abriu o log '%s'\n", logPath);
        return 1;
    }

    nes::Bus bus;
    bus.connectCartridge(&cart);
    bus.reset();
    while (!bus.cpu.complete()) bus.cpu.clock(); // termina os ciclos do reset

    // Modo "automation" do nestest: comeca em $C000 com estado conhecido.
    bus.cpu.pc = 0xC000;
    bus.cpu.a = 0; bus.cpu.x = 0; bus.cpu.y = 0;
    bus.cpu.stkp = 0xFD;
    bus.cpu.status = 0x24; // U + I
    bus.cpu.cycleCount = 7;

    std::string line;
    long count = 0;
    while (std::getline(log, line)) {
        if (line.size() < 4) continue;

        // O nestest marca opcodes NAO-oficiais com '*'. Nosso nucleo implementa
        // apenas os oficiais (suficiente p/ jogos NROM), entao paramos aqui.
        if (line.find('*') != std::string::npos) {
            std::printf("nestest: OK! %ld instrucoes OFICIAIS conferem com o log.\n", count);
            std::printf("(parando na primeira instrucao nao-oficial, fora de escopo)\n");
            return 0;
        }

        long ePC  = std::strtol(line.substr(0, 4).c_str(), nullptr, 16);
        long eA   = logHex(line, "A:");
        long eX   = logHex(line, "X:");
        long eY   = logHex(line, "Y:");
        long eP   = logHex(line, "P:");
        long eSP  = logHex(line, "SP:");
        long eCYC = logDec(line, "CYC:");

        bool match = (bus.cpu.pc == ePC) && (bus.cpu.a == eA) &&
                     (bus.cpu.x == eX) && (bus.cpu.y == eY) &&
                     (bus.cpu.status == eP) && (bus.cpu.stkp == eSP) &&
                     ((long)bus.cpu.cycleCount == eCYC);

        if (!match) {
            std::printf("DIVERGENCIA na linha %ld:\n", count + 1);
            std::printf("  log : PC:%04lX A:%02lX X:%02lX Y:%02lX P:%02lX SP:%02lX CYC:%ld\n",
                        ePC, eA, eX, eY, eP, eSP, eCYC);
            std::printf("  meu : PC:%04X A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%u\n",
                        bus.cpu.pc, bus.cpu.a, bus.cpu.x, bus.cpu.y,
                        bus.cpu.status, bus.cpu.stkp, bus.cpu.cycleCount);
            std::printf("  (passou %ld instrucoes antes de divergir)\n", count);
            return 2;
        }

        // Executa uma instrucao completa.
        do { bus.cpu.clock(); } while (!bus.cpu.complete());
        count++;
    }

    std::printf("nestest: OK! %ld instrucoes conferem com o log de referencia.\n", count);
    return 0;
}

int main(int argc, char** argv) {
    if (argc >= 4 && std::strcmp(argv[1], "--nestest") == 0)
        return runNestest(argv[2], argv[3]);

    eng::Window window(kNesW * 3, kNesH * 3, "nes-emu :: M0 (teste de video)");
    if (!window.ok()) {
        std::fprintf(stderr, "Falha ao abrir a janela.\n");
        return 1;
    }

    eng::SpriteRenderer renderer;
    if (!renderer.init()) {
        std::fprintf(stderr, "Falha ao iniciar o renderer.\n");
        return 1;
    }

    eng::Texture screen;
    screen.createEmpty(kNesW, kNesH);

    std::vector<uint32_t> framebuffer(kNesW * kNesH, 0);

    eng::Camera2D camera(window.width(), window.height());

    std::printf("M0: framebuffer de teste %dx%d. Esc para sair.\n", kNesW, kNesH);

    while (!window.shouldClose()) {
        window.pollEvents();

        if (eng::Input::wasPressed(GLFW_KEY_ESCAPE))
            window.close();

        fillTestPattern(framebuffer, glfwGetTime());
        screen.update(framebuffer.data());

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        eng::Vec2 pos, size;
        computeViewport(window.width(), window.height(), pos, size);

        renderer.begin(camera.projection());
        renderer.drawSprite(pos, size, screen);

        eng::Input::endFrame();
        window.swapBuffers();
    }

    return 0;
}
