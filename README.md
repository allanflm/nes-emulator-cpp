# engine-cpp

Projeto de estudo de **gamedev em C++ do zero**, com duas frentes que compartilham a mesma base:

1. **Engine 2D** (GLFW + OpenGL 3.3) com um **protótipo de jogo de plataforma**.
2. **Emulador de NES** ("from scratch") que reaproveita a camada de vídeo/input do engine pra rodar ROMs `.nes` reais.

> Feito de forma incremental, passo a passo, com cada parte testada antes de seguir.

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

Isso gera dois executáveis em `build/`:

| Executável | O que é |
|---|---|
| `engine` | Protótipo de plataforma 2D (anda, pula, colisão AABB) |
| `nes`    | Emulador de NES |

## Rodando

**Platformer:**
```sh
./build/engine
```
Controles: `A`/`D` ou setas = mover · `Espaço`/`W`/`Cima` = pular · `Esc` = sair.

**Emulador — teste da CPU** (valida o 6502 contra um log de referência):
```sh
./build/nes --nestest test/nestest/nestest.nes test/nestest/nestest.log
```

## Estado atual

### Engine 2D
- [x] Janela + contexto OpenGL 3.3 (GLFW + glad)
- [x] Renderer de sprites/quads (VAO/VBO, shaders, câmera ortográfica)
- [x] Input com edge-trigger, loop com timestep fixo
- [x] Protótipo de plataforma com gravidade e colisão AABB

### Emulador NES
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
  engine/     # camada reutilizável: Window, Input, Shader, Texture, SpriteRenderer, ...
  game/       # protótipo de plataforma 2D
  nes/        # emulador: Bus, Cpu6502, Cartridge, mappers/, (PPU em breve)
  main.cpp        # entrada do engine (platformer)
  nes_main.cpp    # entrada do emulador
vendor/       # stb_image.h
test/         # ROM e log de teste (nestest)
```

## Créditos / referências

- Série **"NES Emulator From Scratch"** do **javidx9 (OneLoneCoder)** — inspiração e referência da arquitetura.
- **nestest** — ROM de teste da CPU 6502 (domínio comum na comunidade de emulação).
