#pragma once
// Barramento (a "placa-mae"): 2KB de RAM da CPU e o mapa de memoria.
// Conecta CPU, cartucho e (a partir do M3) a PPU. Tambem dirige o clock do
// sistema, onde a PPU roda 3x mais rapido que a CPU.

#include <cstdint>
#include "nes/Cpu6502.h"

namespace nes {

class Cartridge;

class Bus {
public:
    Bus();

    void connectCartridge(Cartridge* cart) { m_cart = cart; }
    void reset();
    void clock(); // 1 tick do sistema (PPU 3x; CPU a cada 3)

    // Mapa de memoria da CPU.
    uint8_t cpuRead(uint16_t addr, bool readOnly = false);
    void    cpuWrite(uint16_t addr, uint8_t data);

    Cpu6502 cpu;

private:
    uint8_t m_cpuRam[2048] = {0};
    Cartridge* m_cart = nullptr;
    uint32_t m_systemClock = 0;
};

} // namespace nes
