#pragma once
// Mapper 0 (NROM): o mais simples. PRG-ROM fixa em $8000-$FFFF (16KB espelhado
// ou 32KB), CHR-ROM fixa em $0000-$1FFF. Cobre Donkey Kong, Super Mario Bros,
// Balloon Fight, e dezenas de jogos da primeira geracao.

#include "nes/mappers/Mapper.h"

namespace nes {

class Mapper000 : public Mapper {
public:
    Mapper000(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks) {}

    bool cpuMapRead(uint16_t addr, uint32_t& mapped) override;
    bool cpuMapWrite(uint16_t addr, uint32_t& mapped) override;
    bool ppuMapRead(uint16_t addr, uint32_t& mapped) override;
    bool ppuMapWrite(uint16_t addr, uint32_t& mapped) override;
};

} // namespace nes
