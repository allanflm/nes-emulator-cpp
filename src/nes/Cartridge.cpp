#include "nes/Cartridge.h"
#include "nes/mappers/Mapper000.h"

#include <cstdio>
#include <fstream>

namespace nes {

// Cabecalho iNES (16 bytes).
struct INesHeader {
    char     name[4];      // "NES\x1A"
    uint8_t  prgChunks;    // num. de bancos de 16KB de PRG-ROM
    uint8_t  chrChunks;    // num. de bancos de 8KB de CHR-ROM
    uint8_t  flags6;
    uint8_t  flags7;
    uint8_t  prgRamSize;
    uint8_t  tvSystem1;
    uint8_t  tvSystem2;
    char     unused[5];
};

Cartridge::Cartridge(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f.is_open()) {
        std::fprintf(stderr, "[Cartridge] Nao abriu '%s'\n", path.c_str());
        return;
    }

    INesHeader header{};
    f.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (header.name[0] != 'N' || header.name[1] != 'E' ||
        header.name[2] != 'S' || (uint8_t)header.name[3] != 0x1A) {
        std::fprintf(stderr, "[Cartridge] '%s' nao e um arquivo iNES valido\n",
                     path.c_str());
        return;
    }

    // Pula o "trainer" de 512 bytes, se presente.
    if (header.flags6 & 0x04)
        f.seekg(512, std::ios_base::cur);

    m_mapperId = ((header.flags7 >> 4) << 4) | (header.flags6 >> 4);
    m_mirror = (header.flags6 & 0x01) ? Mirror::Vertical : Mirror::Horizontal;

    m_prgBanks = header.prgChunks;
    m_chrBanks = header.chrChunks;

    m_prg.resize((size_t)m_prgBanks * 16384);
    f.read(reinterpret_cast<char*>(m_prg.data()), m_prg.size());

    if (m_chrBanks == 0)
        m_chr.resize(8192); // CHR-RAM
    else
        m_chr.resize((size_t)m_chrBanks * 8192);
    f.read(reinterpret_cast<char*>(m_chr.data()), m_chr.size());

    switch (m_mapperId) {
        case 0: m_mapper = std::make_shared<Mapper000>(m_prgBanks, m_chrBanks); break;
        default:
            std::fprintf(stderr, "[Cartridge] Mapper %d ainda nao suportado\n",
                         m_mapperId);
            return;
    }

    m_valid = true;
    std::printf("[Cartridge] OK: mapper %d, PRG %dx16KB, CHR %dx8KB, mirror %s\n",
                m_mapperId, m_prgBanks, m_chrBanks,
                m_mirror == Mirror::Vertical ? "V" : "H");
}

bool Cartridge::cpuRead(uint16_t addr, uint8_t& data) {
    uint32_t mapped = 0;
    if (m_mapper && m_mapper->cpuMapRead(addr, mapped)) {
        if (mapped < m_prg.size()) data = m_prg[mapped];
        return true;
    }
    return false;
}

bool Cartridge::cpuWrite(uint16_t addr, uint8_t data) {
    uint32_t mapped = 0;
    if (m_mapper && m_mapper->cpuMapWrite(addr, mapped)) {
        if (mapped < m_prg.size()) m_prg[mapped] = data;
        return true;
    }
    return false;
}

bool Cartridge::ppuRead(uint16_t addr, uint8_t& data) {
    uint32_t mapped = 0;
    if (m_mapper && m_mapper->ppuMapRead(addr, mapped)) {
        if (mapped < m_chr.size()) data = m_chr[mapped];
        return true;
    }
    return false;
}

bool Cartridge::ppuWrite(uint16_t addr, uint8_t data) {
    uint32_t mapped = 0;
    if (m_mapper && m_mapper->ppuMapWrite(addr, mapped)) {
        if (mapped < m_chr.size()) m_chr[mapped] = data;
        return true;
    }
    return false;
}

} // namespace nes
