#include "cpu.hpp"
#include "nes.hpp"
#include <algorithm>

namespace nes{

static auto nop(Cpu& cpu, Nes& nes) -> void{}

static auto lda(Cpu& cpu, Nes& nes) -> void{
  cpu.accumulator = cpu.fetch(nes);

  cpu.status.set(Cpu::Status::Zero, cpu.accumulator == 0);
  cpu.status.set(Cpu::Status::Negative, cpu.accumulator >> 7);
}

static auto ldx(Cpu& cpu, Nes& nes) -> void{
  cpu.x = cpu.fetch(nes);

  cpu.status.set(Cpu::Status::Zero, cpu.x == 0);
  cpu.status.set(Cpu::Status::Negative, cpu.x >> 7);
}

static auto ldy(Cpu& cpu, Nes& nes) -> void{
  cpu.y = cpu.fetch(nes);

  cpu.status.set(Cpu::Status::Zero, cpu.y == 0);
  cpu.status.set(Cpu::Status::Negative, cpu.y >> 7);
}

static auto sta(Cpu& cpu, Nes& nes) -> void{
  nes.mem_write(cpu.absolute_address, cpu.accumulator );
}

static auto stx(Cpu& cpu, Nes& nes) -> void{
  nes.mem_write(cpu.absolute_address, cpu.x);
}

static auto sty(Cpu& cpu, Nes& nes) -> void{
  nes.mem_write(cpu.absolute_address, cpu.y);
}

static auto tax(Cpu& cpu, Nes& nes) -> void{
  cpu.x = cpu.accumulator ;
  cpu.status.set(Cpu::Status::Zero, cpu.x == 0);
  cpu.status.set(Cpu::Status::Negative, cpu.x >> 7);
}

static auto tay(Cpu& cpu, Nes& nes) -> void{
  cpu.y = cpu.accumulator ;
  cpu.status.set(Cpu::Status::Zero, cpu.y == 0);
  cpu.status.set(Cpu::Status::Negative, cpu.y >> 7);
}

static auto tsx(Cpu& cpu, Nes& nes) -> void{
  cpu.x = cpu.sp;
  cpu.status.set(Cpu::Status::Zero, cpu.x == 0);
  cpu.status.set(Cpu::Status::Negative, cpu.x >> 7);
}

static auto txa(Cpu& cpu, Nes& nes) -> void{
  cpu.accumulator = cpu.x;
  cpu.status.set(Cpu::Status::Zero, cpu.accumulator   == 0);
  cpu.status.set(Cpu::Status::Negative, cpu.accumulator  >> 7);
}

static auto txs(Cpu& cpu, Nes& nes) -> void{
  cpu.sp = cpu.x;
}

static auto tya(Cpu& cpu, Nes& nes) -> void{
  cpu.accumulator = cpu.y;
  cpu.status.set(Cpu::Status::Zero, cpu.accumulator   == 0);
  cpu.status.set(Cpu::Status::Negative, cpu.accumulator  >> 7);
}

static auto pha(Cpu& cpu, Nes& nes) -> void{
  cpu.stack_push(nes, cpu.accumulator );
}

static auto php(Cpu& cpu, Nes& nes) -> void{
  cpu.stack_push(nes, cpu.status.plus(Cpu::Status::Unused).plus(Cpu::Status::BreakCommand).value);
}

static auto pla(Cpu& cpu, Nes& nes) -> void{
  cpu.sp++;
  cpu.accumulator  = nes.mem_read(Cpu::StackEnd + cpu.sp);
  cpu.status.set(Cpu::Status::Zero, cpu.accumulator   == 0);
  cpu.status.set(Cpu::Status::Negative, cpu.accumulator  >> 7);
}

static auto plp(Cpu& cpu, Nes& nes) -> void{
  cpu.sp++;
  cpu.status.value = nes.mem_read(Cpu::StackEnd + cpu.sp);
  cpu.status.set(Cpu::Status::BreakCommand, 0);
}

static auto and_(Cpu& cpu, Nes& nes) -> void{
  const u8 value = cpu.fetch(nes);
  cpu.accumulator  &= value;

  cpu.status.set(Cpu::Status::Zero, cpu.accumulator   == 0);
  cpu.status.set(Cpu::Status::Negative, cpu.accumulator  >> 7);
}

static auto eor(Cpu& cpu, Nes& nes) -> void{
  const u8 value = cpu.fetch(nes);
  cpu.accumulator  ^= value;

  cpu.status.set(Cpu::Status::Zero, cpu.accumulator   == 0);
  cpu.status.set(Cpu::Status::Negative, cpu.accumulator  >> 7);
}

static auto ora(Cpu& cpu, Nes& nes) -> void{
  const u8 value = cpu.fetch(nes);
  cpu.accumulator  |= value;

  cpu.status.set(Cpu::Status::Zero, cpu.accumulator   == 0);
  cpu.status.set(Cpu::Status::Negative, cpu.accumulator  >> 7);
}

static auto bit(Cpu& cpu, Nes& nes) -> void{
  const u8 value = cpu.fetch(nes);
  const u8 result = cpu.accumulator & value;

  cpu.status.set(Cpu::Status::Zero, result == 0);
  cpu.status.set(Cpu::Status::Overflow, value & (1 << 6));
  cpu.status.set(Cpu::Status::Negative, value & (1 << 7));
}

static auto adc(Cpu& cpu, Nes& nes) -> void{
  const u8 value = cpu.fetch(nes);
  const u8 carry = cpu.status.get(Cpu::Status::Carry);

  const uint16_t result = cpu.accumulator  + value + carry;
  cpu.status.set(Cpu::Status::Carry, result > 255);
  cpu.status.set(Cpu::Status::Zero, (result & 0x00FF) == 0);

  const u8 sign_bit_incorrect = ((value ^ result) & (cpu.accumulator  ^ result)) & (1 << 7);
  cpu.status.set(Cpu::Status::Overflow, sign_bit_incorrect);
  cpu.status.set(Cpu::Status::Negative, result & (1 << 7));
  cpu.accumulator  = result;
}

static auto sbc(Cpu& cpu, Nes& nes) -> void{
  const u8 value = cpu.fetch(nes);
  const u8 carry = cpu.status.get(Cpu::Status::Carry);

  const uint16_t result = cpu.accumulator  + u8(~value) + carry;
  cpu.status.set(Cpu::Status::Carry, result > 255);
  cpu.status.set(Cpu::Status::Zero, (result & 0x00FF) == 0);

  const u8 sign_bit_incorrect = ((u8(~value) ^ result) & (cpu.accumulator  ^ result)) & (1 << 7);
  cpu.status.set(Cpu::Status::Overflow, sign_bit_incorrect);
  cpu.status.set(Cpu::Status::Negative, result & (1 << 7));
  cpu.accumulator  = result;
}

static auto cmp(Cpu& cpu, Nes& nes) -> void{
  const u8 value = cpu.fetch(nes);
  cpu.status.set(Cpu::Status::Carry, value <= cpu.accumulator );
  cpu.status.set(Cpu::Status::Zero, value == cpu.accumulator );
  const uint16_t result = cpu.accumulator  - value;
  cpu.status.set(Cpu::Status::Negative, result & (1 << 7));
}

static auto cpx(Cpu& cpu, Nes& nes) -> void{
  const u8 value = cpu.fetch(nes);
  cpu.status.set(Cpu::Status::Carry, value <= cpu.x);
  cpu.status.set(Cpu::Status::Zero, value == cpu.x);
  const uint16_t result = cpu.x - value;
  cpu.status.set(Cpu::Status::Negative, result & (1 << 7));
}

static auto cpy(Cpu& cpu, Nes& nes) -> void{
  const u8 value = cpu.fetch(nes);
  cpu.status.set(Cpu::Status::Carry, value <= cpu.y);
  cpu.status.set(Cpu::Status::Zero, value == cpu.y);
  const uint16_t result = cpu.y - value;
  cpu.status.set(Cpu::Status::Negative, result & (1 << 7));
}

static auto inc(Cpu& cpu, Nes& nes) -> void{
  const u8 value = cpu.fetch(nes);
  const u8 result = value + 1;

  nes.mem_write(cpu.absolute_address, result);
  cpu.status.set(Cpu::Status::Zero, result == 0);
  cpu.status.set(Cpu::Status::Negative, result & (1 << 7));
}

static auto inx(Cpu& cpu, Nes& nes) -> void{
  cpu.x++;

  cpu.status.set(Cpu::Status::Zero, cpu.x == 0);
  cpu.status.set(Cpu::Status::Negative, cpu.x & (1 << 7));
}

static auto iny(Cpu& cpu, Nes& nes) -> void{
  cpu.y++;

  cpu.status.set(Cpu::Status::Zero, cpu.y == 0);
  cpu.status.set(Cpu::Status::Negative, cpu.y & (1 << 7));
}

static auto dec(Cpu& cpu, Nes& nes) -> void{
  const u8 value = cpu.fetch(nes);
  const u8 result = value - 1;

  nes.mem_write(cpu.absolute_address, result);
  cpu.status.set(Cpu::Status::Zero, result == 0);
  cpu.status.set(Cpu::Status::Negative, result & (1 << 7));
}

static auto dex(Cpu& cpu, Nes& nes) -> void{
  cpu.x--;

  cpu.status.set(Cpu::Status::Zero, cpu.x == 0);
  cpu.status.set(Cpu::Status::Negative, cpu.x & (1 << 7));
}

static auto dey(Cpu& cpu, Nes& nes) -> void{
  cpu.y--;

  cpu.status.set(Cpu::Status::Zero, cpu.y == 0);
  cpu.status.set(Cpu::Status::Negative, cpu.y & (1 << 7));
}

static auto asl(Cpu& cpu, Nes& nes) -> void{
  u8 result = cpu.accumulator_addressing ? cpu.accumulator : cpu.fetch(nes);

  cpu.status.set(Cpu::Status::Carry, result >> 7);
  result <<= 1;
  cpu.status.set(Cpu::Status::Zero, result == 0);
  cpu.status.set(Cpu::Status::Negative, result & (1 << 7));

  if (cpu.accumulator_addressing){
    cpu.accumulator  = result;
  }
  else{
    nes.mem_write(cpu.absolute_address, result);
  }
}

static auto lsr(Cpu& cpu, Nes& nes) -> void{
  u8 result = cpu.accumulator_addressing ? cpu.accumulator  : cpu.fetch(nes);

  cpu.status.set(Cpu::Status::Carry, result & 1);
  result >>= 1;
  cpu.status.set(Cpu::Status::Zero, result == 0);
  cpu.status.set(Cpu::Status::Negative, result & (1 << 7));

  if (cpu.accumulator_addressing){
    cpu.accumulator  = result;
  }
  else{
    nes.mem_write(cpu.absolute_address, result);
  }
}

static auto rol(Cpu& cpu, Nes& nes) -> void{
  u8 result = cpu.accumulator_addressing ? cpu.accumulator  : cpu.fetch(nes);

  const u8 msb = result >> 7;
  result <<= 1;
  result |= cpu.status.get(Cpu::Status::Carry);
  cpu.status.set(Cpu::Status::Carry, msb);
  cpu.status.set(Cpu::Status::Zero, result == 0);
  cpu.status.set(Cpu::Status::Negative, result & (1 << 7));

  if (cpu.accumulator_addressing){
    cpu.accumulator  = result;
  }
  else{
    nes.mem_write(cpu.absolute_address, result);
  }
}

static auto ror(Cpu& cpu, Nes& nes) -> void{
  u8 result = cpu.accumulator_addressing ? cpu.accumulator  : cpu.fetch(nes);

  const u8 lsb = result & 1;
  result >>= 1;
  result |= cpu.status.get(Cpu::Status::Carry) << 7;
  cpu.status.set(Cpu::Status::Carry, lsb);
  cpu.status.set(Cpu::Status::Zero, result == 0);
  cpu.status.set(Cpu::Status::Negative, result & (1 << 7));

  if (cpu.accumulator_addressing){
    cpu.accumulator  = result;
  }
  else{
    nes.mem_write(cpu.absolute_address, result);
  }
}

static auto jmp(Cpu& cpu, Nes& nes) -> void{
  cpu.pc = cpu.absolute_address;
}

static auto jsr(Cpu& cpu, Nes& nes) -> void{
  cpu.pc--;

  cpu.stack_push(nes, (cpu.pc >> 8) & 0x00FF);
  cpu.stack_push(nes, cpu.pc & 0x00FF);

  cpu.pc = cpu.absolute_address;
}

static auto rts(Cpu& cpu, Nes& nes) -> void{
  cpu.pc = cpu.stack_pull_u16(nes);
  cpu.pc++;
}

static auto cpu_branch(Cpu& cpu){
  cpu.req_cycles++;
  cpu.absolute_address = cpu.pc + cpu.relative_address;
  if (cpu.relative_address > 127){
    cpu.absolute_address -= 256;
  }

  if ((cpu.pc & 0xFF00) != (cpu.absolute_address & 0xFF00)) cpu.req_cycles++;
  cpu.pc = cpu.absolute_address;
}

static auto bcc(Cpu& cpu, Nes& nes) -> void{
  if (cpu.status.get(Cpu::Status::Carry) == 0){
    cpu_branch(cpu);
  }
}

static auto bcs(Cpu& cpu, Nes& nes) -> void{
  if (cpu.status.get(Cpu::Status::Carry)){
    cpu_branch(cpu);
  }
}

static auto beq(Cpu& cpu, Nes& nes) -> void{
  if (cpu.status.get(Cpu::Status::Zero) == 1){
    cpu_branch(cpu);
  }
}

static auto bne(Cpu& cpu, Nes& nes) -> void{
  if (cpu.status.get(Cpu::Status::Zero) == 0){
    cpu_branch(cpu);
  }
}

static auto bpl(Cpu& cpu, Nes& nes) -> void{
  if (cpu.status.get(Cpu::Status::Negative) == 0){
    cpu_branch(cpu);
  }
}

static auto bmi(Cpu& cpu, Nes& nes) -> void{
  if (cpu.status.get(Cpu::Status::Negative)){
    cpu_branch(cpu);
  }
}

static auto bvc(Cpu& cpu, Nes& nes) -> void{
  if (cpu.status.get(Cpu::Status::Overflow) == 0){
    cpu_branch(cpu);
  }
}

static auto bvs(Cpu& cpu, Nes& nes) -> void{
  if (cpu.status.get(Cpu::Status::Overflow)){
    cpu_branch(cpu);
  }
}

static auto clc(Cpu& cpu, Nes& nes) -> void{
  cpu.status.set(Cpu::Status::Carry, 0);
}

static auto cld(Cpu& cpu, Nes& nes) -> void{
  cpu.status.set(Cpu::Status::DecimalMode, 0);
}

static auto cli(Cpu& cpu, Nes& nes) -> void{
  cpu.status.set(Cpu::Status::InterruptDisable, 0);
}

static auto clv(Cpu& cpu, Nes& nes) -> void{
  cpu.status.set(Cpu::Status::Overflow, 0);
}

static auto sec(Cpu& cpu, Nes& nes) -> void{
  cpu.status.set(Cpu::Status::Carry, 1);
}

static auto sed(Cpu& cpu, Nes& nes) -> void{
  cpu.status.set(Cpu::Status::DecimalMode, 1);
}

static auto sei(Cpu& cpu, Nes& nes) -> void{
  cpu.status.set(Cpu::Status::InterruptDisable, 1);
}

static auto brk(Cpu& cpu, Nes& nes) -> void{
  cpu.status.set(Cpu::Status::BreakCommand, 1);
  cpu.stack_push(nes, cpu.pc << 8);
  cpu.stack_push(nes, cpu.pc);
  cpu.stack_push(nes, cpu.status.value);

  cpu.pc = nes.mem_read(0xFFFE) | u16(nes.mem_read(0xFFFF) << 8);
}

static auto rti(Cpu& cpu, Nes& nes) -> void{
  cpu.status.value = cpu.stack_pull(nes);

  cpu.status.set(Cpu::Status::BreakCommand, 0);

  cpu.pc = cpu.stack_pull_u16(nes);
}

//Illegal opcodes:
static auto lax(Cpu& cpu, Nes& nes) -> void{
  lda(cpu, nes);
  tax(cpu, nes);
}

static auto sax(Cpu& cpu, Nes& nes) -> void{
  nes.mem_write(cpu.absolute_address, cpu.accumulator  & cpu.x);
}

static auto dcp(Cpu& cpu, Nes& nes) -> void{
  dec(cpu, nes);
  cmp(cpu, nes);
}

static auto isc(Cpu& cpu, Nes& nes) -> void{
  inc(cpu, nes);
  sbc(cpu, nes);
}

static auto slo(Cpu& cpu, Nes& nes) -> void{
  asl(cpu, nes);
  ora(cpu, nes);
}

static auto sre(Cpu& cpu, Nes& nes) -> void{
  lsr(cpu, nes);
  eor(cpu, nes);
}

static auto rra(Cpu& cpu, Nes& nes) -> void{
  ror(cpu, nes);
  adc(cpu, nes);
}

static auto rla(Cpu& cpu, Nes& nes) -> void{
  rol(cpu, nes);
  and_(cpu, nes);
}

Cpu::Cpu(){
  auto& lookup = instruction_lookup;

  auto instruction = &ora;
  lookup[0x09] = { 2, AddressMode::Immediate, instruction };
  lookup[0x05] = { 3, AddressMode::ZeroPage, instruction };
  lookup[0x15] = { 4, AddressMode::ZeroPageX, instruction };
  lookup[0x0D] = { 4, AddressMode::Absolute, instruction };
  lookup[0x1D] = { 4, AddressMode::AbsoluteX, instruction };
  lookup[0x19] = { 4, AddressMode::AbsoluteY, instruction };
  lookup[0x01] = { 6, AddressMode::XIndirect, instruction };
  lookup[0x11] = { 5, AddressMode::IndirectY, instruction };

  instruction = &and_;
  lookup[0x29] = { 2, AddressMode::Immediate, instruction };
  lookup[0x25] = { 3, AddressMode::ZeroPage, instruction };
  lookup[0x35] = { 4, AddressMode::ZeroPageX, instruction };
  lookup[0x2D] = { 4, AddressMode::Absolute, instruction };
  lookup[0x3D] = { 4, AddressMode::AbsoluteX, instruction };
  lookup[0x39] = { 4, AddressMode::AbsoluteY, instruction };
  lookup[0x21] = { 6, AddressMode::XIndirect, instruction };
  lookup[0x31] = { 5, AddressMode::IndirectY, instruction };

  instruction = &eor;
  lookup[0x49] = { 2, AddressMode::Immediate, instruction };
  lookup[0x45] = { 3, AddressMode::ZeroPage, instruction };
  lookup[0x55] = { 4, AddressMode::ZeroPageX, instruction };
  lookup[0x4D] = { 4, AddressMode::Absolute, instruction };
  lookup[0x5D] = { 4, AddressMode::AbsoluteX, instruction };
  lookup[0x59] = { 4, AddressMode::AbsoluteY, instruction };
  lookup[0x41] = { 6, AddressMode::XIndirect, instruction };
  lookup[0x51] = { 5, AddressMode::IndirectY, instruction };
  
  instruction = &adc;
  lookup[0x69] = { 2, AddressMode::Immediate, instruction };
  lookup[0x65] = { 3, AddressMode::ZeroPage, instruction };
  lookup[0x75] = { 4, AddressMode::ZeroPageX, instruction };
  lookup[0x6D] = { 4, AddressMode::Absolute, instruction };
  lookup[0x7D] = { 4, AddressMode::AbsoluteX, instruction };
  lookup[0x79] = { 4, AddressMode::AbsoluteY, instruction };
  lookup[0x61] = { 6, AddressMode::XIndirect, instruction };
  lookup[0x71] = { 5, AddressMode::IndirectY, instruction };

  instruction = &asl;
  lookup[0x0A] = { 2, AddressMode::Accumulator, instruction };
  lookup[0x06] = { 5, AddressMode::ZeroPage, instruction };
  lookup[0x16] = { 6, AddressMode::ZeroPageX, instruction };
  lookup[0x0E] = { 6, AddressMode::Absolute, instruction };
  lookup[0x1E] = { 7, AddressMode::AbsoluteX, instruction };

  instruction = &bcc;
  lookup[0x90] = { 2, AddressMode::Relative, instruction };

  instruction = &bcs;
  lookup[0xB0] = { 2, AddressMode::Relative, instruction };

  instruction = &beq;
  lookup[0xF0] = { 2, AddressMode::Relative, instruction };

  instruction = &bit;
  lookup[0x24] = { 3, AddressMode::ZeroPage, instruction };
  lookup[0x2C] = { 4, AddressMode::Absolute, instruction };

  instruction = &bmi;
  lookup[0x30] = { 2, AddressMode::Relative, instruction };

  instruction = &bne;
  lookup[0xD0] = { 2, AddressMode::Relative, instruction };

  instruction = &bpl;
  lookup[0x10] = { 2, AddressMode::Relative, instruction };

  instruction = &brk;
  lookup[0x00] = { 7, AddressMode::Immediate, instruction };

  instruction = &bvc;
  lookup[0x50] = { 2, AddressMode::Relative, instruction };

  instruction = &bvs;
  lookup[0x70] = { 2, AddressMode::Relative, instruction };

  instruction = &clc;
  lookup[0x18] = { 2, AddressMode::Implied, instruction };

  instruction = &cld;
  lookup[0xD8] = { 2, AddressMode::Implied, instruction };

  instruction = &cli;
  lookup[0x58] = { 2, AddressMode::Implied, instruction };

  instruction = &clv;
  lookup[0xB8] = { 2, AddressMode::Implied, instruction };

  instruction = &cmp;
  lookup[0xC9] = { 2, AddressMode::Immediate, instruction };
  lookup[0xC5] = { 3, AddressMode::ZeroPage, instruction };
  lookup[0xD5] = { 4, AddressMode::ZeroPageX, instruction };
  lookup[0xCD] = { 4, AddressMode::Absolute, instruction };
  lookup[0xDD] = { 4, AddressMode::AbsoluteX, instruction };
  lookup[0xD9] = { 4, AddressMode::AbsoluteY, instruction };
  lookup[0xC1] = { 6, AddressMode::XIndirect, instruction };
  lookup[0xD1] = { 5, AddressMode::IndirectY, instruction };

  instruction = &cpx;
  lookup[0xE0] = { 2, AddressMode::Immediate, instruction };
  lookup[0xE4] = { 3, AddressMode::ZeroPage, instruction };
  lookup[0xEC] = { 4, AddressMode::Absolute, instruction };

  instruction = &cpy;
  lookup[0xC0] = { 2, AddressMode::Immediate, instruction };
  lookup[0xC4] = { 3, AddressMode::ZeroPage, instruction };
  lookup[0xCC] = { 4, AddressMode::Absolute, instruction };

  instruction = &dec;
  lookup[0xC6] = { 5, AddressMode::ZeroPage, instruction };
  lookup[0xD6] = { 6, AddressMode::ZeroPageX, instruction };
  lookup[0xCE] = { 6, AddressMode::Absolute, instruction };
  lookup[0xDE] = { 7, AddressMode::AbsoluteX, instruction };

  instruction = &dex;
  lookup[0xCA] = { 2, AddressMode::Implied, instruction };

  instruction = &dey;
  lookup[0x88] = { 2, AddressMode::Implied, instruction };

  instruction = &inc;
  lookup[0xE6] = { 5, AddressMode::ZeroPage, instruction };
  lookup[0xF6] = { 6, AddressMode::ZeroPageX, instruction };
  lookup[0xEE] = { 6, AddressMode::Absolute, instruction };
  lookup[0xFE] = { 7, AddressMode::AbsoluteX, instruction };

  instruction = &inx;
  lookup[0xE8] = { 2, AddressMode::Implied, instruction };

  instruction = &iny;
  lookup[0xC8] = { 2, AddressMode::Implied, instruction };

  instruction = &jmp;
  lookup[0x4C] = { 3, AddressMode::Absolute, instruction };
  lookup[0x6C] = { 5, AddressMode::Indirect, instruction };

  instruction = &jsr;
  lookup[0x20] = { 6, AddressMode::Absolute, instruction };

  instruction = &lda;
  lookup[0xA9] = { 2, AddressMode::Immediate, instruction };
  lookup[0xA5] = { 3, AddressMode::ZeroPage, instruction };
  lookup[0xB5] = { 4, AddressMode::ZeroPageX, instruction };
  lookup[0xAD] = { 4, AddressMode::Absolute, instruction };
  lookup[0xBD] = { 4, AddressMode::AbsoluteX, instruction };
  lookup[0xB9] = { 4, AddressMode::AbsoluteY, instruction };
  lookup[0xA1] = { 6, AddressMode::XIndirect, instruction };
  lookup[0xB1] = { 5, AddressMode::IndirectY, instruction };

  instruction = &ldx;
  lookup[0xA2] = { 2, AddressMode::Immediate, instruction };
  lookup[0xA6] = { 3, AddressMode::ZeroPage, instruction };
  lookup[0xB6] = { 4, AddressMode::ZeroPageY, instruction };
  lookup[0xAE] = { 4, AddressMode::Absolute, instruction };
  lookup[0xBE] = { 4, AddressMode::AbsoluteY, instruction };

  instruction = &ldy;
  lookup[0xA0] = { 2, AddressMode::Immediate, instruction };
  lookup[0xA4] = { 3, AddressMode::ZeroPage, instruction };
  lookup[0xB4] = { 4, AddressMode::ZeroPageX, instruction };
  lookup[0xAC] = { 4, AddressMode::Absolute, instruction };
  lookup[0xBC] = { 4, AddressMode::AbsoluteX, instruction };

  instruction = &lsr;
  lookup[0x4A] = { 2, AddressMode::Accumulator, instruction };
  lookup[0x46] = { 5, AddressMode::ZeroPage, instruction };
  lookup[0x56] = { 6, AddressMode::ZeroPageX, instruction };
  lookup[0x4E] = { 6, AddressMode::Absolute, instruction };
  lookup[0x5E] = { 7, AddressMode::AbsoluteX, instruction };

  instruction = &nop;
  lookup[0xEA] = { 2, AddressMode::Implied, instruction };

  instruction = &pha;
  lookup[0x48] = { 3, AddressMode::Implied, instruction };

  instruction = &php;
  lookup[0x08] = { 3, AddressMode::Implied, instruction };

  instruction = &pla;
  lookup[0x68] = { 4, AddressMode::Implied, instruction };

  instruction = &plp;
  lookup[0x28] = { 4, AddressMode::Implied, instruction };

  instruction = &rol;
  lookup[0x2A] = { 2, AddressMode::Accumulator, instruction };
  lookup[0x26] = { 5, AddressMode::ZeroPage, instruction };
  lookup[0x36] = { 6, AddressMode::ZeroPageX, instruction };
  lookup[0x2E] = { 6, AddressMode::Absolute, instruction };
  lookup[0x3E] = { 7, AddressMode::AbsoluteX, instruction };

  instruction = &ror;
  lookup[0x6A] = { 2, AddressMode::Accumulator, instruction };
  lookup[0x66] = { 5, AddressMode::ZeroPage, instruction };
  lookup[0x76] = { 6, AddressMode::ZeroPageX, instruction };
  lookup[0x6E] = { 6, AddressMode::Absolute, instruction };
  lookup[0x7E] = { 7, AddressMode::AbsoluteX, instruction };

  instruction = &rti;
  lookup[0x40] = { 6, AddressMode::Implied, instruction };

  instruction = &rts;
  lookup[0x60] = { 6, AddressMode::Implied, instruction };

  instruction = &sbc;
  lookup[0xE9] = { 2, AddressMode::Immediate, instruction };
  lookup[0xE5] = { 3, AddressMode::ZeroPage, instruction };
  lookup[0xF5] = { 4, AddressMode::ZeroPageX, instruction };
  lookup[0xED] = { 4, AddressMode::Absolute, instruction };
  lookup[0xFD] = { 4, AddressMode::AbsoluteX, instruction };
  lookup[0xF9] = { 4, AddressMode::AbsoluteY, instruction };
  lookup[0xE1] = { 6, AddressMode::XIndirect, instruction };
  lookup[0xF1] = { 5, AddressMode::IndirectY, instruction };

  instruction = &sec;
  lookup[0x38] = { 2, AddressMode::Implied, instruction };

  instruction = &sed;
  lookup[0xF8] = { 2, AddressMode::Implied, instruction };

  instruction = &sei;
  lookup[0x78] = { 2, AddressMode::Implied, instruction };

  instruction = &sta;
  lookup[0x85] = { 3, AddressMode::ZeroPage, instruction };
  lookup[0x95] = { 4, AddressMode::ZeroPageX, instruction };
  lookup[0x8D] = { 4, AddressMode::Absolute, instruction };
  lookup[0x9D] = { 5, AddressMode::AbsoluteX, instruction };
  lookup[0x99] = { 5, AddressMode::AbsoluteY, instruction };
  lookup[0x81] = { 6, AddressMode::XIndirect, instruction };
  lookup[0x91] = { 6, AddressMode::IndirectY, instruction };

  instruction = &stx;
  lookup[0x86] = { 3, AddressMode::ZeroPage, instruction };
  lookup[0x96] = { 4, AddressMode::ZeroPageY, instruction };
  lookup[0x8E] = { 4, AddressMode::Absolute, instruction };

  instruction = &sty;
  lookup[0x84] = { 3, AddressMode::ZeroPage, instruction };
  lookup[0x94] = { 4, AddressMode::ZeroPageX, instruction };
  lookup[0x8C] = { 4, AddressMode::Absolute, instruction };

  instruction = &tax;
  lookup[0xAA] = { 2, AddressMode::Implied, instruction };

  instruction = &tay;
  lookup[0xA8] = { 2, AddressMode::Implied, instruction };

  instruction = &tsx;
  lookup[0xBA] = { 2, AddressMode::Implied, instruction };

  instruction = &txa;
  lookup[0x8A] = { 2, AddressMode::Implied, instruction };

  instruction = &txs;
  lookup[0x9A] = { 2, AddressMode::Implied, instruction };

  instruction = &tya;
  lookup[0x98] = { 2, AddressMode::Implied, instruction };

  //ILLEGAL OPCODES:

  instruction = &nop;
  lookup[0x04] = { 3, AddressMode::ZeroPage, instruction };
  lookup[0x44] = { 3, AddressMode::ZeroPage, instruction };
  lookup[0x64] = { 3, AddressMode::ZeroPage, instruction };

  lookup[0x0C] = { 4, AddressMode::Absolute, instruction };

  lookup[0x14] = { 4, AddressMode::ZeroPageX, instruction };
  lookup[0x34] = { 4, AddressMode::ZeroPageX, instruction };
  lookup[0x54] = { 4, AddressMode::ZeroPageX, instruction };
  lookup[0x74] = { 4, AddressMode::ZeroPageX, instruction };
  lookup[0xD4] = { 4, AddressMode::ZeroPageX, instruction };
  lookup[0xF4] = { 4, AddressMode::ZeroPageX, instruction };

  lookup[0x1A] = { 2, AddressMode::Implied, instruction };
  lookup[0x3A] = { 2, AddressMode::Implied, instruction };
  lookup[0x5A] = { 2, AddressMode::Implied, instruction };
  lookup[0x7A] = { 2, AddressMode::Implied, instruction };
  lookup[0xDA] = { 2, AddressMode::Implied, instruction };
  lookup[0xEA] = { 2, AddressMode::Implied, instruction };
  lookup[0xFA] = { 2, AddressMode::Implied, instruction };

  lookup[0x80] = { 2, AddressMode::Immediate, instruction };
  lookup[0x82] = { 2, AddressMode::Immediate, instruction };
  lookup[0x89] = { 2, AddressMode::Immediate, instruction };
  lookup[0xC2] = { 2, AddressMode::Immediate, instruction };
  lookup[0xE2] = { 2, AddressMode::Immediate, instruction };

  lookup[0x1C] = { 4, AddressMode::AbsoluteX, instruction };
  lookup[0x3C] = { 4, AddressMode::AbsoluteX, instruction };
  lookup[0x5C] = { 4, AddressMode::AbsoluteX, instruction };
  lookup[0x7C] = { 4, AddressMode::AbsoluteX, instruction };
  lookup[0xDC] = { 4, AddressMode::AbsoluteX, instruction };
  lookup[0xFC] = { 4, AddressMode::AbsoluteX, instruction };

  instruction = &lax;
  lookup[0xA3] = { 6, AddressMode::XIndirect, instruction };
  lookup[0xA7] = { 3, AddressMode::ZeroPage, instruction };
  lookup[0xAF] = { 4, AddressMode::Absolute, instruction };
  lookup[0xB3] = { 5, AddressMode::IndirectY, instruction };
  lookup[0xB7] = { 4, AddressMode::ZeroPageY, instruction };
  lookup[0xBF] = { 4, AddressMode::AbsoluteY, instruction };

  instruction = &sax;
  lookup[0x83] = { 6, AddressMode::XIndirect, instruction };
  lookup[0x87] = { 3, AddressMode::ZeroPage, instruction };
  lookup[0x8F] = { 4, AddressMode::Absolute, instruction };
  lookup[0x97] = { 4, AddressMode::ZeroPageY, instruction };

  //SBC Duplicate
  instruction = &sbc;
  lookup[0xEB] = { 2, AddressMode::Immediate, instruction };

  instruction = &dcp;
  lookup[0xC3] = { 8, AddressMode::XIndirect, instruction };
  lookup[0xC7] = { 5, AddressMode::ZeroPage, instruction };
  lookup[0xCF] = { 6, AddressMode::Absolute, instruction };
  lookup[0xD3] = { 8, AddressMode::IndirectY, instruction };
  lookup[0xD7] = { 6, AddressMode::ZeroPageX, instruction };
  lookup[0xDB] = { 7, AddressMode::AbsoluteY, instruction };
  lookup[0xDF] = { 7, AddressMode::AbsoluteX, instruction };

  instruction = &isc;
  lookup[0xE3] = { 8, AddressMode::XIndirect, instruction };
  lookup[0xE7] = { 5, AddressMode::ZeroPage, instruction };
  lookup[0xEF] = { 6, AddressMode::Absolute, instruction };
  lookup[0xF3] = { 8, AddressMode::IndirectY, instruction };
  lookup[0xF7] = { 6, AddressMode::ZeroPageX, instruction };
  lookup[0xFB] = { 7, AddressMode::AbsoluteY, instruction };
  lookup[0xFF] = { 7, AddressMode::AbsoluteX, instruction };

  instruction = &rla;
  lookup[0x23] = { 8, AddressMode::XIndirect, instruction };
  lookup[0x27] = { 5, AddressMode::ZeroPage, instruction };
  lookup[0x2F] = { 6, AddressMode::Absolute, instruction };
  lookup[0x33] = { 8, AddressMode::IndirectY, instruction };
  lookup[0x37] = { 6, AddressMode::ZeroPageX, instruction };
  lookup[0x3B] = { 7, AddressMode::AbsoluteY, instruction };
  lookup[0x3F] = { 7, AddressMode::AbsoluteX, instruction };

  instruction = &rra;
  lookup[0x63] = { 8, AddressMode::XIndirect, instruction };
  lookup[0x67] = { 5, AddressMode::ZeroPage, instruction };
  lookup[0x6F] = { 6, AddressMode::Absolute, instruction };
  lookup[0x73] = { 8, AddressMode::IndirectY, instruction };
  lookup[0x77] = { 6, AddressMode::ZeroPageX, instruction };
  lookup[0x7B] = { 7, AddressMode::AbsoluteY, instruction };
  lookup[0x7F] = { 7, AddressMode::AbsoluteX, instruction };

  instruction = &slo;
  lookup[0x03] = { 8, AddressMode::XIndirect, instruction };
  lookup[0x07] = { 5, AddressMode::ZeroPage, instruction };
  lookup[0x0F] = { 6, AddressMode::Absolute, instruction };
  lookup[0x13] = { 8, AddressMode::IndirectY, instruction };
  lookup[0x17] = { 6, AddressMode::ZeroPageX, instruction };
  lookup[0x1B] = { 7, AddressMode::AbsoluteY, instruction };
  lookup[0x1F] = { 7, AddressMode::AbsoluteX, instruction };

  instruction = &sre;
  lookup[0x43] = { 8, AddressMode::XIndirect, instruction };
  lookup[0x47] = { 5, AddressMode::ZeroPage, instruction };
  lookup[0x4F] = { 6, AddressMode::Absolute, instruction };
  lookup[0x53] = { 8, AddressMode::IndirectY, instruction };
  lookup[0x57] = { 6, AddressMode::ZeroPageX, instruction };
  lookup[0x5B] = { 7, AddressMode::AbsoluteY, instruction };
  lookup[0x5F] = { 7, AddressMode::AbsoluteX, instruction };

  lookup[0x1C].may_req_additional_cycle = true;
  lookup[0x3C].may_req_additional_cycle = true;
  lookup[0x5C].may_req_additional_cycle = true;
  lookup[0x7C].may_req_additional_cycle = true;
  lookup[0xDC].may_req_additional_cycle = true;
  lookup[0xFC].may_req_additional_cycle = true;

  for (auto& instruction : lookup){
    const auto additional_cycle_instructions = {
      ora, and_, eor, adc, cmp, lda, ldx, ldy, sbc, lax
    };

    if (
      std::find(
        additional_cycle_instructions.begin(),
        additional_cycle_instructions.end(),
        instruction.call_ptr
      ) != additional_cycle_instructions.end()
    ){
      instruction.may_req_additional_cycle = true;
    }
  }
}

auto Cpu::set_address_mode(Nes& nes, Cpu::AddressMode mode) -> bool{
  using Mode = Cpu::AddressMode;
  switch(mode){
    case Mode::None:
      break;

    case Mode::Accumulator:
      break;

    case Mode::Immediate:
      absolute_address = pc;
      pc++;
      break;

    case Mode::ZeroPage:
      absolute_address = nes.mem_read(pc) & 0x00FF;
      pc++;
      break;

    case Mode::ZeroPageX:
      absolute_address = nes.mem_read(pc) + x;
      absolute_address &= 0x00FF;
      pc++;
      break;

    case Mode::ZeroPageY:
      absolute_address = nes.mem_read(pc) + y;
      absolute_address &= 0x00FF;
      pc++;
      break;

    case Mode::Relative:
      relative_address = nes.mem_read(pc);
      pc++;
      break;

    case Mode::Absolute: {
      absolute_address = nes.mem_read_u16(pc);                           
      pc += 2;

      break;
    }

    case Mode::AbsoluteX:{
      absolute_address = nes.mem_read_u16(pc);
      const auto high = absolute_address & 0xFF00;

      absolute_address += x;
      pc += 2;

      //Check if page changed:
      if ((absolute_address & 0xFF00) != high) return MayRequireAdditionalCycle;

      break;
    }

    case Mode::AbsoluteY:{
      absolute_address = nes.mem_read_u16(pc);
      const auto high = absolute_address & 0xFF00;

      absolute_address += y;
      pc += 2;

      //Check if page changed:
      if ((absolute_address & 0xFF00) != high) return MayRequireAdditionalCycle;

      break;

    }

    case Mode::Indirect:{
      const auto ptr = nes.mem_read_u16(pc);
      const auto ptr_low = ptr & 0x00FF;

      const auto low = nes.mem_read(ptr);
      const auto high = ptr_low == 0x00FF 
        ? nes.mem_read(ptr & 0xFF00)
        : nes.mem_read(ptr + 1);

      pc += 2;
      absolute_address = make_u16(high, low);
      break;
    }

    case Mode::XIndirect:{
      const auto ptr = nes.mem_read(pc);

      const auto low = nes.mem_read(uint16_t(ptr + x) & 0x00FF);
      const auto high = nes.mem_read(uint16_t(ptr + x + 1) & 0x00FF);

      absolute_address = make_u16(high, low);
      pc++;

      break;
    }

    case Mode::IndirectY:{
      const auto ptr = nes.mem_read(pc);

      const u8 low = nes.mem_read(uint16_t(ptr) & 0x00FF);
      const u8 high = nes.mem_read(uint16_t(ptr + 1) & 0x00FF);
      absolute_address = make_u16(high, low);
      absolute_address += y;
      pc++;

      //Check if page changed:
      if ((absolute_address & 0xFF00) != (high << 8)) return MayRequireAdditionalCycle;

      break;
    }

    case Mode::Implied:
      break;
  }

  return 0;
}

auto Cpu::fetch(Nes& nes) -> u8{
  return nes.mem_read(absolute_address);
}

auto Cpu::stack_push(Nes& nes, u8 data) -> void{
  nes.mem_write(Cpu::StackEnd + sp, data);
  sp--;
}

auto Cpu::stack_push_u16(Nes& nes, u16 data) -> void{
  stack_push(nes, (data >> 8) & 0x00FF);
  stack_push(nes, data & 0x00FF);
}

auto Cpu::stack_pull(Nes& nes) -> u8{
  sp++;
  u8 value = nes.mem_read(Cpu::StackEnd + sp);
  return value;
}

auto Cpu::stack_pull_u16(Nes& nes) -> u16{
  const auto low = stack_pull(nes);
  const auto high = stack_pull(nes);

  return make_u16(high, low);
}

auto Cpu::execute_instruction(Nes& nes, u8 opcode) -> bool{
  auto instruction = instruction_lookup[opcode];

  if (instruction.call_ptr == nullptr){
    throw std::runtime_error(hex_str(instruction_pc) + " Unsupported opcode: " + hex_str(opcode));
  }

  const auto may_req_additional_cycle = set_address_mode(nes, instruction.address_mode);
  this->req_cycles = instruction.req_cycles;
  if (instruction.may_req_additional_cycle && may_req_additional_cycle){
    this->req_cycles++;
  }

  accumulator_addressing = instruction.address_mode == AddressMode::Accumulator;

  instruction.call_ptr(*this, nes);

  return false;
}

auto Cpu::clock(Nes& nes) -> void{
  if (req_cycles == 0){
    const u8 cmd = nes.mem_read(pc);

    next_instruction_started = true;
    instruction_pc = pc;

    pc++;

    const auto requires_additional_cycle = execute_instruction(nes, cmd);
    if (requires_additional_cycle) req_cycles++;

    status.set(Cpu::Status::Unused, 1);
  }

  req_cycles--;
}

static auto cpu_interrupt(Cpu& cpu, Nes& nes, u16 absolute_address){
  cpu.stack_push_u16(nes, cpu.pc);

  cpu.status.set(Cpu::Status::BreakCommand, 0);
  cpu.status.set(Cpu::Status::Unused, 1);
  cpu.status.set(Cpu::Status::InterruptDisable, 1);

  cpu.stack_push(nes, cpu.status.value);

  cpu.absolute_address = absolute_address;
  cpu.pc = nes.mem_read_u16(absolute_address);

  cpu.req_cycles = 7;
}

auto Cpu::irq(Nes& nes) -> void{
  if (status.get(Cpu::Status::InterruptDisable) == 0){
    cpu_interrupt(*this, nes, 0xFFFE);
  }
}

auto Cpu::nmi(Nes& nes) -> void{
  cpu_interrupt(*this, nes, 0xFFFA);
}

} //namespace nes
