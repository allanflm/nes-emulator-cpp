#include "nes/Bus.h"
#include "nes/Cartridge.h"

namespace nes {

Bus::Bus() {
    cpu.connectBus(this);
}

void Bus::reset() {
    cpu.reset();
    m_systemClock = 0;
}

void Bus::clock() {
    // No M1 o clock do sistema ainda nao roda a PPU (ela chega no M3).
    // Por enquanto so a CPU avanca, 1 ciclo por tick.
    cpu.clock();
    m_systemClock++;
}

uint8_t Bus::cpuRead(uint16_t addr, bool readOnly) {
    (void)readOnly;
    uint8_t data = 0x00;

    if (m_cart && m_cart->cpuRead(addr, data)) {
        // cartucho respondeu ($4020-$FFFF)
    } else if (addr <= 0x1FFF) {
        data = m_cpuRam[addr & 0x07FF]; // 2KB espelhados ate $1FFF
    } else if (addr >= 0x2000 && addr <= 0x3FFF) {
        // registradores da PPU — implementados no M3
    }
    return data;
}

void Bus::cpuWrite(uint16_t addr, uint8_t data) {
    if (m_cart && m_cart->cpuWrite(addr, data)) {
        // cartucho respondeu
    } else if (addr <= 0x1FFF) {
        m_cpuRam[addr & 0x07FF] = data;
    } else if (addr >= 0x2000 && addr <= 0x3FFF) {
        // registradores da PPU — M3
    }
}

} // namespace nes
