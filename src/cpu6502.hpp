#pragma once
#include "aliases.hpp"
#include "bus.hpp"
#include <iostream>

namespace nes{

struct Cpu{
  static constexpr auto StackBegin = 0x01FF;
  static constexpr auto StackEnd = 0x0100;

  u16 pc = 0;
  u8 sp = 0xFD;
  
  //Registers:
  u8 accumulator = 0;
  u8 x = 0;
  u8 y = 0;
  u8 status = 0;

  struct Status{
    enum{
      Carry = 1,
      Zero = (1 << 1),
      InterruptDisable = (1 << 2),
      DecimalMode = (1 << 3),
      BreakCommand = (1 << 4),
      Unused = (1 << 5),
      Overflow = (1 << 6),
      Negative = (1 << 7)
    };
  };

  enum class AddressMode{
    Accumulator,
    Implied,
    Immediate,
    ZeroPage,
    ZeroPageX,
    ZeroPageY,
    Relative,
    Absolute,
    AbsoluteX,
    AbsoluteY,
    Indirect,
    XIndirect,
    IndirectY,
    //Used for illegal instructions:
    None
  };

  u16 absolute_address = 0;
  u8 relative_address = 0;

  u8 req_cycles = 0;
  u32 cycles = 7;

  struct InstructionInfo{
    u8 req_cycles;
    AddressMode addressing = AddressMode::None;
  };

  InstructionInfo instruction_info[16 * 16];

