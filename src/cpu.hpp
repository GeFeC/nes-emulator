#pragma once
#include "aliases.hpp"
#include <array>

namespace nes{

struct Nes;

struct Cpu{
  static constexpr auto StackBegin = 0x01FF;
  static constexpr auto StackEnd = 0x0100;

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

  struct AddressingData{
    u16 new_absolute_address;
    u16 new_relative_address = 0;
    u16 instruction_size = 0;
    bool may_req_additional_cycle = false;
  };

  struct InstructionInfo{
    u8 req_cycles;
    AddressMode addressing = AddressMode::None;
  };

  //Registers:
  u8 accumulator = 0;
  u8 x = 0;
  u8 y = 0;
  u8 status = 0;
  u8 sp = 0xFD;
  u16 pc = 0;

  u16 absolute_address = 0;
  u8 relative_address = 0;

  u8 req_cycles = 0;
  u32 cycles = 7;

  std::array<InstructionInfo, 16 * 16> instruction_info;

  Cpu();
  auto get_addressing_data(Nes& nes, Cpu::AddressMode mode) -> AddressingData;
  auto set_status(u8 flag, u8 state) -> void;
  auto get_status(u8 flag) -> u8;
  
  //Read from address inside 'absolute_address' prop
  auto fetch(Nes& nes) -> u8;

  auto stack_push(Nes& nes, u8 data) -> void;
  auto stack_pull(Nes& nes) -> u8;
  auto stack_pull_u16(Nes& nes) -> u16;

  auto lda(Nes& nes) -> void;
  auto ldx(Nes& nes) -> void;
  auto ldy(Nes& nes) -> void;
  auto sta(Nes& nes) -> void;
  auto stx(Nes& nes) -> void;
  auto sty(Nes& nes) -> void;
  auto tax() -> void;
  auto tay() -> void;
  auto tsx() -> void;
  auto txa() -> void;
  auto txs() -> void;
  auto tya() -> void;
  auto pha(Nes& nes) -> void;
  auto php(Nes& nes) -> void;
  auto pla(Nes& nes) -> void;
  auto plp(Nes& nes) -> void;
  auto and_(Nes& nes) -> void;
  auto eor(Nes& nes) -> void;
  auto ora(Nes& nes) -> void;
  auto bit(Nes& nes) -> void;
  auto adc(Nes& nes) -> void;
  auto sbc(Nes& nes) -> void;
  auto cmp(Nes& nes) -> void;
  auto cpx(Nes& nes) -> void;
  auto cpy(Nes& nes) -> void;
  auto inc(Nes& nes) -> void;
  auto inx() -> void;
  auto iny() -> void;
  auto dec(Nes& nes) -> void;
  auto dex() -> void;
  auto dey() -> void;
  auto asl(Nes& nes, bool accumulator_addressing) -> void;
  auto lsr(Nes& nes, bool accumulator_addressing) -> void;
  auto rol(Nes& nes, bool accumulator_addressing) -> void;
  auto ror(Nes& nes, bool accumulator_addressing) -> void;
  auto jmp() -> void;
  auto jsr(Nes& nes) -> void;
  auto rts(Nes& nes) -> void;
  auto bcc() -> void;
  auto bcs() -> void;
  auto beq() -> void;
  auto bne() -> void;
  auto bpl() -> void;
  auto bmi() -> void;
  auto bvc() -> void;
  auto bvs() -> void;
  auto clc() -> void;
  auto cld() -> void;
  auto cli() -> void;
  auto clv() -> void;
  auto sec() -> void;
  auto sed() -> void;
  auto sei() -> void;
  auto brk(Nes& nes) -> void;
  auto nop() {}
  auto rti(Nes& nes) -> void;

  //Illegal opcodes:
  auto lax(Nes& nes) -> void;
  auto sax(Nes& nes) -> void;
  auto dcp(Nes& nes) -> void;
  auto isc(Nes& nes) -> void;
  auto slo(Nes& nes) -> void;
  auto sre(Nes& nes) -> void;
  auto rra(Nes& nes) -> void;
  auto rla(Nes& nes) -> void;
  auto execute_instruction(Nes& nes, u8 instruction) -> bool;
  auto clock(Nes& nes) -> void;
};

} //namespace nes
