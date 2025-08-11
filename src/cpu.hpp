#pragma once
#include "aliases.hpp"
#include <array>

namespace nes{

struct Nes;

struct Cpu{
  static constexpr auto StackBegin = 0x01FF;
  static constexpr auto StackEnd = 0x0100;
  static constexpr auto MayRequireAdditionalCycle = 1; 

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

  struct Instruction{
    using fn_type = void(*)(Cpu&, Nes&);

    u8 req_cycles;
    AddressMode address_mode = AddressMode::None;
    fn_type call_ptr = nullptr;
    bool may_req_additional_cycle = false;
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
  bool accumulator_addressing = false;

  std::array<Instruction, 16 * 16> instruction_lookup;

  Cpu();
  auto set_address_mode(Nes& nes, Cpu::AddressMode mode) -> bool;
  auto set_status(u8 flag, u8 state) -> void;
  auto get_status(u8 flag) -> u8;
  
  //Read from address inside 'absolute_address' prop
  auto fetch(Nes& nes) -> u8;

  auto stack_push(Nes& nes, u8 data) -> void;
  auto stack_push_u16(Nes& nes, u16 data) -> void;
  auto stack_pull(Nes& nes) -> u8;
  auto stack_pull_u16(Nes& nes) -> u16;

  auto irq(Nes& nes) -> void;
  auto nmi(Nes& nes) -> void;

  auto execute_instruction(Nes& nes, u8 instruction) -> bool;
  auto clock(Nes& nes) -> void;
};

} //namespace nes