  Cpu(){
    auto& info = instruction_info;

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
  }
};

inline auto cpu_set_address_mode(Cpu* cpu, const Bus& bus, Cpu::AddressMode mode){
  using Mode = Cpu::AddressMode;
  switch(mode){
    case Mode::None:
      return 0;

    case Mode::Accumulator:
      return 0;

    case Mode::Immediate:
      cpu->absolute_address = cpu->pc;
      cpu->pc++;
      return 0;

    case Mode::ZeroPage:
      cpu->absolute_address = bus_read(bus, cpu->pc) & 0x00FF;
      cpu->pc++;
      return 0;

    case Mode::ZeroPageX:
      cpu->absolute_address = bus_read(bus, cpu->pc) + cpu->x;
      cpu->absolute_address &= 0x00FF;
      cpu->pc++;
      return 0;

    case Mode::ZeroPageY:
      cpu->absolute_address = bus_read(bus, cpu->pc) + cpu->y;
      cpu->absolute_address &= 0x00FF;
      cpu->pc++;
      return 0;

    case Mode::Relative:
      cpu->relative_address = bus_read(bus, cpu->pc);
      cpu->pc++;
      return 0;

    case Mode::Absolute: {
      const auto low = bus_read(bus, cpu->pc);
      cpu->pc++;
      const auto high = bus_read(bus, cpu->pc);
      cpu->pc++;

      cpu->absolute_address = (high << 8) | low;

      return 0;
    }

    case Mode::AbsoluteX:{
      const auto low = bus_read(bus, cpu->pc);
      cpu->pc++;
      const auto high = bus_read(bus, cpu->pc);
      cpu->pc++;

      cpu->absolute_address = (high << 8) | low;
      cpu->absolute_address += cpu->x;

      //Check if page changed:
      if ((cpu->absolute_address & 0xFF00) != (high << 8)) return 1;

      return 0;
    }

    case Mode::AbsoluteY:{
      const auto low = bus_read(bus, cpu->pc);
      cpu->pc++;
      const auto high = bus_read(bus, cpu->pc);
      cpu->pc++;

      cpu->absolute_address = (high << 8) | low;
      cpu->absolute_address += cpu->y;

      //Check if page changed:
      if ((cpu->absolute_address & 0xFF00) != (high << 8)) return 1;

      return 0;
    }

    case Mode::Indirect:{
      const auto ptr_low = bus_read(bus, cpu->pc);
      cpu->pc++;

      const auto ptr_high = bus_read(bus, cpu->pc);
      cpu->pc++;

      const auto ptr = (ptr_high << 8) | ptr_low;

      const auto low = bus_read(bus, ptr);
      const auto high = ptr_low == 0x00FF 
        ? bus_read(bus, ptr & 0xFF00)
        : bus_read(bus, ptr + 1);

      cpu->absolute_address = (high << 8) | low;
      return 0;
    }

    case Mode::XIndirect:{
      const auto ptr = bus_read(bus, cpu->pc);
      cpu->pc++;

      const auto low = bus_read(bus, uint16_t(ptr + cpu->x) & 0x00FF);
      const auto high = bus_read(bus, uint16_t(ptr + cpu->x + 1) & 0x00FF);
      cpu->absolute_address = (high << 8) | low;

      return 0;
    }

    case Mode::IndirectY:{
      const auto ptr = bus_read(bus, cpu->pc);
      cpu->pc++;

      const u8 low = bus_read(bus, uint16_t(ptr) & 0x00FF);
      const u8 high = bus_read(bus, uint16_t(ptr + 1) & 0x00FF);
      cpu->absolute_address = (high << 8) | low;
      cpu->absolute_address += cpu->y;

      //Check if page changed:
      if ((cpu->absolute_address & 0xFF00) != (high << 8)) return 1;

      return 0;
    }

    case Mode::Implied:
      return 0;
  }
  return 0;
}

inline auto cpu_set_flag(Cpu* cpu, u8 status, u8 state){
  if (state){
    cpu->status |= status;
  }
  else{
    cpu->status &= (~status);
  }
}

inline auto cpu_flag(const Cpu& cpu, u8 status){
  return (cpu.status & status) > 0;
}

inline auto cpu_fetch(const Cpu& cpu, const Bus& bus){
  return bus_read(bus, cpu.absolute_address);
}

inline auto cpu_stack_push(Cpu* cpu, Bus* bus, u8 data){
  bus_write(bus, Cpu::StackEnd + cpu->sp, data);
  cpu->sp--;
}

inline auto cpu_stack_pull(Cpu* cpu, const Bus& bus){
  cpu->sp++;
  u8 value = bus_read(bus, Cpu::StackEnd + cpu->sp);
  return value;
}

inline auto cpu_lda(Cpu* cpu, const Bus& bus){
  cpu->accumulator = cpu_fetch(*cpu, bus);

  cpu_set_flag(cpu, Cpu::Status::Zero, cpu->accumulator == 0);
  cpu_set_flag(cpu, Cpu::Status::Negative, cpu->accumulator >> 7);

  return 1;
}

inline auto cpu_ldx(Cpu* cpu, const Bus& bus){
  cpu->x = cpu_fetch(*cpu, bus);

  cpu_set_flag(cpu, Cpu::Status::Zero, cpu->x == 0);
  cpu_set_flag(cpu, Cpu::Status::Negative, cpu->x >> 7);

  return 1;
}

inline auto cpu_ldy(Cpu* cpu, const Bus& bus){
  cpu->y = cpu_fetch(*cpu, bus);

  cpu_set_flag(cpu, Cpu::Status::Zero, cpu->y == 0);
  cpu_set_flag(cpu, Cpu::Status::Negative, cpu->y >> 7);

  return 1;
}

inline auto cpu_sta(const Cpu& cpu, Bus* bus){
  bus_write(bus, cpu.absolute_address, cpu.accumulator);
}

inline auto cpu_stx(const Cpu& cpu, Bus* bus){
  bus_write(bus, cpu.absolute_address, cpu.x);
}

inline auto cpu_sty(const Cpu& cpu, Bus* bus){
  bus_write(bus, cpu.absolute_address, cpu.y);
}

inline auto cpu_tax(Cpu* cpu){
  cpu->x = cpu->accumulator;
  cpu_set_flag(cpu, Cpu::Status::Zero, cpu->x == 0);
  cpu_set_flag(cpu, Cpu::Status::Negative, cpu->x >> 7);
}

inline auto cpu_tay(Cpu* cpu){
  cpu->y = cpu->accumulator;
  cpu_set_flag(cpu, Cpu::Status::Zero, cpu->y == 0);
  cpu_set_flag(cpu, Cpu::Status::Negative, cpu->y >> 7);
}

inline auto cpu_tsx(Cpu* cpu){
  cpu->x = cpu->sp;
  cpu_set_flag(cpu, Cpu::Status::Zero, cpu->x == 0);
  cpu_set_flag(cpu, Cpu::Status::Negative, cpu->x >> 7);
}

inline auto cpu_txa(Cpu* cpu){
  cpu->accumulator = cpu->x;
  cpu_set_flag(cpu, Cpu::Status::Zero, cpu->accumulator  == 0);
  cpu_set_flag(cpu, Cpu::Status::Negative, cpu->accumulator >> 7);
}

inline auto cpu_txs(Cpu* cpu){
  cpu->sp = cpu->x;
}

inline auto cpu_tya(Cpu* cpu){
  cpu->accumulator = cpu->y;
  cpu_set_flag(cpu, Cpu::Status::Zero, cpu->accumulator  == 0);
  cpu_set_flag(cpu, Cpu::Status::Negative, cpu->accumulator >> 7);
}

inline auto cpu_pha(Cpu* cpu, Bus* bus){
  cpu_stack_push(cpu, bus, cpu->accumulator);
}

inline auto cpu_php(Cpu* cpu, Bus* bus){
  cpu_stack_push(cpu, bus, cpu->status | Cpu::Status::Unused | Cpu::Status::BreakCommand);
}

inline auto cpu_pla(Cpu* cpu, const Bus& bus){
  cpu->sp++;
  cpu->accumulator = bus_read(bus, Cpu::StackEnd + cpu->sp);
  cpu_set_flag(cpu, Cpu::Status::Zero, cpu->accumulator  == 0);
  cpu_set_flag(cpu, Cpu::Status::Negative, cpu->accumulator >> 7);
}

inline auto cpu_plp(Cpu* cpu, const Bus& bus){
  cpu->sp++;
  cpu->status = bus_read(bus, Cpu::StackEnd + cpu->sp);
  cpu_set_flag(cpu, Cpu::Status::BreakCommand, 0);
}

inline auto cpu_and(Cpu* cpu, const Bus& bus){
  const u8 value = cpu_fetch(*cpu, bus);
  cpu->accumulator &= value;

  cpu_set_flag(cpu, Cpu::Status::Zero, cpu->accumulator  == 0);
  cpu_set_flag(cpu, Cpu::Status::Negative, cpu->accumulator >> 7);

  return 1;
}

inline auto cpu_eor(Cpu* cpu, const Bus& bus){
  const u8 value = cpu_fetch(*cpu, bus);
  cpu->accumulator ^= value;

  cpu_set_flag(cpu, Cpu::Status::Zero, cpu->accumulator  == 0);
  cpu_set_flag(cpu, Cpu::Status::Negative, cpu->accumulator >> 7);

  return 1;
}

inline auto cpu_ora(Cpu* cpu, const Bus& bus){
  const u8 value = cpu_fetch(*cpu, bus);
  cpu->accumulator |= value;

  cpu_set_flag(cpu, Cpu::Status::Zero, cpu->accumulator  == 0);
  cpu_set_flag(cpu, Cpu::Status::Negative, cpu->accumulator >> 7);

  return 1;
}

inline auto cpu_bit(Cpu* cpu, const Bus& bus){
  const u8 value = cpu_fetch(*cpu, bus);
  const u8 result = cpu->accumulator & value;

  cpu_set_flag(cpu, Cpu::Status::Zero, result == 0);
  cpu_set_flag(cpu, Cpu::Status::Overflow, value & (1 << 6));
  cpu_set_flag(cpu, Cpu::Status::Negative, value & (1 << 7));
}

inline auto cpu_adc(Cpu* cpu, const Bus& bus){
  const u8 value = cpu_fetch(*cpu, bus);
  const u8 carry = cpu_flag(*cpu, Cpu::Status::Carry);

  const uint16_t result = cpu->accumulator + value + carry;
  cpu_set_flag(cpu, Cpu::Status::Carry, result > 255);
  cpu_set_flag(cpu, Cpu::Status::Zero, (result & 0x00FF) == 0);

  const u8 sign_bit_incorrect = ((value ^ result) & (cpu->accumulator ^ result)) & (1 << 7);
  cpu_set_flag(cpu, Cpu::Status::Overflow, sign_bit_incorrect);
  cpu_set_flag(cpu, Cpu::Status::Negative, result & (1 << 7));
  cpu->accumulator = result;

  return 1;
}

inline auto cpu_sbc(Cpu* cpu, const Bus& bus){
  const u8 value = cpu_fetch(*cpu, bus);
  const u8 carry = cpu_flag(*cpu, Cpu::Status::Carry);

  const uint16_t result = cpu->accumulator + u8(~value) + carry;
  cpu_set_flag(cpu, Cpu::Status::Carry, result > 255);
  cpu_set_flag(cpu, Cpu::Status::Zero, (result & 0x00FF) == 0);

  const u8 sign_bit_incorrect = ((u8(~value) ^ result) & (cpu->accumulator ^ result)) & (1 << 7);
  cpu_set_flag(cpu, Cpu::Status::Overflow, sign_bit_incorrect);
  cpu_set_flag(cpu, Cpu::Status::Negative, result & (1 << 7));
  cpu->accumulator = result;

  return 1;
}

inline auto cpu_cmp(Cpu* cpu, const Bus& bus){
  const u8 value = cpu_fetch(*cpu, bus);
  cpu_set_flag(cpu, Cpu::Status::Carry, value <= cpu->accumulator);
  cpu_set_flag(cpu, Cpu::Status::Zero, value == cpu->accumulator);
  const uint16_t result = cpu->accumulator - value;
  cpu_set_flag(cpu, Cpu::Status::Negative, result & (1 << 7));
  return 1;
}

inline auto cpu_cpx(Cpu* cpu, const Bus& bus){
  const u8 value = cpu_fetch(*cpu, bus);
  cpu_set_flag(cpu, Cpu::Status::Carry, value <= cpu->x);
  cpu_set_flag(cpu, Cpu::Status::Zero, value == cpu->x);
  const uint16_t result = cpu->x - value;
  cpu_set_flag(cpu, Cpu::Status::Negative, result & (1 << 7));
}

inline auto cpu_cpy(Cpu* cpu, const Bus& bus){
  const u8 value = cpu_fetch(*cpu, bus);
  cpu_set_flag(cpu, Cpu::Status::Carry, value <= cpu->y);
  cpu_set_flag(cpu, Cpu::Status::Zero, value == cpu->y);
  const uint16_t result = cpu->y - value;
  cpu_set_flag(cpu, Cpu::Status::Negative, result & (1 << 7));
}

inline auto cpu_inc(Cpu* cpu, Bus* bus){
  const u8 value = cpu_fetch(*cpu, *bus);
  const u8 result = value + 1;

  bus_write(bus, cpu->absolute_address, result);
  cpu_set_flag(cpu, Cpu::Status::Zero, result == 0);
  cpu_set_flag(cpu, Cpu::Status::Negative, result & (1 << 7));
}

inline auto cpu_inx(Cpu* cpu){
  cpu->x++;

  cpu_set_flag(cpu, Cpu::Status::Zero, cpu->x == 0);
  cpu_set_flag(cpu, Cpu::Status::Negative, cpu->x & (1 << 7));
}

inline auto cpu_iny(Cpu* cpu){
  cpu->y++;

  cpu_set_flag(cpu, Cpu::Status::Zero, cpu->y == 0);
  cpu_set_flag(cpu, Cpu::Status::Negative, cpu->y & (1 << 7));
}

inline auto cpu_dec(Cpu* cpu, Bus* bus){
  const u8 value = cpu_fetch(*cpu, *bus);
  const u8 result = value - 1;

  bus_write(bus, cpu->absolute_address, result);
  cpu_set_flag(cpu, Cpu::Status::Zero, result == 0);
  cpu_set_flag(cpu, Cpu::Status::Negative, result & (1 << 7));
}

inline auto cpu_dex(Cpu* cpu){
  cpu->x--;

  cpu_set_flag(cpu, Cpu::Status::Zero, cpu->x == 0);
  cpu_set_flag(cpu, Cpu::Status::Negative, cpu->x & (1 << 7));
}

inline auto cpu_dey(Cpu* cpu){
  cpu->y--;

  cpu_set_flag(cpu, Cpu::Status::Zero, cpu->y == 0);
  cpu_set_flag(cpu, Cpu::Status::Negative, cpu->y & (1 << 7));
}

inline auto cpu_asl(Cpu* cpu, Bus* bus, bool accumulator = false){
  u8 result = accumulator ? cpu->accumulator : cpu_fetch(*cpu, *bus);

  cpu_set_flag(cpu, Cpu::Status::Carry, result >> 7);
  result <<= 1;
  cpu_set_flag(cpu, Cpu::Status::Zero, result == 0);
  cpu_set_flag(cpu, Cpu::Status::Negative, result & (1 << 7));

  if (accumulator){
    cpu->accumulator = result;
  }
  else{
    bus_write(bus, cpu->absolute_address, result);
  }
}

inline auto cpu_lsr(Cpu* cpu, Bus* bus, bool accumulator = false){
  u8 result = accumulator ? cpu->accumulator : cpu_fetch(*cpu, *bus);

  cpu_set_flag(cpu, Cpu::Status::Carry, result & 1);
  result >>= 1;
  cpu_set_flag(cpu, Cpu::Status::Zero, result == 0);
  cpu_set_flag(cpu, Cpu::Status::Negative, result & (1 << 7));

  if (accumulator){
    cpu->accumulator = result;
  }
  else{
    bus_write(bus, cpu->absolute_address, result);
  }
}

inline auto cpu_rol(Cpu* cpu, Bus* bus, bool accumulator = false){
  u8 result = accumulator ? cpu->accumulator : cpu_fetch(*cpu, *bus);

  const u8 msb = result >> 7;
  result <<= 1;
  result |= cpu_flag(*cpu, Cpu::Status::Carry);
  cpu_set_flag(cpu, Cpu::Status::Carry, msb);
  cpu_set_flag(cpu, Cpu::Status::Zero, result == 0);
  cpu_set_flag(cpu, Cpu::Status::Negative, result & (1 << 7));

  if (accumulator){
    cpu->accumulator = result;
  }
  else{
    bus_write(bus, cpu->absolute_address, result);
  }
}

inline auto cpu_ror(Cpu* cpu, Bus* bus, bool accumulator = false){
  u8 result = accumulator ? cpu->accumulator : cpu_fetch(*cpu, *bus);

  const u8 lsb = result & 1;
  result >>= 1;
  result |= cpu_flag(*cpu, Cpu::Status::Carry) << 7;
  cpu_set_flag(cpu, Cpu::Status::Carry, lsb);
  cpu_set_flag(cpu, Cpu::Status::Zero, result == 0);
  cpu_set_flag(cpu, Cpu::Status::Negative, result & (1 << 7));

  if (accumulator){
    cpu->accumulator = result;
  }
  else{
    bus_write(bus, cpu->absolute_address, result);
  }
}

inline auto cpu_jmp(Cpu* cpu){
  cpu->pc = cpu->absolute_address;
}

inline auto cpu_jsr(Cpu* cpu, Bus* bus){
  cpu->pc--;

  cpu_stack_push(cpu, bus, (cpu->pc >> 8) & 0x00FF);
  cpu_stack_push(cpu, bus, cpu->pc & 0x00FF);

  cpu->pc = cpu->absolute_address;
}

inline auto cpu_rts(Cpu* cpu, const Bus& bus){
  u8 low = cpu_stack_pull(cpu, bus);
  u8 high = cpu_stack_pull(cpu, bus);

  cpu->pc = (high << 8) | low;
  cpu->pc++;
}

inline auto cpu_bcc(Cpu* cpu){
  if (cpu_flag(*cpu, Cpu::Status::Carry) == 0){
    cpu->req_cycles++;
    cpu->absolute_address = cpu->pc + cpu->relative_address;
    if (cpu->relative_address > 127){
      cpu->absolute_address -= 256;
    }

    if ((cpu->pc & 0xFF00) != (cpu->absolute_address & 0xFF00)) cpu->req_cycles++;
    cpu->pc = cpu->absolute_address;
  }
}

inline auto cpu_bcs(Cpu* cpu){
  if (cpu_flag(*cpu, Cpu::Status::Carry)){
    cpu->req_cycles++;
    cpu->absolute_address = cpu->pc + cpu->relative_address;

    if (cpu->relative_address > 127){
      cpu->absolute_address -= 256;
    }

    if ((cpu->pc & 0xFF00) != (cpu->absolute_address & 0xFF00)) cpu->req_cycles++;

    cpu->pc = cpu->absolute_address;
  }
}

inline auto cpu_beq(Cpu* cpu){
  if (cpu_flag(*cpu, Cpu::Status::Zero) == 1){
    cpu->req_cycles++;
    cpu->absolute_address = cpu->pc + cpu->relative_address;
    if (cpu->relative_address > 127){
      cpu->absolute_address -= 256;
    }

    if ((cpu->pc & 0xFF00) != (cpu->absolute_address & 0xFF00)) cpu->req_cycles++;
    cpu->pc = cpu->absolute_address;
  }
}

inline auto cpu_bne(Cpu* cpu){
  if (cpu_flag(*cpu, Cpu::Status::Zero) == 0){
    cpu->req_cycles++;
    cpu->absolute_address = cpu->pc + cpu->relative_address;
    if (cpu->relative_address > 127){
      cpu->absolute_address -= 256;
    }

    if ((cpu->pc & 0xFF00) != (cpu->absolute_address & 0xFF00)) cpu->req_cycles++;
    cpu->pc = cpu->absolute_address;
  }
}

inline auto cpu_bpl(Cpu* cpu){
  if (cpu_flag(*cpu, Cpu::Status::Negative) == 0){
    cpu->req_cycles++;
    cpu->absolute_address = cpu->pc + cpu->relative_address;
    if (cpu->relative_address > 127){
      cpu->absolute_address -= 256;
    }

    if ((cpu->pc & 0xFF00) != (cpu->absolute_address & 0xFF00)) cpu->req_cycles++;
    cpu->pc = cpu->absolute_address;
  }
}

inline auto cpu_bmi(Cpu* cpu){
  if (cpu_flag(*cpu, Cpu::Status::Negative)){
    cpu->req_cycles++;
    cpu->absolute_address = cpu->pc + cpu->relative_address;
    if (cpu->relative_address > 127){
      cpu->absolute_address -= 256;
    }

    if ((cpu->pc & 0xFF00) != (cpu->absolute_address & 0xFF00)) cpu->req_cycles++;
    cpu->pc = cpu->absolute_address;
  }
}

inline auto cpu_bvc(Cpu* cpu){
  if (cpu_flag(*cpu, Cpu::Status::Overflow) == 0){
    cpu->req_cycles++;
    cpu->absolute_address = cpu->pc + cpu->relative_address;
    if (cpu->relative_address > 127){
      cpu->absolute_address -= 256;
    }

    if ((cpu->pc & 0xFF00) != (cpu->absolute_address & 0xFF00)) cpu->req_cycles++;
    cpu->pc = cpu->absolute_address;
  }
}

inline auto cpu_bvs(Cpu* cpu){
  if (cpu_flag(*cpu, Cpu::Status::Overflow)){
    cpu->req_cycles++;
    cpu->absolute_address = cpu->pc + cpu->relative_address;
    if (cpu->relative_address > 127){
      cpu->absolute_address -= 256;
    }

    if ((cpu->pc & 0xFF00) != (cpu->absolute_address & 0xFF00)) cpu->req_cycles++;
    cpu->pc = cpu->absolute_address;
  }
}

inline auto cpu_clc(Cpu* cpu){
  cpu_set_flag(cpu, Cpu::Status::Carry, 0);
}

inline auto cpu_cld(Cpu* cpu){
  cpu_set_flag(cpu, Cpu::Status::DecimalMode, 0);
}

inline auto cpu_cli(Cpu* cpu){
  cpu_set_flag(cpu, Cpu::Status::InterruptDisable, 0);
}

inline auto cpu_clv(Cpu* cpu){
  cpu_set_flag(cpu, Cpu::Status::Overflow, 0);
}

inline auto cpu_sec(Cpu* cpu){
  cpu_set_flag(cpu, Cpu::Status::Carry, 1);
}

inline auto cpu_sed(Cpu* cpu){
  cpu_set_flag(cpu, Cpu::Status::DecimalMode, 1);
}

inline auto cpu_sei(Cpu* cpu){
  cpu_set_flag(cpu, Cpu::Status::InterruptDisable, 1);
}

inline auto cpu_brk(Cpu* cpu, Bus* bus){
  cpu_set_flag(cpu, Cpu::Status::BreakCommand, 1);
  cpu_stack_push(cpu, bus, cpu->pc << 8);
  cpu_stack_push(cpu, bus, cpu->pc);
  cpu_stack_push(cpu, bus, cpu->status);

  cpu->pc = bus_read(*bus, 0xFFFE) | u16(bus_read(*bus, 0xFFFF) << 8);
}

inline auto cpu_nop(){}

inline auto cpu_rti(Cpu* cpu, const Bus& bus){
  cpu->status = cpu_stack_pull(cpu, bus);
  cpu_set_flag(cpu, Cpu::Status::BreakCommand, 0);
  const u8 low = cpu_stack_pull(cpu, bus);
  const u8 high = cpu_stack_pull(cpu, bus);
  cpu->pc = low | u16(high << 8);
}

//Illegal opcodes:
inline auto cpu_lax(Cpu* cpu, Bus* bus){
  cpu_lda(cpu, *bus);
  cpu_tax(cpu);
  return 1;
}

inline auto cpu_sax(const Cpu& cpu, Bus* bus){
  bus_write(bus, cpu.absolute_address, cpu.accumulator & cpu.x);
}

inline auto cpu_dcp(Cpu* cpu, Bus* bus){
  cpu_dec(cpu, bus);
  cpu_cmp(cpu, *bus);
}

inline auto cpu_isc(Cpu* cpu, Bus* bus){
  cpu_inc(cpu, bus);
  cpu_sbc(cpu, *bus);
}

inline auto cpu_slo(Cpu* cpu, Bus* bus){
  cpu_asl(cpu, bus);
  cpu_ora(cpu, *bus);
}

inline auto cpu_sre(Cpu* cpu, Bus* bus){
  cpu_lsr(cpu, bus);
  cpu_eor(cpu, *bus);
}

inline auto cpu_rra(Cpu* cpu, Bus* bus){
  cpu_ror(cpu, bus);
  cpu_adc(cpu, *bus);
}

inline auto cpu_rla(Cpu* cpu, Bus* bus){
  cpu_rol(cpu, bus);
  cpu_and(cpu, *bus);
}

inline auto cpu_execute(Cpu* cpu, Bus* bus, u8 instruction){
  const auto [req_cycles, addressing] = cpu->instruction_info[instruction];
  cpu->req_cycles = req_cycles;

  const auto may_req_additional_clock = cpu_set_address_mode(cpu, *bus, addressing);

  switch(instruction){

    case 0x09:
    case 0x05:
    case 0x15:
    case 0x0D:
    case 0x1D:
    case 0x19:
    case 0x01:
    case 0x11:
      return cpu_ora(cpu, *bus) && may_req_additional_clock;

    //AND
    case 0x29:
    case 0x25:
    case 0x35:
    case 0x2D:
    case 0x3D:
    case 0x39:
    case 0x21:
    case 0x31:
      return cpu_and(cpu, *bus) && may_req_additional_clock;

    //EOR
    case 0x49:
    case 0x45:
    case 0x55:
    case 0x4D:
    case 0x5D:
    case 0x59:
    case 0x41:
    case 0x51:
      return cpu_eor(cpu, *bus) && may_req_additional_clock;
    
    //ADC
    case 0x69:
    case 0x65:
    case 0x75:
    case 0x6D:
    case 0x7D:
    case 0x79:
    case 0x61:
    case 0x71:
      return cpu_adc(cpu, *bus) && may_req_additional_clock;

    //ASL
    case 0x0A:
      cpu_asl(cpu, bus, true);
      break;

    case 0x06:
    case 0x16:
    case 0x0E:
    case 0x1E:
      cpu_asl(cpu, bus);
      break;

    //BCC
    case 0x90:
      cpu_bcc(cpu);
      break;

    //BCS
    case 0xB0:
      cpu_bcs(cpu);
      break;

    //BEQ
    case 0xF0:
      cpu_beq(cpu);
      break;

    //BIT
    case 0x24:
    case 0x2C:
      cpu_bit(cpu, *bus);
      break;

    //BMI
    case 0x30:
      cpu_bmi(cpu);
      break;

    //BNE
    case 0xD0:
      cpu_bne(cpu);
      break;

    //BPL
    case 0x10:
      cpu_bpl(cpu);
      break;

    //BRK
    case 0x00:
      cpu_brk(cpu, bus);
      break;

    //BVC
    case 0x50:
      cpu_bvc(cpu);
      break;

    //BVS
    case 0x70:
      cpu_bvs(cpu);
      break;

    //CLC
    case 0x18:
      cpu_clc(cpu);
      break;

    //CLD
    case 0xD8:
      cpu_cld(cpu);
      break;

    //CLI
    case 0x58:
      cpu_cli(cpu);
      break;

    //CLV
    case 0xB8:
      cpu_clv(cpu);
      break;

    //CMP
    case 0xC9:
    case 0xC5:
    case 0xD5:
    case 0xCD:
    case 0xDD:
    case 0xD9:
    case 0xC1:
    case 0xD1:
      return cpu_cmp(cpu, *bus) && may_req_additional_clock;

    //CPX
    case 0xE0:
    case 0xE4:
    case 0xEC:
      cpu_cpx(cpu, *bus);
      break;

    //CPY
    case 0xC0:
    case 0xC4:
    case 0xCC:
      cpu_cpy(cpu, *bus);
      break;

    //DEC
    case 0xC6:
    case 0xD6:
    case 0xCE:
    case 0xDE:
      cpu_dec(cpu, bus);
      break;

    //DEX
    case 0xCA:
      cpu_dex(cpu);
      break;

    //DEY
    case 0x88:
      cpu_dey(cpu);
      break;

    //INC
    case 0xE6:
    case 0xF6:
    case 0xEE:
    case 0xFE:
      cpu_inc(cpu, bus);
      break;

    //INX
    case 0xE8:
      cpu_inx(cpu);
      break;

    //INY
    case 0xC8:
      cpu_iny(cpu);
      break;

    //JMP
    case 0x4C:
    case 0x6C:
      cpu_jmp(cpu);
      break;

    //JSR
    case 0x20:
      cpu_jsr(cpu, bus);
      break;

    //LDA
    case 0xA9:
    case 0xA5:
    case 0xB5:
    case 0xAD:
    case 0xBD:
    case 0xB9:
    case 0xA1:
    case 0xB1:
      return cpu_lda(cpu, *bus) && may_req_additional_clock;

    //LDX
    case 0xA2:
    case 0xA6:
    case 0xB6:
    case 0xAE:
    case 0xBE:
      return cpu_ldx(cpu, *bus) && may_req_additional_clock;

    //LDY
    case 0xA0:
    case 0xA4:
    case 0xB4:
    case 0xAC:
    case 0xBC:
      return cpu_ldy(cpu, *bus) && may_req_additional_clock;

    //LSR
    case 0x4A:
      cpu_lsr(cpu, bus, true);
      break;
    case 0x46:
    case 0x56:
    case 0x4E:
    case 0x5E:
      cpu_lsr(cpu, bus);
      break;

    //NOP
    case 0xEA:
      cpu_nop();
      break;

    //PHA
    case 0x48:
      cpu_pha(cpu, bus);
      break;

    //PHP
    case 0x08:
      cpu_php(cpu, bus);
      break;

    //PLA
    case 0x68:
      cpu_pla(cpu, *bus);
      break;

    //PLP
    case 0x28:
      cpu_plp(cpu, *bus);
      break;

    //ROL
    case 0x2A:
      cpu_rol(cpu, bus, true);
      break;

    case 0x26:
    case 0x36:
    case 0x2E:
    case 0x3E:
      cpu_rol(cpu, bus, false);
      break;

    //ROR
    case 0x6A:
      cpu_ror(cpu, bus, true);
      break;

    case 0x66:
    case 0x76:
    case 0x6E:
    case 0x7E:
      cpu_ror(cpu, bus, false);
      break;

    //RTI
    case 0x40:
      cpu_rti(cpu, *bus);
      break;

    //RTS
    case 0x60:
      cpu_rts(cpu, *bus);
      break;

    //SBC
    case 0xE9:
    case 0xE5:
    case 0xF5:
    case 0xED:
    case 0xFD:
    case 0xF9:
    case 0xE1:
    case 0xF1:
      return cpu_sbc(cpu, *bus) && may_req_additional_clock;

    //SEC
    case 0x38:
      cpu_sec(cpu);
      break;

    //SED
    case 0xF8:
      cpu_sed(cpu);
      break;

    //SEI
    case 0x78:
      cpu_sei(cpu);
      break;

    //STA
    case 0x85:
    case 0x95:
    case 0x8D:
    case 0x9D:
    case 0x99:
    case 0x81:
    case 0x91:
      cpu_sta(*cpu, bus);
      break;

    //STX
    case 0x86:
    case 0x96:
    case 0x8E:
      cpu_stx(*cpu, bus);
      break;

    //STY
    case 0x84:
    case 0x94:
    case 0x8C:
      cpu_sty(*cpu, bus);
      break;

    //TAX
    case 0xAA:
      cpu_tax(cpu);
      break;

    //TAY
    case 0xA8:
      cpu_tay(cpu);
      break;

    //TSX
    case 0xBA:
      cpu_tsx(cpu);
      break;

    //TXA
    case 0x8A:
      cpu_txa(cpu);
      break;

    //TXS
    case 0x9A:
      cpu_txs(cpu);
      break;

    //TYA
    case 0x98:
      cpu_tya(cpu);
      break;

    //Illegal opcodes:

    //LAX
    case 0xA3:
    case 0xA7:
    case 0xAF:
    case 0xB3:
    case 0xB7:
    case 0xBF:
      return cpu_lax(cpu, bus) && may_req_additional_clock;

    //NOP
    case 0x04:
    case 0x44:
    case 0x64:
    case 0x0C:
    case 0x14:
    case 0x34:
    case 0x54:
    case 0x74:
    case 0xD4:
    case 0xF4:
    case 0x1A:
    case 0x3A:
    case 0x5A:
    case 0x7A:
    case 0xDA:
    case 0xFA:
    case 0x80:
    case 0x82:
    case 0x89:
    case 0xC2:
    case 0xE2:
      break;

    case 0x1C:
    case 0x3C:
    case 0x5C:
    case 0x7C:
    case 0xDC:
    case 0xFC:
      return !!may_req_additional_clock;

    case 0x83:
    case 0x87:
    case 0x8F:
    case 0x97:
      cpu_sax(*cpu, bus);
      break;

    case 0xEB:
      cpu_sbc(cpu, *bus);
      break;

    case 0xC3:
    case 0xC7:
    case 0xCF:
    case 0xD3:
    case 0xD7:
    case 0xDB:
    case 0xDF:
      cpu_dcp(cpu, bus);
      break;

    case 0xE3:
    case 0xE7:
    case 0xEF:
    case 0xF3:
    case 0xF7:
    case 0xFB:
    case 0xFF:
      cpu_isc(cpu, bus);
      break;

    //RLA
    case 0x23:
    case 0x27:
    case 0x2F:
    case 0x33:
    case 0x37:
    case 0x3B:
    case 0x3F:
      cpu_rla(cpu, bus);
      break;

    //RRA
    case 0x63:
    case 0x67:
    case 0x6F:
    case 0x73:
    case 0x77:
    case 0x7B:
    case 0x7F:
      cpu_rra(cpu, bus);
      break;

    //SLO
    case 0x03:
    case 0x07:
    case 0x0F:
    case 0x13:
    case 0x17:
    case 0x1B:
    case 0x1F:
      cpu_slo(cpu, bus);
      break;

    //SRE
    case 0x43:
    case 0x47:
    case 0x4F:
    case 0x53:
    case 0x57:
    case 0x5B:
    case 0x5F:
      cpu_sre(cpu, bus);
      break;

    default:
      std::cerr << std::hex << cpu->pc << " Unsupported opcode: " << int(instruction) << '\n';
      throw std::runtime_error("");
      break;
  }

  return false;
}

inline auto cpu_clock(Cpu* cpu, Bus* bus){
  if (cpu->req_cycles == 0){
    const u8 cmd = bus_read(*bus, cpu->pc);
    cpu->pc++;

    const auto requires_additional_cycle = cpu_execute(cpu, bus, cmd);
    if (requires_additional_cycle) cpu->req_cycles++;

    cpu_set_flag(cpu, Cpu::Status::Unused, 1);
  }

  cpu->req_cycles--;
}

} //namespace nes
