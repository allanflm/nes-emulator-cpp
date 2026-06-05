#include "nes/mappers/Mapper000.h"

namespace nes {

bool Mapper000::cpuMapRead(uint16_t addr, uint32_t& mapped) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        // 16KB (1 banco) espelha; 32KB (2 bancos) usa o endereco cheio.
        mapped = addr & (m_prgBanks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }
    return false;
}

bool Mapper000::cpuMapWrite(uint16_t addr, uint32_t& mapped) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        mapped = addr & (m_prgBanks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }
    return false;
}

bool Mapper000::ppuMapRead(uint16_t addr, uint32_t& mapped) {
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        mapped = addr;
        return true;
    }
    return false;
}

bool Mapper000::ppuMapWrite(uint16_t addr, uint32_t& mapped) {
    // CHR-RAM: se o cartucho nao tem CHR-ROM (chrBanks==0), permite escrita.
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        if (m_chrBanks == 0) { mapped = addr; return true; }
    }
    return false;
}

} // namespace nes
