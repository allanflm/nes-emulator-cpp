#include "nes/Cpu6502.h"
#include "nes/Bus.h"

namespace nes {

Cpu6502::Cpu6502() {
    using a = Cpu6502;
    // Tabela de 256 opcodes: {mnemonico, operacao, modo de enderecamento, ciclos base}.
    // Posicoes nao oficiais ficam como "???" -> XXX (tratado como NOP de N bytes
    // simples). Cobre todas as instrucoes OFICIAIS do 6502 (suficiente p/ nestest).
    m_lookup = {
        {"BRK",&a::BRK,&a::IMM,7},{"ORA",&a::ORA,&a::IZX,6},{"???",&a::XXX,&a::IMP,2},{"???",&a::XXX,&a::IMP,8},{"???",&a::NOP,&a::IMP,3},{"ORA",&a::ORA,&a::ZP0,3},{"ASL",&a::ASL,&a::ZP0,5},{"???",&a::XXX,&a::IMP,5},{"PHP",&a::PHP,&a::IMP,3},{"ORA",&a::ORA,&a::IMM,2},{"ASL",&a::ASL,&a::IMP,2},{"???",&a::XXX,&a::IMP,2},{"???",&a::NOP,&a::IMP,4},{"ORA",&a::ORA,&a::ABS,4},{"ASL",&a::ASL,&a::ABS,6},{"???",&a::XXX,&a::IMP,6},
        {"BPL",&a::BPL,&a::REL,2},{"ORA",&a::ORA,&a::IZY,5},{"???",&a::XXX,&a::IMP,2},{"???",&a::XXX,&a::IMP,8},{"???",&a::NOP,&a::IMP,4},{"ORA",&a::ORA,&a::ZPX,4},{"ASL",&a::ASL,&a::ZPX,6},{"???",&a::XXX,&a::IMP,6},{"CLC",&a::CLC,&a::IMP,2},{"ORA",&a::ORA,&a::ABY,4},{"???",&a::NOP,&a::IMP,2},{"???",&a::XXX,&a::IMP,7},{"???",&a::NOP,&a::IMP,4},{"ORA",&a::ORA,&a::ABX,4},{"ASL",&a::ASL,&a::ABX,7},{"???",&a::XXX,&a::IMP,7},
        {"JSR",&a::JSR,&a::ABS,6},{"AND",&a::AND,&a::IZX,6},{"???",&a::XXX,&a::IMP,2},{"???",&a::XXX,&a::IMP,8},{"BIT",&a::BIT,&a::ZP0,3},{"AND",&a::AND,&a::ZP0,3},{"ROL",&a::ROL,&a::ZP0,5},{"???",&a::XXX,&a::IMP,5},{"PLP",&a::PLP,&a::IMP,4},{"AND",&a::AND,&a::IMM,2},{"ROL",&a::ROL,&a::IMP,2},{"???",&a::XXX,&a::IMP,2},{"BIT",&a::BIT,&a::ABS,4},{"AND",&a::AND,&a::ABS,4},{"ROL",&a::ROL,&a::ABS,6},{"???",&a::XXX,&a::IMP,6},
        {"BMI",&a::BMI,&a::REL,2},{"AND",&a::AND,&a::IZY,5},{"???",&a::XXX,&a::IMP,2},{"???",&a::XXX,&a::IMP,8},{"???",&a::NOP,&a::IMP,4},{"AND",&a::AND,&a::ZPX,4},{"ROL",&a::ROL,&a::ZPX,6},{"???",&a::XXX,&a::IMP,6},{"SEC",&a::SEC,&a::IMP,2},{"AND",&a::AND,&a::ABY,4},{"???",&a::NOP,&a::IMP,2},{"???",&a::XXX,&a::IMP,7},{"???",&a::NOP,&a::IMP,4},{"AND",&a::AND,&a::ABX,4},{"ROL",&a::ROL,&a::ABX,7},{"???",&a::XXX,&a::IMP,7},
        {"RTI",&a::RTI,&a::IMP,6},{"EOR",&a::EOR,&a::IZX,6},{"???",&a::XXX,&a::IMP,2},{"???",&a::XXX,&a::IMP,8},{"???",&a::NOP,&a::IMP,3},{"EOR",&a::EOR,&a::ZP0,3},{"LSR",&a::LSR,&a::ZP0,5},{"???",&a::XXX,&a::IMP,5},{"PHA",&a::PHA,&a::IMP,3},{"EOR",&a::EOR,&a::IMM,2},{"LSR",&a::LSR,&a::IMP,2},{"???",&a::XXX,&a::IMP,2},{"JMP",&a::JMP,&a::ABS,3},{"EOR",&a::EOR,&a::ABS,4},{"LSR",&a::LSR,&a::ABS,6},{"???",&a::XXX,&a::IMP,6},
        {"BVC",&a::BVC,&a::REL,2},{"EOR",&a::EOR,&a::IZY,5},{"???",&a::XXX,&a::IMP,2},{"???",&a::XXX,&a::IMP,8},{"???",&a::NOP,&a::IMP,4},{"EOR",&a::EOR,&a::ZPX,4},{"LSR",&a::LSR,&a::ZPX,6},{"???",&a::XXX,&a::IMP,6},{"CLI",&a::CLI,&a::IMP,2},{"EOR",&a::EOR,&a::ABY,4},{"???",&a::NOP,&a::IMP,2},{"???",&a::XXX,&a::IMP,7},{"???",&a::NOP,&a::IMP,4},{"EOR",&a::EOR,&a::ABX,4},{"LSR",&a::LSR,&a::ABX,7},{"???",&a::XXX,&a::IMP,7},
        {"RTS",&a::RTS,&a::IMP,6},{"ADC",&a::ADC,&a::IZX,6},{"???",&a::XXX,&a::IMP,2},{"???",&a::XXX,&a::IMP,8},{"???",&a::NOP,&a::IMP,3},{"ADC",&a::ADC,&a::ZP0,3},{"ROR",&a::ROR,&a::ZP0,5},{"???",&a::XXX,&a::IMP,5},{"PLA",&a::PLA,&a::IMP,4},{"ADC",&a::ADC,&a::IMM,2},{"ROR",&a::ROR,&a::IMP,2},{"???",&a::XXX,&a::IMP,2},{"JMP",&a::JMP,&a::IND,5},{"ADC",&a::ADC,&a::ABS,4},{"ROR",&a::ROR,&a::ABS,6},{"???",&a::XXX,&a::IMP,6},
        {"BVS",&a::BVS,&a::REL,2},{"ADC",&a::ADC,&a::IZY,5},{"???",&a::XXX,&a::IMP,2},{"???",&a::XXX,&a::IMP,8},{"???",&a::NOP,&a::IMP,4},{"ADC",&a::ADC,&a::ZPX,4},{"ROR",&a::ROR,&a::ZPX,6},{"???",&a::XXX,&a::IMP,6},{"SEI",&a::SEI,&a::IMP,2},{"ADC",&a::ADC,&a::ABY,4},{"???",&a::NOP,&a::IMP,2},{"???",&a::XXX,&a::IMP,7},{"???",&a::NOP,&a::IMP,4},{"ADC",&a::ADC,&a::ABX,4},{"ROR",&a::ROR,&a::ABX,7},{"???",&a::XXX,&a::IMP,7},
        {"???",&a::NOP,&a::IMP,2},{"STA",&a::STA,&a::IZX,6},{"???",&a::NOP,&a::IMP,2},{"???",&a::XXX,&a::IMP,6},{"STY",&a::STY,&a::ZP0,3},{"STA",&a::STA,&a::ZP0,3},{"STX",&a::STX,&a::ZP0,3},{"???",&a::XXX,&a::IMP,3},{"DEY",&a::DEY,&a::IMP,2},{"???",&a::NOP,&a::IMP,2},{"TXA",&a::TXA,&a::IMP,2},{"???",&a::XXX,&a::IMP,2},{"STY",&a::STY,&a::ABS,4},{"STA",&a::STA,&a::ABS,4},{"STX",&a::STX,&a::ABS,4},{"???",&a::XXX,&a::IMP,4},
        {"BCC",&a::BCC,&a::REL,2},{"STA",&a::STA,&a::IZY,6},{"???",&a::XXX,&a::IMP,2},{"???",&a::XXX,&a::IMP,6},{"STY",&a::STY,&a::ZPX,4},{"STA",&a::STA,&a::ZPX,4},{"STX",&a::STX,&a::ZPY,4},{"???",&a::XXX,&a::IMP,4},{"TYA",&a::TYA,&a::IMP,2},{"STA",&a::STA,&a::ABY,5},{"TXS",&a::TXS,&a::IMP,2},{"???",&a::XXX,&a::IMP,5},{"???",&a::NOP,&a::IMP,5},{"STA",&a::STA,&a::ABX,5},{"???",&a::XXX,&a::IMP,5},{"???",&a::XXX,&a::IMP,5},
        {"LDY",&a::LDY,&a::IMM,2},{"LDA",&a::LDA,&a::IZX,6},{"LDX",&a::LDX,&a::IMM,2},{"???",&a::XXX,&a::IMP,6},{"LDY",&a::LDY,&a::ZP0,3},{"LDA",&a::LDA,&a::ZP0,3},{"LDX",&a::LDX,&a::ZP0,3},{"???",&a::XXX,&a::IMP,3},{"TAY",&a::TAY,&a::IMP,2},{"LDA",&a::LDA,&a::IMM,2},{"TAX",&a::TAX,&a::IMP,2},{"???",&a::XXX,&a::IMP,2},{"LDY",&a::LDY,&a::ABS,4},{"LDA",&a::LDA,&a::ABS,4},{"LDX",&a::LDX,&a::ABS,4},{"???",&a::XXX,&a::IMP,4},
        {"BCS",&a::BCS,&a::REL,2},{"LDA",&a::LDA,&a::IZY,5},{"???",&a::XXX,&a::IMP,2},{"???",&a::XXX,&a::IMP,5},{"LDY",&a::LDY,&a::ZPX,4},{"LDA",&a::LDA,&a::ZPX,4},{"LDX",&a::LDX,&a::ZPY,4},{"???",&a::XXX,&a::IMP,4},{"CLV",&a::CLV,&a::IMP,2},{"LDA",&a::LDA,&a::ABY,4},{"TSX",&a::TSX,&a::IMP,2},{"???",&a::XXX,&a::IMP,4},{"LDY",&a::LDY,&a::ABX,4},{"LDA",&a::LDA,&a::ABX,4},{"LDX",&a::LDX,&a::ABY,4},{"???",&a::XXX,&a::IMP,4},
        {"CPY",&a::CPY,&a::IMM,2},{"CMP",&a::CMP,&a::IZX,6},{"???",&a::NOP,&a::IMP,2},{"???",&a::XXX,&a::IMP,8},{"CPY",&a::CPY,&a::ZP0,3},{"CMP",&a::CMP,&a::ZP0,3},{"DEC",&a::DEC,&a::ZP0,5},{"???",&a::XXX,&a::IMP,5},{"INY",&a::INY,&a::IMP,2},{"CMP",&a::CMP,&a::IMM,2},{"DEX",&a::DEX,&a::IMP,2},{"???",&a::XXX,&a::IMP,2},{"CPY",&a::CPY,&a::ABS,4},{"CMP",&a::CMP,&a::ABS,4},{"DEC",&a::DEC,&a::ABS,6},{"???",&a::XXX,&a::IMP,6},
        {"BNE",&a::BNE,&a::REL,2},{"CMP",&a::CMP,&a::IZY,5},{"???",&a::XXX,&a::IMP,2},{"???",&a::XXX,&a::IMP,8},{"???",&a::NOP,&a::IMP,4},{"CMP",&a::CMP,&a::ZPX,4},{"DEC",&a::DEC,&a::ZPX,6},{"???",&a::XXX,&a::IMP,6},{"CLD",&a::CLD,&a::IMP,2},{"CMP",&a::CMP,&a::ABY,4},{"NOP",&a::NOP,&a::IMP,2},{"???",&a::XXX,&a::IMP,7},{"???",&a::NOP,&a::IMP,4},{"CMP",&a::CMP,&a::ABX,4},{"DEC",&a::DEC,&a::ABX,7},{"???",&a::XXX,&a::IMP,7},
        {"CPX",&a::CPX,&a::IMM,2},{"SBC",&a::SBC,&a::IZX,6},{"???",&a::NOP,&a::IMP,2},{"???",&a::XXX,&a::IMP,8},{"CPX",&a::CPX,&a::ZP0,3},{"SBC",&a::SBC,&a::ZP0,3},{"INC",&a::INC,&a::ZP0,5},{"???",&a::XXX,&a::IMP,5},{"INX",&a::INX,&a::IMP,2},{"SBC",&a::SBC,&a::IMM,2},{"NOP",&a::NOP,&a::IMP,2},{"???",&a::SBC,&a::IMP,2},{"CPX",&a::CPX,&a::ABS,4},{"SBC",&a::SBC,&a::ABS,4},{"INC",&a::INC,&a::ABS,6},{"???",&a::XXX,&a::IMP,6},
        {"BEQ",&a::BEQ,&a::REL,2},{"SBC",&a::SBC,&a::IZY,5},{"???",&a::XXX,&a::IMP,2},{"???",&a::XXX,&a::IMP,8},{"???",&a::NOP,&a::IMP,4},{"SBC",&a::SBC,&a::ZPX,4},{"INC",&a::INC,&a::ZPX,6},{"???",&a::XXX,&a::IMP,6},{"SED",&a::SED,&a::IMP,2},{"SBC",&a::SBC,&a::ABY,4},{"NOP",&a::NOP,&a::IMP,2},{"???",&a::XXX,&a::IMP,7},{"???",&a::NOP,&a::IMP,4},{"SBC",&a::SBC,&a::ABX,4},{"INC",&a::INC,&a::ABX,7},{"???",&a::XXX,&a::IMP,7},
    };
}

uint8_t Cpu6502::read(uint16_t addr) { return m_bus->cpuRead(addr, false); }
void Cpu6502::write(uint16_t addr, uint8_t data) { m_bus->cpuWrite(addr, data); }

uint8_t Cpu6502::getFlag(Flags f) const { return (status & f) ? 1 : 0; }
void Cpu6502::setFlag(Flags f, bool v) {
    if (v) status |= f; else status &= ~f;
}

uint8_t Cpu6502::fetch() {
    if (m_lookup[opcode].addrmode != &Cpu6502::IMP)
        m_fetched = read(m_addrAbs);
    return m_fetched;
}

// ===================== Ciclo principal =====================

void Cpu6502::clock() {
    if (m_cycles == 0) {
        opcode = read(pc);
        setFlag(U, true);
        pc++;

        const Instruction& in = m_lookup[opcode];
        m_cycles = in.cycles;
        uint8_t extra1 = (this->*in.addrmode)();
        uint8_t extra2 = (this->*in.operate)();
        m_cycles += (extra1 & extra2);

        // Bits 5 (U) e 4 (B) nao existem de fato no registrador: na fronteira
        // de instrucao U le sempre 1 e B sempre 0 (B so vale quando empilhado).
        setFlag(U, true);
        setFlag(B, false);
    }
    m_cycles--;
    cycleCount++;
}

// ===================== Sinais =====================

void Cpu6502::reset() {
    m_addrAbs = 0xFFFC;
    uint16_t lo = read(m_addrAbs + 0);
    uint16_t hi = read(m_addrAbs + 1);
    pc = (hi << 8) | lo;

    a = 0; x = 0; y = 0;
    stkp = 0xFD;
    status = 0x00 | U | I;

    m_addrRel = 0x0000;
    m_addrAbs = 0x0000;
    m_fetched = 0x00;

    m_cycles = 8;
}

void Cpu6502::irq() {
    if (getFlag(I) == 0) {
        write(0x0100 + stkp, (pc >> 8) & 0x00FF); stkp--;
        write(0x0100 + stkp, pc & 0x00FF); stkp--;

        setFlag(B, false);
        setFlag(U, true);
        setFlag(I, true);
        write(0x0100 + stkp, status); stkp--;

        m_addrAbs = 0xFFFE;
        uint16_t lo = read(m_addrAbs + 0);
        uint16_t hi = read(m_addrAbs + 1);
        pc = (hi << 8) | lo;

        m_cycles = 7;
    }
}

void Cpu6502::nmi() {
    write(0x0100 + stkp, (pc >> 8) & 0x00FF); stkp--;
    write(0x0100 + stkp, pc & 0x00FF); stkp--;

    setFlag(B, false);
    setFlag(U, true);
    setFlag(I, true);
    write(0x0100 + stkp, status); stkp--;

    m_addrAbs = 0xFFFA;
    uint16_t lo = read(m_addrAbs + 0);
    uint16_t hi = read(m_addrAbs + 1);
    pc = (hi << 8) | lo;

    m_cycles = 8;
}

// ===================== Modos de enderecamento =====================

uint8_t Cpu6502::IMP() { m_fetched = a; return 0; }

uint8_t Cpu6502::IMM() { m_addrAbs = pc++; return 0; }

uint8_t Cpu6502::ZP0() {
    m_addrAbs = read(pc); pc++;
    m_addrAbs &= 0x00FF;
    return 0;
}

uint8_t Cpu6502::ZPX() {
    m_addrAbs = (read(pc) + x); pc++;
    m_addrAbs &= 0x00FF;
    return 0;
}

uint8_t Cpu6502::ZPY() {
    m_addrAbs = (read(pc) + y); pc++;
    m_addrAbs &= 0x00FF;
    return 0;
}

uint8_t Cpu6502::REL() {
    m_addrRel = read(pc); pc++;
    if (m_addrRel & 0x80) m_addrRel |= 0xFF00; // sinaliza (extensao de sinal)
    return 0;
}

uint8_t Cpu6502::ABS() {
    uint16_t lo = read(pc); pc++;
    uint16_t hi = read(pc); pc++;
    m_addrAbs = (hi << 8) | lo;
    return 0;
}

uint8_t Cpu6502::ABX() {
    uint16_t lo = read(pc); pc++;
    uint16_t hi = read(pc); pc++;
    m_addrAbs = ((hi << 8) | lo) + x;
    return ((m_addrAbs & 0xFF00) != (hi << 8)) ? 1 : 0;
}

uint8_t Cpu6502::ABY() {
    uint16_t lo = read(pc); pc++;
    uint16_t hi = read(pc); pc++;
    m_addrAbs = ((hi << 8) | lo) + y;
    return ((m_addrAbs & 0xFF00) != (hi << 8)) ? 1 : 0;
}

uint8_t Cpu6502::IND() {
    uint16_t lo = read(pc); pc++;
    uint16_t hi = read(pc); pc++;
    uint16_t ptr = (hi << 8) | lo;

    if (lo == 0x00FF) // bug de hardware na fronteira de pagina
        m_addrAbs = (read(ptr & 0xFF00) << 8) | read(ptr + 0);
    else
        m_addrAbs = (read(ptr + 1) << 8) | read(ptr + 0);
    return 0;
}

uint8_t Cpu6502::IZX() {
    uint16_t t = read(pc); pc++;
    uint16_t lo = read((uint16_t)(t + (uint16_t)x) & 0x00FF);
    uint16_t hi = read((uint16_t)(t + (uint16_t)x + 1) & 0x00FF);
    m_addrAbs = (hi << 8) | lo;
    return 0;
}

uint8_t Cpu6502::IZY() {
    uint16_t t = read(pc); pc++;
    uint16_t lo = read(t & 0x00FF);
    uint16_t hi = read((t + 1) & 0x00FF);
    m_addrAbs = ((hi << 8) | lo) + y;
    return ((m_addrAbs & 0xFF00) != (hi << 8)) ? 1 : 0;
}

// ===================== Helpers de branch =====================

static inline void doBranch(uint8_t& cycles, uint16_t& pc, uint16_t addrRel,
                            uint16_t& addrAbs) {
    cycles++;
    addrAbs = pc + addrRel;
    if ((addrAbs & 0xFF00) != (pc & 0xFF00)) cycles++;
    pc = addrAbs;
}

// ===================== Opcodes =====================

uint8_t Cpu6502::ADC() {
    fetch();
    m_temp = (uint16_t)a + (uint16_t)m_fetched + (uint16_t)getFlag(C);
    setFlag(C, m_temp > 255);
    setFlag(Z, (m_temp & 0x00FF) == 0);
    setFlag(V, (~((uint16_t)a ^ (uint16_t)m_fetched) & ((uint16_t)a ^ m_temp)) & 0x0080);
    setFlag(N, m_temp & 0x80);
    a = m_temp & 0x00FF;
    return 1;
}

uint8_t Cpu6502::SBC() {
    fetch();
    uint16_t value = ((uint16_t)m_fetched) ^ 0x00FF;
    m_temp = (uint16_t)a + value + (uint16_t)getFlag(C);
    setFlag(C, m_temp & 0xFF00);
    setFlag(Z, (m_temp & 0x00FF) == 0);
    setFlag(V, (m_temp ^ (uint16_t)a) & (m_temp ^ value) & 0x0080);
    setFlag(N, m_temp & 0x0080);
    a = m_temp & 0x00FF;
    return 1;
}

uint8_t Cpu6502::AND() {
    fetch();
    a = a & m_fetched;
    setFlag(Z, a == 0x00);
    setFlag(N, a & 0x80);
    return 1;
}

uint8_t Cpu6502::ASL() {
    fetch();
    m_temp = (uint16_t)m_fetched << 1;
    setFlag(C, (m_temp & 0xFF00) > 0);
    setFlag(Z, (m_temp & 0x00FF) == 0x00);
    setFlag(N, m_temp & 0x80);
    if (m_lookup[opcode].addrmode == &Cpu6502::IMP)
        a = m_temp & 0x00FF;
    else
        write(m_addrAbs, m_temp & 0x00FF);
    return 0;
}

uint8_t Cpu6502::BCC() { if (getFlag(C) == 0) doBranch(m_cycles, pc, m_addrRel, m_addrAbs); return 0; }
uint8_t Cpu6502::BCS() { if (getFlag(C) == 1) doBranch(m_cycles, pc, m_addrRel, m_addrAbs); return 0; }
uint8_t Cpu6502::BEQ() { if (getFlag(Z) == 1) doBranch(m_cycles, pc, m_addrRel, m_addrAbs); return 0; }
uint8_t Cpu6502::BNE() { if (getFlag(Z) == 0) doBranch(m_cycles, pc, m_addrRel, m_addrAbs); return 0; }
uint8_t Cpu6502::BMI() { if (getFlag(N) == 1) doBranch(m_cycles, pc, m_addrRel, m_addrAbs); return 0; }
uint8_t Cpu6502::BPL() { if (getFlag(N) == 0) doBranch(m_cycles, pc, m_addrRel, m_addrAbs); return 0; }
uint8_t Cpu6502::BVC() { if (getFlag(V) == 0) doBranch(m_cycles, pc, m_addrRel, m_addrAbs); return 0; }
uint8_t Cpu6502::BVS() { if (getFlag(V) == 1) doBranch(m_cycles, pc, m_addrRel, m_addrAbs); return 0; }

uint8_t Cpu6502::BIT() {
    fetch();
    m_temp = a & m_fetched;
    setFlag(Z, (m_temp & 0x00FF) == 0x00);
    setFlag(N, m_fetched & (1 << 7));
    setFlag(V, m_fetched & (1 << 6));
    return 0;
}

uint8_t Cpu6502::BRK() {
    pc++;
    setFlag(I, true);
    write(0x0100 + stkp, (pc >> 8) & 0x00FF); stkp--;
    write(0x0100 + stkp, pc & 0x00FF); stkp--;

    setFlag(B, true);
    write(0x0100 + stkp, status); stkp--;
    setFlag(B, false);

    pc = (uint16_t)read(0xFFFE) | ((uint16_t)read(0xFFFF) << 8);
    return 0;
}

uint8_t Cpu6502::CLC() { setFlag(C, false); return 0; }
uint8_t Cpu6502::CLD() { setFlag(D, false); return 0; }
uint8_t Cpu6502::CLI() { setFlag(I, false); return 0; }
uint8_t Cpu6502::CLV() { setFlag(V, false); return 0; }

uint8_t Cpu6502::CMP() {
    fetch();
    m_temp = (uint16_t)a - (uint16_t)m_fetched;
    setFlag(C, a >= m_fetched);
    setFlag(Z, (m_temp & 0x00FF) == 0x0000);
    setFlag(N, m_temp & 0x0080);
    return 1;
}

uint8_t Cpu6502::CPX() {
    fetch();
    m_temp = (uint16_t)x - (uint16_t)m_fetched;
    setFlag(C, x >= m_fetched);
    setFlag(Z, (m_temp & 0x00FF) == 0x0000);
    setFlag(N, m_temp & 0x0080);
    return 0;
}

uint8_t Cpu6502::CPY() {
    fetch();
    m_temp = (uint16_t)y - (uint16_t)m_fetched;
    setFlag(C, y >= m_fetched);
    setFlag(Z, (m_temp & 0x00FF) == 0x0000);
    setFlag(N, m_temp & 0x0080);
    return 0;
}

uint8_t Cpu6502::DEC() {
    fetch();
    m_temp = m_fetched - 1;
    write(m_addrAbs, m_temp & 0x00FF);
    setFlag(Z, (m_temp & 0x00FF) == 0x0000);
    setFlag(N, m_temp & 0x0080);
    return 0;
}

uint8_t Cpu6502::DEX() { x--; setFlag(Z, x == 0x00); setFlag(N, x & 0x80); return 0; }
uint8_t Cpu6502::DEY() { y--; setFlag(Z, y == 0x00); setFlag(N, y & 0x80); return 0; }

uint8_t Cpu6502::EOR() {
    fetch();
    a = a ^ m_fetched;
    setFlag(Z, a == 0x00);
    setFlag(N, a & 0x80);
    return 1;
}

uint8_t Cpu6502::INC() {
    fetch();
    m_temp = m_fetched + 1;
    write(m_addrAbs, m_temp & 0x00FF);
    setFlag(Z, (m_temp & 0x00FF) == 0x0000);
    setFlag(N, m_temp & 0x0080);
    return 0;
}

uint8_t Cpu6502::INX() { x++; setFlag(Z, x == 0x00); setFlag(N, x & 0x80); return 0; }
uint8_t Cpu6502::INY() { y++; setFlag(Z, y == 0x00); setFlag(N, y & 0x80); return 0; }

uint8_t Cpu6502::JMP() { pc = m_addrAbs; return 0; }

uint8_t Cpu6502::JSR() {
    pc--;
    write(0x0100 + stkp, (pc >> 8) & 0x00FF); stkp--;
    write(0x0100 + stkp, pc & 0x00FF); stkp--;
    pc = m_addrAbs;
    return 0;
}

uint8_t Cpu6502::LDA() {
    fetch();
    a = m_fetched;
    setFlag(Z, a == 0x00);
    setFlag(N, a & 0x80);
    return 1;
}

uint8_t Cpu6502::LDX() {
    fetch();
    x = m_fetched;
    setFlag(Z, x == 0x00);
    setFlag(N, x & 0x80);
    return 1;
}

uint8_t Cpu6502::LDY() {
    fetch();
    y = m_fetched;
    setFlag(Z, y == 0x00);
    setFlag(N, y & 0x80);
    return 1;
}

uint8_t Cpu6502::LSR() {
    fetch();
    setFlag(C, m_fetched & 0x0001);
    m_temp = m_fetched >> 1;
    setFlag(Z, (m_temp & 0x00FF) == 0x0000);
    setFlag(N, m_temp & 0x0080);
    if (m_lookup[opcode].addrmode == &Cpu6502::IMP)
        a = m_temp & 0x00FF;
    else
        write(m_addrAbs, m_temp & 0x00FF);
    return 0;
}

uint8_t Cpu6502::NOP() {
    // Alguns NOPs nao oficiais consomem ciclo extra com cruzamento de pagina.
    switch (opcode) {
        case 0x1C: case 0x3C: case 0x5C: case 0x7C: case 0xDC: case 0xFC:
            return 1;
        default:
            return 0;
    }
}

uint8_t Cpu6502::ORA() {
    fetch();
    a = a | m_fetched;
    setFlag(Z, a == 0x00);
    setFlag(N, a & 0x80);
    return 1;
}

uint8_t Cpu6502::PHA() {
    write(0x0100 + stkp, a); stkp--;
    return 0;
}

uint8_t Cpu6502::PHP() {
    write(0x0100 + stkp, status | B | U); stkp--;
    setFlag(B, false);
    setFlag(U, false);
    return 0;
}

uint8_t Cpu6502::PLA() {
    stkp++;
    a = read(0x0100 + stkp);
    setFlag(Z, a == 0x00);
    setFlag(N, a & 0x80);
    return 0;
}

uint8_t Cpu6502::PLP() {
    stkp++;
    status = read(0x0100 + stkp);
    setFlag(U, true);
    return 0;
}

uint8_t Cpu6502::ROL() {
    fetch();
    m_temp = (uint16_t)(m_fetched << 1) | getFlag(C);
    setFlag(C, m_temp & 0xFF00);
    setFlag(Z, (m_temp & 0x00FF) == 0x0000);
    setFlag(N, m_temp & 0x0080);
    if (m_lookup[opcode].addrmode == &Cpu6502::IMP)
        a = m_temp & 0x00FF;
    else
        write(m_addrAbs, m_temp & 0x00FF);
    return 0;
}

uint8_t Cpu6502::ROR() {
    fetch();
    m_temp = (uint16_t)(getFlag(C) << 7) | (m_fetched >> 1);
    setFlag(C, m_fetched & 0x01);
    setFlag(Z, (m_temp & 0x00FF) == 0x00);
    setFlag(N, m_temp & 0x0080);
    if (m_lookup[opcode].addrmode == &Cpu6502::IMP)
        a = m_temp & 0x00FF;
    else
        write(m_addrAbs, m_temp & 0x00FF);
    return 0;
}

uint8_t Cpu6502::RTI() {
    stkp++;
    status = read(0x0100 + stkp);
    status &= ~B;
    status &= ~U;

    stkp++;
    pc = (uint16_t)read(0x0100 + stkp);
    stkp++;
    pc |= (uint16_t)read(0x0100 + stkp) << 8;
    return 0;
}

uint8_t Cpu6502::RTS() {
    stkp++;
    pc = (uint16_t)read(0x0100 + stkp);
    stkp++;
    pc |= (uint16_t)read(0x0100 + stkp) << 8;
    pc++;
    return 0;
}

uint8_t Cpu6502::SEC() { setFlag(C, true); return 0; }
uint8_t Cpu6502::SED() { setFlag(D, true); return 0; }
uint8_t Cpu6502::SEI() { setFlag(I, true); return 0; }

uint8_t Cpu6502::STA() { write(m_addrAbs, a); return 0; }
uint8_t Cpu6502::STX() { write(m_addrAbs, x); return 0; }
uint8_t Cpu6502::STY() { write(m_addrAbs, y); return 0; }

uint8_t Cpu6502::TAX() { x = a; setFlag(Z, x == 0x00); setFlag(N, x & 0x80); return 0; }
uint8_t Cpu6502::TAY() { y = a; setFlag(Z, y == 0x00); setFlag(N, y & 0x80); return 0; }
uint8_t Cpu6502::TSX() { x = stkp; setFlag(Z, x == 0x00); setFlag(N, x & 0x80); return 0; }
uint8_t Cpu6502::TXA() { a = x; setFlag(Z, a == 0x00); setFlag(N, a & 0x80); return 0; }
uint8_t Cpu6502::TXS() { stkp = x; return 0; }
uint8_t Cpu6502::TYA() { a = y; setFlag(Z, a == 0x00); setFlag(N, a & 0x80); return 0; }

uint8_t Cpu6502::XXX() { return 0; }

} // namespace nes
