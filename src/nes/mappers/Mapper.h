#pragma once
// Interface de mapper. O mapper traduz enderecos do barramento (CPU/PPU)
// para offsets dentro da PRG-ROM / CHR-ROM do cartucho. Cada jogo usa um.

#include <cstdint>

namespace nes {

class Mapper {
public:
    Mapper(uint8_t prgBanks, uint8_t chrBanks)
        : m_prgBanks(prgBanks), m_chrBanks(chrBanks) {}
    virtual ~Mapper() = default;

    // Traduzem 'addr' do barramento em 'mapped' (offset na ROM). true se mapeou.
    virtual bool cpuMapRead(uint16_t addr, uint32_t& mapped) = 0;
    virtual bool cpuMapWrite(uint16_t addr, uint32_t& mapped) = 0;
    virtual bool ppuMapRead(uint16_t addr, uint32_t& mapped) = 0;
    virtual bool ppuMapWrite(uint16_t addr, uint32_t& mapped) = 0;

protected:
    uint8_t m_prgBanks = 0;
    uint8_t m_chrBanks = 0;
};

} // namespace nes
