# nes-emulator-cpp

Um **emulador de NES (Nintendinho) escrito em C++ do zero**, feito de forma incremental e passo a passo — cada parte testada antes de seguir para a próxima.

A camada de vídeo/input usa **GLFW + OpenGL 3.3** (em `src/engine/`), que faz o papel da janela, do desenho dos pixels na tela e da leitura do teclado — equivalente à `olcPixelGameEngine` usada na série que inspirou o projeto.

## Requisitos

- **CMake** ≥ 3.16
- **Ninja**
- **Compilador C++17** (testado com MinGW-w64 GCC 16 / WinLibs no Windows)

As dependências (**GLFW** e **glad**) são baixadas automaticamente pelo CMake via `FetchContent` — não precisa instalar nada à mão.

## Como compilar

```sh
cmake -S . -B build -G Ninja
cmake --build build
```

Gera o executável `nes` em `build/`.

## Rodando

**Teste da CPU** (valida o 6502 ciclo-a-ciclo contra um log de referência — roda no terminal, sem janela):
```sh
./build/nes --nestest test/nestest/nestest.nes test/nestest/nestest.log
```

**Janela** (por enquanto mostra um padrão de teste de vídeo; a imagem de jogo chega quando a PPU estiver pronta):
```sh
./build/nes
```

## Estado atual

- [x] **Camada de vídeo/input** — janela OpenGL 3.3, renderer de framebuffer, teclado
- [x] **CPU 6502** — todos os opcodes oficiais (validados ciclo-a-ciclo com `nestest.log`)
- [x] **Bus** + mapa de memória da CPU
- [x] **Cartridge** (formato iNES) + **Mapper 0** (NROM)
- [ ] **PPU** (vídeo) — background
- [ ] **PPU** — sprites + scrolling
- [ ] **Controles**
- [ ] APU (áudio) — opcional

## Estrutura

```
src/
  engine/     # camada de apresentacao: Window, Input, Shader, Texture, SpriteRenderer, Camera2D, Math
  nes/        # emulador: Bus, Cpu6502, Cartridge, mappers/, (PPU em breve)
  nes_main.cpp    # ponto de entrada
vendor/       # stb_image.h
test/         # ROM e log de teste (nestest)
```

## Créditos / referências

- Série **"NES Emulator From Scratch"** do **javidx9 (OneLoneCoder)** — inspiração e referência da arquitetura.
- **nestest** — ROM de teste da CPU 6502 (amplamente usada na comunidade de emulação).
