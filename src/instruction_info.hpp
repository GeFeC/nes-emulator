#pragma once

#include "cpu.hpp"
#include <array>

namespace nes{

inline auto get_instruction_info(){
  auto info = std::array<Cpu::InstructionInfo, 16 * 16>();

  using AddressMode = Cpu::AddressMode;
  //ORA
  info[0x09] = { 2, AddressMode::Immediate };
  info[0x05] = { 3, AddressMode::ZeroPage };
  info[0x15] = { 4, AddressMode::ZeroPageX };
  info[0x0D] = { 4, AddressMode::Absolute };
  info[0x1D] = { 4, AddressMode::AbsoluteX };
  info[0x19] = { 4, AddressMode::AbsoluteY };
  info[0x01] = { 6, AddressMode::XIndirect };
  info[0x11] = { 5, AddressMode::IndirectY };

  //AND
  info[0x29] = { 2, AddressMode::Immediate };
  info[0x25] = { 3, AddressMode::ZeroPage };
  info[0x35] = { 4, AddressMode::ZeroPageX };
  info[0x2D] = { 4, AddressMode::Absolute };
  info[0x3D] = { 4, AddressMode::AbsoluteX };
  info[0x39] = { 4, AddressMode::AbsoluteY };
  info[0x21] = { 6, AddressMode::XIndirect };
  info[0x31] = { 5, AddressMode::IndirectY };

  //EOR
  info[0x49] = { 2, AddressMode::Immediate };
  info[0x45] = { 3, AddressMode::ZeroPage };
  info[0x55] = { 4, AddressMode::ZeroPageX };
  info[0x4D] = { 4, AddressMode::Absolute };
  info[0x5D] = { 4, AddressMode::AbsoluteX };
  info[0x59] = { 4, AddressMode::AbsoluteY };
  info[0x41] = { 6, AddressMode::XIndirect };
  info[0x51] = { 5, AddressMode::IndirectY };
  
  //ADC
  info[0x69] = { 2, AddressMode::Immediate };
  info[0x65] = { 3, AddressMode::ZeroPage };
  info[0x75] = { 4, AddressMode::ZeroPageX };
  info[0x6D] = { 4, AddressMode::Absolute };
  info[0x7D] = { 4, AddressMode::AbsoluteX };
  info[0x79] = { 4, AddressMode::AbsoluteY };
  info[0x61] = { 6, AddressMode::XIndirect };
  info[0x71] = { 5, AddressMode::IndirectY };

  //ASL
  info[0x0A] = { 2, AddressMode::Accumulator };
  info[0x06] = { 5, AddressMode::ZeroPage };
  info[0x16] = { 6, AddressMode::ZeroPageX };
  info[0x0E] = { 6, AddressMode::Absolute };
  info[0x1E] = { 7, AddressMode::AbsoluteX };

  //BCC
  info[0x90] = { 2, AddressMode::Relative };

  //BCS
  info[0xB0] = { 2, AddressMode::Relative };

  //BEQ
  info[0xF0] = { 2, AddressMode::Relative };

  //BIT
  info[0x24] = { 3, AddressMode::ZeroPage };
  info[0x2C] = { 4, AddressMode::Absolute };

  //BMI
  info[0x30] = { 2, AddressMode::Relative };

  //BNE
  info[0xD0] = { 2, AddressMode::Relative };

  //BPL
  info[0x10] = { 2, AddressMode::Relative };

  //BRK
  info[0x00] = { 7, AddressMode::Implied };

  //BVC
  info[0x50] = { 2, AddressMode::Relative };

  //BVS
  info[0x70] = { 2, AddressMode::Relative };

  //CLC
  info[0x18] = { 2, AddressMode::Implied };

  //CLD
  info[0xD8] = { 2, AddressMode::Implied };

  //CLI
  info[0x58] = { 2, AddressMode::Implied };

  //CLV
  info[0xB8] = { 2, AddressMode::Implied };

  //CMP
  info[0xC9] = { 2, AddressMode::Immediate };
  info[0xC5] = { 3, AddressMode::ZeroPage };
  info[0xD5] = { 4, AddressMode::ZeroPageX };
  info[0xCD] = { 4, AddressMode::Absolute };
  info[0xDD] = { 4, AddressMode::AbsoluteX };
  info[0xD9] = { 4, AddressMode::AbsoluteY };
  info[0xC1] = { 6, AddressMode::XIndirect };
  info[0xD1] = { 5, AddressMode::IndirectY };

  //CPX
  info[0xE0] = { 2, AddressMode::Immediate };
  info[0xE4] = { 3, AddressMode::ZeroPage };
  info[0xEC] = { 4, AddressMode::Absolute };

  //CPY
  info[0xC0] = { 2, AddressMode::Immediate };
  info[0xC4] = { 3, AddressMode::ZeroPage };
  info[0xCC] = { 4, AddressMode::Absolute };

  //DEC
  info[0xC6] = { 5, AddressMode::ZeroPage };
  info[0xD6] = { 6, AddressMode::ZeroPageX };
  info[0xCE] = { 6, AddressMode::Absolute };
  info[0xDE] = { 7, AddressMode::AbsoluteX };

  //DEX
  info[0xCA] = { 2, AddressMode::Implied };

  //DEY
  info[0x88] = { 2, AddressMode::Implied };

  //INC
  info[0xE6] = { 5, AddressMode::ZeroPage };
  info[0xF6] = { 6, AddressMode::ZeroPageX };
  info[0xEE] = { 6, AddressMode::Absolute };
  info[0xFE] = { 7, AddressMode::AbsoluteX };

  //INX
  info[0xE8] = { 2, AddressMode::Implied };

  //INY
  info[0xC8] = { 2, AddressMode::Implied };

  //JMP
  info[0x4C] = { 3, AddressMode::Absolute };
  info[0x6C] = { 5, AddressMode::Indirect };

  //JSR
  info[0x20] = { 6, AddressMode::Absolute };

  //LDA
  info[0xA9] = { 2, AddressMode::Immediate };
  info[0xA5] = { 3, AddressMode::ZeroPage };
  info[0xB5] = { 4, AddressMode::ZeroPageX };
  info[0xAD] = { 4, AddressMode::Absolute };
  info[0xBD] = { 4, AddressMode::AbsoluteX };
  info[0xB9] = { 4, AddressMode::AbsoluteY };
  info[0xA1] = { 6, AddressMode::XIndirect };
  info[0xB1] = { 5, AddressMode::IndirectY };

  //LDX
  info[0xA2] = { 2, AddressMode::Immediate };
  info[0xA6] = { 3, AddressMode::ZeroPage };
  info[0xB6] = { 4, AddressMode::ZeroPageY };
  info[0xAE] = { 4, AddressMode::Absolute };
  info[0xBE] = { 4, AddressMode::AbsoluteY };

  //LDY
  info[0xA0] = { 2, AddressMode::Immediate };
  info[0xA4] = { 3, AddressMode::ZeroPage };
  info[0xB4] = { 4, AddressMode::ZeroPageX };
  info[0xAC] = { 4, AddressMode::Absolute };
  info[0xBC] = { 4, AddressMode::AbsoluteX };

  //LSR
  info[0x4A] = { 2, AddressMode::Accumulator };
  info[0x46] = { 5, AddressMode::ZeroPage };
  info[0x56] = { 6, AddressMode::ZeroPageX };
  info[0x4E] = { 6, AddressMode::Absolute };
  info[0x5E] = { 7, AddressMode::AbsoluteX };

  //NOP
  info[0xEA] = { 2, AddressMode::Implied };

  //PHA
  info[0x48] = { 3, AddressMode::Implied };

  //PHP
  info[0x08] = { 3, AddressMode::Implied };

  //PLA
  info[0x68] = { 4, AddressMode::Implied };

  //PLP
  info[0x28] = { 4, AddressMode::Implied };

  //ROL
  info[0x2A] = { 2, AddressMode::Accumulator };
  info[0x26] = { 5, AddressMode::ZeroPage };
  info[0x36] = { 6, AddressMode::ZeroPageX };
  info[0x2E] = { 6, AddressMode::Absolute };
  info[0x3E] = { 7, AddressMode::AbsoluteX };

  //ROR
  info[0x6A] = { 2, AddressMode::Accumulator };
  info[0x66] = { 5, AddressMode::ZeroPage };
  info[0x76] = { 6, AddressMode::ZeroPageX };
  info[0x6E] = { 6, AddressMode::Absolute };
  info[0x7E] = { 7, AddressMode::AbsoluteX };

  //RTI
  info[0x40] = { 6, AddressMode::Implied };

  //RTS
  info[0x60] = { 6, AddressMode::Implied };

  //SBC
  info[0xE9] = { 2, AddressMode::Immediate };
  info[0xE5] = { 3, AddressMode::ZeroPage };
  info[0xF5] = { 4, AddressMode::ZeroPageX };
  info[0xED] = { 4, AddressMode::Absolute };
  info[0xFD] = { 4, AddressMode::AbsoluteX };
  info[0xF9] = { 4, AddressMode::AbsoluteY };
  info[0xE1] = { 6, AddressMode::XIndirect };
  info[0xF1] = { 5, AddressMode::IndirectY };

  //SEC
  info[0x38] = { 2, AddressMode::Implied };

  //SED
  info[0xF8] = { 2, AddressMode::Implied };

  //SEI
  info[0x78] = { 2, AddressMode::Implied };

  //STA
  info[0x85] = { 3, AddressMode::ZeroPage };
  info[0x95] = { 4, AddressMode::ZeroPageX };
  info[0x8D] = { 4, AddressMode::Absolute };
  info[0x9D] = { 5, AddressMode::AbsoluteX };
  info[0x99] = { 5, AddressMode::AbsoluteY };
  info[0x81] = { 6, AddressMode::XIndirect };
  info[0x91] = { 6, AddressMode::IndirectY };

  //STX
  info[0x86] = { 3, AddressMode::ZeroPage };
  info[0x96] = { 4, AddressMode::ZeroPageY };
  info[0x8E] = { 4, AddressMode::Absolute };

  //STY
  info[0x84] = { 3, AddressMode::ZeroPage };
  info[0x94] = { 4, AddressMode::ZeroPageX };
  info[0x8C] = { 4, AddressMode::Absolute };

  //TAX
  info[0xAA] = { 2, AddressMode::Implied };

  //TAY
  info[0xA8] = { 2, AddressMode::Implied };

  //TSX
  info[0xBA] = { 2, AddressMode::Implied };

  //TXA
  info[0x8A] = { 2, AddressMode::Implied };

  //TXS
  info[0x9A] = { 2, AddressMode::Implied };

  //TYA
  info[0x98] = { 2, AddressMode::Implied };

  //ILLEGAL OPCODES:

  //NOP
  info[0x04] = { 3, AddressMode::ZeroPage };
  info[0x44] = { 3, AddressMode::ZeroPage };
  info[0x64] = { 3, AddressMode::ZeroPage };

  info[0x0C] = { 4, AddressMode::Absolute };

  info[0x14] = { 4, AddressMode::ZeroPageX };
  info[0x34] = { 4, AddressMode::ZeroPageX };
  info[0x54] = { 4, AddressMode::ZeroPageX };
  info[0x74] = { 4, AddressMode::ZeroPageX };
  info[0xD4] = { 4, AddressMode::ZeroPageX };
  info[0xF4] = { 4, AddressMode::ZeroPageX };

  info[0x1A] = { 2, AddressMode::Implied };
  info[0x3A] = { 2, AddressMode::Implied };
  info[0x5A] = { 2, AddressMode::Implied };
  info[0x7A] = { 2, AddressMode::Implied };
  info[0xDA] = { 2, AddressMode::Implied };
  info[0xEA] = { 2, AddressMode::Implied };
  info[0xFA] = { 2, AddressMode::Implied };

  info[0x80] = { 2, AddressMode::Immediate };
  info[0x82] = { 2, AddressMode::Immediate };
  info[0x89] = { 2, AddressMode::Immediate };
  info[0xC2] = { 2, AddressMode::Immediate };
  info[0xE2] = { 2, AddressMode::Immediate };

  info[0x1C] = { 4, AddressMode::AbsoluteX };
  info[0x3C] = { 4, AddressMode::AbsoluteX };
  info[0x5C] = { 4, AddressMode::AbsoluteX };
  info[0x7C] = { 4, AddressMode::AbsoluteX };
  info[0xDC] = { 4, AddressMode::AbsoluteX };
  info[0xFC] = { 4, AddressMode::AbsoluteX };

  //LAX
  info[0xA3] = { 6, AddressMode::XIndirect };
  info[0xA7] = { 3, AddressMode::ZeroPage };
  info[0xAF] = { 4, AddressMode::Absolute };
  info[0xB3] = { 5, AddressMode::IndirectY };
  info[0xB7] = { 4, AddressMode::ZeroPageY };
  info[0xBF] = { 4, AddressMode::AbsoluteY};

  //SAX
  info[0x83] = { 6, AddressMode::XIndirect };
  info[0x87] = { 3, AddressMode::ZeroPage };
  info[0x8F] = { 4, AddressMode::Absolute };
  info[0x97] = { 4, AddressMode::ZeroPageY };

  //SBC Duplicate
  info[0xEB] = { 2, AddressMode::Immediate };

  //DCP
  info[0xC3] = { 8, AddressMode::XIndirect };
  info[0xC7] = { 5, AddressMode::ZeroPage };
  info[0xCF] = { 6, AddressMode::Absolute };
  info[0xD3] = { 8, AddressMode::IndirectY };
  info[0xD7] = { 6, AddressMode::ZeroPageX };
  info[0xDB] = { 7, AddressMode::AbsoluteY };
  info[0xDF] = { 7, AddressMode::AbsoluteX };

  //ISC
  info[0xE3] = { 8, AddressMode::XIndirect };
  info[0xE7] = { 5, AddressMode::ZeroPage };
  info[0xEF] = { 6, AddressMode::Absolute };
  info[0xF3] = { 8, AddressMode::IndirectY };
  info[0xF7] = { 6, AddressMode::ZeroPageX };
  info[0xFB] = { 7, AddressMode::AbsoluteY };
  info[0xFF] = { 7, AddressMode::AbsoluteX };

  //RLA
  info[0x23] = { 8, AddressMode::XIndirect };
  info[0x27] = { 5, AddressMode::ZeroPage };
  info[0x2F] = { 6, AddressMode::Absolute };
  info[0x33] = { 8, AddressMode::IndirectY };
  info[0x37] = { 6, AddressMode::ZeroPageX };
  info[0x3B] = { 7, AddressMode::AbsoluteY };
  info[0x3F] = { 7, AddressMode::AbsoluteX };

  //RRA
  info[0x63] = { 8, AddressMode::XIndirect };
  info[0x67] = { 5, AddressMode::ZeroPage };
  info[0x6F] = { 6, AddressMode::Absolute };
  info[0x73] = { 8, AddressMode::IndirectY };
  info[0x77] = { 6, AddressMode::ZeroPageX };
  info[0x7B] = { 7, AddressMode::AbsoluteY };
  info[0x7F] = { 7, AddressMode::AbsoluteX };

  //SLO
  info[0x03] = { 8, AddressMode::XIndirect };
  info[0x07] = { 5, AddressMode::ZeroPage };
  info[0x0F] = { 6, AddressMode::Absolute };
  info[0x13] = { 8, AddressMode::IndirectY };
  info[0x17] = { 6, AddressMode::ZeroPageX };
  info[0x1B] = { 7, AddressMode::AbsoluteY };
  info[0x1F] = { 7, AddressMode::AbsoluteX };

  //SRE
  info[0x43] = { 8, AddressMode::XIndirect };
  info[0x47] = { 5, AddressMode::ZeroPage };
  info[0x4F] = { 6, AddressMode::Absolute };
  info[0x53] = { 8, AddressMode::IndirectY };
  info[0x57] = { 6, AddressMode::ZeroPageX };
  info[0x5B] = { 7, AddressMode::AbsoluteY };
  info[0x5F] = { 7, AddressMode::AbsoluteX };

  return info;
}

} //namespace nes
