#pragma once
// Cartucho: le um arquivo .nes (formato iNES), separa PRG-ROM e CHR-ROM,
// descobre o mapper e o tipo de espelhamento de nametable, e expoe acesso
// de leitura/escrita para CPU e PPU (delegando a traducao ao mapper).

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace nes {

class Mapper;

class Cartridge {
public:
    enum class Mirror { Horizontal, Vertical, OneScreenLo, OneScreenHi };

    explicit Cartridge(const std::string& path);

    bool valid() const { return m_valid; }
    Mirror mirror() const { return m_mirror; }

    // Acesso pela CPU ($4020-$FFFF). Retorna true se o cartucho respondeu.
    bool cpuRead(uint16_t addr, uint8_t& data);
    bool cpuWrite(uint16_t addr, uint8_t data);

    // Acesso pela PPU ($0000-$1FFF, pattern tables).
    bool ppuRead(uint16_t addr, uint8_t& data);
    bool ppuWrite(uint16_t addr, uint8_t data);

private:
    bool m_valid = false;
    Mirror m_mirror = Mirror::Horizontal;

    uint8_t m_mapperId = 0;
    uint8_t m_prgBanks = 0;
    uint8_t m_chrBanks = 0;

    std::vector<uint8_t> m_prg;
    std::vector<uint8_t> m_chr;
    std::shared_ptr<Mapper> m_mapper;
};

} // namespace nes
