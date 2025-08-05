#include "cpu.hpp"
#include "nes.hpp"
#include "instruction_info.hpp"
#include <iostream>

namespace nes{

Cpu::Cpu(){
  instruction_info = get_instruction_info();
}

auto Cpu::get_addressing_data(Nes& nes, Cpu::AddressMode mode) -> Cpu::AddressingData{
  auto data = Cpu::AddressingData{};
  data.new_absolute_address = absolute_address;

  using Mode = Cpu::AddressMode;
  switch(mode){
    case Mode::None:
      return data;

    case Mode::Accumulator:
      return data;

    case Mode::Immediate:
      data.new_absolute_address = pc;
      data.instruction_size = 1;
      return data;

    case Mode::ZeroPage:
      data.new_absolute_address = nes.mem_read(pc) & 0x00FF;
      data.instruction_size = 1;
      return data;

    case Mode::ZeroPageX:
      data.new_absolute_address = nes.mem_read(pc) + x;
      data.new_absolute_address &= 0x00FF;
      data.instruction_size = 1;
      return data;

    case Mode::ZeroPageY:
      data.new_absolute_address = nes.mem_read(pc) + y;
      data.new_absolute_address &= 0x00FF;
      data.instruction_size = 1;
      return data;

    case Mode::Relative:
      data.new_relative_address = nes.mem_read(pc);
      data.instruction_size = 1;
      return data;

    case Mode::Absolute: {
      data.new_absolute_address = nes.mem_read_u16(pc);                           
      data.instruction_size = 2;

      return data;
    }

    case Mode::AbsoluteX:{
      data.new_absolute_address = nes.mem_read_u16(pc);
      const auto high = data.new_absolute_address & 0xFF00;

      data.new_absolute_address += x;
      data.instruction_size = 2;

      //Check if page changed:
      if ((data.new_absolute_address & 0xFF00) != high) data.may_req_additional_cycle = true;

      return data;
    }

    case Mode::AbsoluteY:{
      data.new_absolute_address = nes.mem_read_u16(pc);
      const auto high = data.new_absolute_address & 0xFF00;

      data.new_absolute_address += y;
      data.instruction_size = 2;

      //Check if page changed:
      if ((data.new_absolute_address & 0xFF00) != high) data.may_req_additional_cycle = true;

      return data;

    }

    case Mode::Indirect:{
      const auto ptr = nes.mem_read_u16(pc);
      const auto ptr_low = ptr & 0x00FF;

      const auto low = nes.mem_read(ptr);
      const auto high = ptr_low == 0x00FF 
        ? nes.mem_read(ptr & 0xFF00)
        : nes.mem_read(ptr + 1);

      data.instruction_size = 2;
      data.new_absolute_address = make_u16(high, low);
      return data;
    }

    case Mode::XIndirect:{
      const auto ptr = nes.mem_read(pc);

      const auto low = nes.mem_read(uint16_t(ptr + x) & 0x00FF);
      const auto high = nes.mem_read(uint16_t(ptr + x + 1) & 0x00FF);

      data.new_absolute_address = make_u16(high, low);
      data.instruction_size = 1;

      return data;
    }

    case Mode::IndirectY:{
      const auto ptr = nes.mem_read(pc);

      const u8 low = nes.mem_read(uint16_t(ptr) & 0x00FF);
      const u8 high = nes.mem_read(uint16_t(ptr + 1) & 0x00FF);
      data.new_absolute_address = make_u16(high, low);
      data.new_absolute_address += y;
      data.instruction_size = 1;

      //Check if page changed:
      if ((data.new_absolute_address & 0xFF00) != (high << 8)) data.may_req_additional_cycle = true;

      return data;
    }

    case Mode::Implied:
      return data;
  }
  return data;
}

auto Cpu::set_status(u8 flag, u8 state) -> void{
  if (state){
    status |= flag;
  }
  else{
    status &= (~flag);
  }
}

auto Cpu::get_status(u8 flag) -> u8{
  return (status & flag) > 0;
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

auto Cpu::lda(Nes& nes) -> void{
  accumulator = fetch(nes);

  set_status(Cpu::Status::Zero, accumulator == 0);
  set_status(Cpu::Status::Negative, accumulator >> 7);
}

auto Cpu::ldx(Nes& nes) -> void{
  x = fetch(nes);

  set_status(Cpu::Status::Zero, x == 0);
  set_status(Cpu::Status::Negative, x >> 7);
}

auto Cpu::ldy(Nes& nes) -> void{
  y = fetch(nes);

  set_status(Cpu::Status::Zero, y == 0);
  set_status(Cpu::Status::Negative, y >> 7);
}

auto Cpu::sta(Nes& nes) -> void{
  nes.mem_write(absolute_address, accumulator);
}

auto Cpu::stx(Nes& nes) -> void{
  nes.mem_write(absolute_address, x);
}

auto Cpu::sty(Nes& nes) -> void{
  nes.mem_write(absolute_address, y);
}

auto Cpu::tax() -> void{
  x = accumulator;
  set_status(Cpu::Status::Zero, x == 0);
  set_status(Cpu::Status::Negative, x >> 7);
}

auto Cpu::tay() -> void{
  y = accumulator;
  set_status(Cpu::Status::Zero, y == 0);
  set_status(Cpu::Status::Negative, y >> 7);
}

auto Cpu::tsx() -> void{
  x = sp;
  set_status(Cpu::Status::Zero, x == 0);
  set_status(Cpu::Status::Negative, x >> 7);
}

auto Cpu::txa() -> void{
  accumulator = x;
  set_status(Cpu::Status::Zero, accumulator  == 0);
  set_status(Cpu::Status::Negative, accumulator >> 7);
}

auto Cpu::txs() -> void{
  sp = x;
}

auto Cpu::tya() -> void{
  accumulator = y;
  set_status(Cpu::Status::Zero, accumulator  == 0);
  set_status(Cpu::Status::Negative, accumulator >> 7);
}

auto Cpu::pha(Nes& nes) -> void{
  stack_push(nes, accumulator);
}

auto Cpu::php(Nes& nes) -> void{
  stack_push(nes, status | Cpu::Status::Unused | Cpu::Status::BreakCommand);
}

auto Cpu::pla(Nes& nes) -> void{
  sp++;
  accumulator = nes.mem_read(Cpu::StackEnd + sp);
  set_status(Cpu::Status::Zero, accumulator  == 0);
  set_status(Cpu::Status::Negative, accumulator >> 7);
}

auto Cpu::plp(Nes& nes) -> void{
  sp++;
  status = nes.mem_read(Cpu::StackEnd + sp);
  set_status(Cpu::Status::BreakCommand, 0);
}

auto Cpu::and_(Nes& nes) -> void{
  const u8 value = fetch(nes);
  accumulator &= value;

  set_status(Cpu::Status::Zero, accumulator  == 0);
  set_status(Cpu::Status::Negative, accumulator >> 7);
}

auto Cpu::eor(Nes& nes) -> void{
  const u8 value = fetch(nes);
  accumulator ^= value;

  set_status(Cpu::Status::Zero, accumulator  == 0);
  set_status(Cpu::Status::Negative, accumulator >> 7);
}

auto Cpu::ora(Nes& nes) -> void{
  const u8 value = fetch(nes);
  accumulator |= value;

  set_status(Cpu::Status::Zero, accumulator  == 0);
  set_status(Cpu::Status::Negative, accumulator >> 7);
}

auto Cpu::bit(Nes& nes) -> void{
  const u8 value = fetch(nes);
  const u8 result = accumulator & value;

  set_status(Cpu::Status::Zero, result == 0);
  set_status(Cpu::Status::Overflow, value & (1 << 6));
  set_status(Cpu::Status::Negative, value & (1 << 7));
}

auto Cpu::adc(Nes& nes) -> void{
  const u8 value = fetch(nes);
  const u8 carry = get_status(Cpu::Status::Carry);

  const uint16_t result = accumulator + value + carry;
  set_status(Cpu::Status::Carry, result > 255);
  set_status(Cpu::Status::Zero, (result & 0x00FF) == 0);

  const u8 sign_bit_incorrect = ((value ^ result) & (accumulator ^ result)) & (1 << 7);
  set_status(Cpu::Status::Overflow, sign_bit_incorrect);
  set_status(Cpu::Status::Negative, result & (1 << 7));
  accumulator = result;
}

auto Cpu::sbc(Nes& nes) -> void{
  const u8 value = fetch(nes);
  const u8 carry = get_status(Cpu::Status::Carry);

  const uint16_t result = accumulator + u8(~value) + carry;
  set_status(Cpu::Status::Carry, result > 255);
  set_status(Cpu::Status::Zero, (result & 0x00FF) == 0);

  const u8 sign_bit_incorrect = ((u8(~value) ^ result) & (accumulator ^ result)) & (1 << 7);
  set_status(Cpu::Status::Overflow, sign_bit_incorrect);
  set_status(Cpu::Status::Negative, result & (1 << 7));
  accumulator = result;
}

auto Cpu::cmp(Nes& nes) -> void{
  const u8 value = fetch(nes);
  set_status(Cpu::Status::Carry, value <= accumulator);
  set_status(Cpu::Status::Zero, value == accumulator);
  const uint16_t result = accumulator - value;
  set_status(Cpu::Status::Negative, result & (1 << 7));
}

auto Cpu::cpx(Nes& nes) -> void{
  const u8 value = fetch(nes);
  set_status(Cpu::Status::Carry, value <= x);
  set_status(Cpu::Status::Zero, value == x);
  const uint16_t result = x - value;
  set_status(Cpu::Status::Negative, result & (1 << 7));
}

auto Cpu::cpy(Nes& nes) -> void{
  const u8 value = fetch(nes);
  set_status(Cpu::Status::Carry, value <= y);
  set_status(Cpu::Status::Zero, value == y);
  const uint16_t result = y - value;
  set_status(Cpu::Status::Negative, result & (1 << 7));
}

auto Cpu::inc(Nes& nes) -> void{
  const u8 value = fetch(nes);
  const u8 result = value + 1;

  nes.mem_write(absolute_address, result);
  set_status(Cpu::Status::Zero, result == 0);
  set_status(Cpu::Status::Negative, result & (1 << 7));
}

auto Cpu::inx() -> void{
  x++;

  set_status(Cpu::Status::Zero, x == 0);
  set_status(Cpu::Status::Negative, x & (1 << 7));
}

auto Cpu::iny() -> void{
  y++;

  set_status(Cpu::Status::Zero, y == 0);
  set_status(Cpu::Status::Negative, y & (1 << 7));
}

auto Cpu::dec(Nes& nes) -> void{
  const u8 value = fetch(nes);
  const u8 result = value - 1;

  nes.mem_write(absolute_address, result);
  set_status(Cpu::Status::Zero, result == 0);
  set_status(Cpu::Status::Negative, result & (1 << 7));
}

auto Cpu::dex() -> void{
  x--;

  set_status(Cpu::Status::Zero, x == 0);
  set_status(Cpu::Status::Negative, x & (1 << 7));
}

auto Cpu::dey() -> void{
  y--;

  set_status(Cpu::Status::Zero, y == 0);
  set_status(Cpu::Status::Negative, y & (1 << 7));
}

auto Cpu::asl(Nes& nes, bool accumulator_addressing = false) -> void{
  u8 result = accumulator_addressing ? accumulator : fetch(nes);

  set_status(Cpu::Status::Carry, result >> 7);
  result <<= 1;
  set_status(Cpu::Status::Zero, result == 0);
  set_status(Cpu::Status::Negative, result & (1 << 7));

  if (accumulator_addressing){
    accumulator = result;
  }
  else{
    nes.mem_write(absolute_address, result);
  }
}

auto Cpu::lsr(Nes& nes, bool accumulator_addressing = false) -> void{
  u8 result = accumulator_addressing ? accumulator : fetch(nes);

  set_status(Cpu::Status::Carry, result & 1);
  result >>= 1;
  set_status(Cpu::Status::Zero, result == 0);
  set_status(Cpu::Status::Negative, result & (1 << 7));

  if (accumulator_addressing){
    accumulator = result;
  }
  else{
    nes.mem_write(absolute_address, result);
  }
}

auto Cpu::rol(Nes& nes, bool accumulator_addressing = false) -> void{
  u8 result = accumulator_addressing ? this->accumulator : fetch(nes);

  const u8 msb = result >> 7;
  result <<= 1;
  result |= get_status(Cpu::Status::Carry);
  set_status(Cpu::Status::Carry, msb);
  set_status(Cpu::Status::Zero, result == 0);
  set_status(Cpu::Status::Negative, result & (1 << 7));

  if (accumulator_addressing){
    accumulator = result;
  }
  else{
    nes.mem_write(absolute_address, result);
  }
}

auto Cpu::ror(Nes& nes, bool accumulator_addressing = false) -> void{
  u8 result = accumulator_addressing ? accumulator : fetch(nes);

  const u8 lsb = result & 1;
  result >>= 1;
  result |= get_status(Cpu::Status::Carry) << 7;
  set_status(Cpu::Status::Carry, lsb);
  set_status(Cpu::Status::Zero, result == 0);
  set_status(Cpu::Status::Negative, result & (1 << 7));

  if (accumulator_addressing){
    accumulator = result;
  }
  else{
    nes.mem_write(absolute_address, result);
  }
}

auto Cpu::jmp() -> void{
  pc = absolute_address;
}

auto Cpu::jsr(Nes& nes) -> void{
  pc--;

  stack_push(nes, (pc >> 8) & 0x00FF);
  stack_push(nes, pc & 0x00FF);

  pc = absolute_address;
}

auto Cpu::rts(Nes& nes) -> void{
  pc = stack_pull_u16(nes);
  pc++;
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

auto Cpu::bcc() -> void{
  if (get_status(Cpu::Status::Carry) == 0){
    cpu_branch(*this);
  }
}

auto Cpu::bcs() -> void{
  if (get_status(Cpu::Status::Carry)){
    cpu_branch(*this);
  }
}

auto Cpu::beq() -> void{
  if (get_status(Cpu::Status::Zero) == 1){
    cpu_branch(*this);
  }
}

auto Cpu::bne() -> void{
  if (get_status(Cpu::Status::Zero) == 0){
    cpu_branch(*this);
  }
}

auto Cpu::bpl() -> void{
  if (get_status(Cpu::Status::Negative) == 0){
    cpu_branch(*this);
  }
}

auto Cpu::bmi() -> void{
  if (get_status(Cpu::Status::Negative)){
    cpu_branch(*this);
  }
}

auto Cpu::bvc() -> void{
  if (get_status(Cpu::Status::Overflow) == 0){
    cpu_branch(*this);
  }
}

auto Cpu::bvs() -> void{
  if (get_status(Cpu::Status::Overflow)){
    cpu_branch(*this);
  }
}

auto Cpu::clc() -> void{
  set_status(Cpu::Status::Carry, 0);
}

auto Cpu::cld() -> void{
  set_status(Cpu::Status::DecimalMode, 0);
}

auto Cpu::cli() -> void{
  set_status(Cpu::Status::InterruptDisable, 0);
}

auto Cpu::clv() -> void{
  set_status(Cpu::Status::Overflow, 0);
}

auto Cpu::sec() -> void{
  set_status(Cpu::Status::Carry, 1);
}

auto Cpu::sed() -> void{
  set_status(Cpu::Status::DecimalMode, 1);
}

auto Cpu::sei() -> void{
  set_status(Cpu::Status::InterruptDisable, 1);
}

auto Cpu::brk(Nes& nes) -> void{
  set_status(Cpu::Status::BreakCommand, 1);
  stack_push(nes, pc << 8);
  stack_push(nes, pc);
  stack_push(nes, status);

  pc = nes.mem_read(0xFFFE) | u16(nes.mem_read(0xFFFF) << 8);
}

auto Cpu::rti(Nes& nes) -> void{
  status = stack_pull(nes);

  set_status(Cpu::Status::BreakCommand, 0);

  pc = stack_pull_u16(nes);
}

//Illegal opcodes:
auto Cpu::lax(Nes& nes) -> void{
  lda(nes);
  tax();
}

auto Cpu::sax(Nes& nes) -> void{
  nes.mem_write(absolute_address, accumulator & x);
}

auto Cpu::dcp(Nes& nes) -> void{
  dec(nes);
  cmp(nes);
}

auto Cpu::isc(Nes& nes) -> void{
  inc(nes);
  sbc(nes);
}

auto Cpu::slo(Nes& nes) -> void{
  asl(nes);
  ora(nes);
}

auto Cpu::sre(Nes& nes) -> void{
  lsr(nes);
  eor(nes);
}

auto Cpu::rra(Nes& nes) -> void{
  ror(nes);
  adc(nes);
}

auto Cpu::rla(Nes& nes) -> void{
  rol(nes);
  and_(nes);
}

auto Cpu::execute_instruction(Nes& nes, u8 instruction) -> bool{
  const auto [req_cycles, addressing] = instruction_info[instruction];
  this->req_cycles = req_cycles;

  const auto addressing_data = get_addressing_data(nes, addressing);
  const auto requires_additional_cycle = addressing_data.may_req_additional_cycle;

  absolute_address = addressing_data.new_absolute_address;
  relative_address = addressing_data.new_relative_address;
  pc += addressing_data.instruction_size;

  switch(instruction){
    case 0x09:
    case 0x05:
    case 0x15:
    case 0x0D:
    case 0x1D:
    case 0x19:
    case 0x01:
    case 0x11:
      ora(nes); return requires_additional_cycle;

    //AND
    case 0x29:
    case 0x25:
    case 0x35:
    case 0x2D:
    case 0x3D:
    case 0x39:
    case 0x21:
    case 0x31:
      and_(nes); return requires_additional_cycle;

    //EOR
    case 0x49:
    case 0x45:
    case 0x55:
    case 0x4D:
    case 0x5D:
    case 0x59:
    case 0x41:
    case 0x51:
      eor(nes); return requires_additional_cycle;
    
    //ADC
    case 0x69:
    case 0x65:
    case 0x75:
    case 0x6D:
    case 0x7D:
    case 0x79:
    case 0x61:
    case 0x71:
      adc(nes); return requires_additional_cycle;

    //ASL
    case 0x0A:
      asl(nes, true);
      break;

    case 0x06:
    case 0x16:
    case 0x0E:
    case 0x1E:
      asl(nes);
      break;

    //BCC
    case 0x90:
      bcc();
      break;

    //BCS
    case 0xB0:
      bcs();
      break;

    //BEQ
    case 0xF0:
      beq();
      break;

    //BIT
    case 0x24:
    case 0x2C:
      bit(nes);
      break;

    //BMI
    case 0x30:
      bmi();
      break;

    //BNE
    case 0xD0:
      bne();
      break;

    //BPL
    case 0x10:
      bpl();
      break;

    //BRK
    case 0x00:
      brk(nes);
      break;

    //BVC
    case 0x50:
      bvc();
      break;

    //BVS
    case 0x70:
      bvs();
      break;

    //CLC
    case 0x18:
      clc();
      break;

    //CLD
    case 0xD8:
      cld();
      break;

    //CLI
    case 0x58:
      cli();
      break;

    //CLV
    case 0xB8:
      clv();
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
      cmp(nes); return requires_additional_cycle;

    //CPX
    case 0xE0:
    case 0xE4:
    case 0xEC:
      cpx(nes);
      break;

    //CPY
    case 0xC0:
    case 0xC4:
    case 0xCC:
      cpy(nes);
      break;

    //DEC
    case 0xC6:
    case 0xD6:
    case 0xCE:
    case 0xDE:
      dec(nes);
      break;

    //DEX
    case 0xCA:
      dex();
      break;

    //DEY
    case 0x88:
      dey();
      break;

    //INC
    case 0xE6:
    case 0xF6:
    case 0xEE:
    case 0xFE:
      inc(nes);
      break;

    //INX
    case 0xE8:
      inx();
      break;

    //INY
    case 0xC8:
      iny();
      break;

    //JMP
    case 0x4C:
    case 0x6C:
      jmp();
      break;

    //JSR
    case 0x20:
      jsr(nes);
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
      lda(nes); return requires_additional_cycle;

    //LDX
    case 0xA2:
    case 0xA6:
    case 0xB6:
    case 0xAE:
    case 0xBE:
      ldx(nes); return requires_additional_cycle;

    //LDY
    case 0xA0:
    case 0xA4:
    case 0xB4:
    case 0xAC:
    case 0xBC:
      ldy(nes); return requires_additional_cycle;

    //LSR
    case 0x4A:
      lsr(nes, true);
      break;
    case 0x46:
    case 0x56:
    case 0x4E:
    case 0x5E:
      lsr(nes);
      break;

    //NOP
    case 0xEA:
      nop();
      break;

    //PHA
    case 0x48:
      pha(nes);
      break;

    //PHP
    case 0x08:
      php(nes);
      break;

    //PLA
    case 0x68:
      pla(nes);
      break;

    //PLP
    case 0x28:
      plp(nes);
      break;

    //ROL
    case 0x2A:
      rol(nes, true);
      break;

    case 0x26:
    case 0x36:
    case 0x2E:
    case 0x3E:
      rol(nes, false);
      break;

    //ROR
    case 0x6A:
      ror(nes, true);
      break;

    case 0x66:
    case 0x76:
    case 0x6E:
    case 0x7E:
      ror(nes, false);
      break;

    //RTI
    case 0x40:
      rti(nes);
      break;

    //RTS
    case 0x60:
      rts(nes);
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
      sbc(nes); return requires_additional_cycle;

    //SEC
    case 0x38:
      sec();
      break;

    //SED
    case 0xF8:
      sed();
      break;

    //SEI
    case 0x78:
      sei();
      break;

    //STA
    case 0x85:
    case 0x95:
    case 0x8D:
    case 0x9D:
    case 0x99:
    case 0x81:
    case 0x91:
      sta(nes);
      break;

    //STX
    case 0x86:
    case 0x96:
    case 0x8E:
      stx(nes);
      break;

    //STY
    case 0x84:
    case 0x94:
    case 0x8C:
      sty(nes);
      break;

    //TAX
    case 0xAA:
      tax();
      break;

    //TAY
    case 0xA8:
      tay();
      break;

    //TSX
    case 0xBA:
      tsx();
      break;

    //TXA
    case 0x8A:
      txa();
      break;

    //TXS
    case 0x9A:
      txs();
      break;

    //TYA
    case 0x98:
      tya();
      break;

    //Illegal opcodes:

    //LAX
    case 0xA3:
    case 0xA7:
    case 0xAF:
    case 0xB3:
    case 0xB7:
    case 0xBF:
      lax(nes); return requires_additional_cycle;

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
      return requires_additional_cycle;

    case 0x83:
    case 0x87:
    case 0x8F:
    case 0x97:
      sax(nes);
      break;

    case 0xEB:
      sbc(nes);
      break;

    case 0xC3:
    case 0xC7:
    case 0xCF:
    case 0xD3:
    case 0xD7:
    case 0xDB:
    case 0xDF:
      dcp(nes);
      break;

    case 0xE3:
    case 0xE7:
    case 0xEF:
    case 0xF3:
    case 0xF7:
    case 0xFB:
    case 0xFF:
      isc(nes);
      break;

    //RLA
    case 0x23:
    case 0x27:
    case 0x2F:
    case 0x33:
    case 0x37:
    case 0x3B:
    case 0x3F:
      rla(nes);
      break;

    //RRA
    case 0x63:
    case 0x67:
    case 0x6F:
    case 0x73:
    case 0x77:
    case 0x7B:
    case 0x7F:
      rra(nes);
      break;

    //SLO
    case 0x03:
    case 0x07:
    case 0x0F:
    case 0x13:
    case 0x17:
    case 0x1B:
    case 0x1F:
      slo(nes);
      break;

    //SRE
    case 0x43:
    case 0x47:
    case 0x4F:
    case 0x53:
    case 0x57:
    case 0x5B:
    case 0x5F:
      sre(nes);
      break;

    default:
      std::cerr << std::hex << pc << " Unsupported opcode: " << int(instruction) << '\n';
      throw std::runtime_error("");
      break;
  }

  return false;
}

auto Cpu::clock(Nes& nes) -> void{
  if (req_cycles == 0){
    const u8 cmd = nes.mem_read(pc);
    pc++;

    const auto requires_additional_cycle = execute_instruction(nes, cmd);
    if (requires_additional_cycle) req_cycles++;

    set_status(Cpu::Status::Unused, 1);
  }

  req_cycles--;
}

static auto cpu_interrupt(Cpu& cpu, Nes& nes, u16 absolute_address){
  cpu.stack_push_u16(nes, cpu.pc);

  cpu.set_status(Cpu::Status::BreakCommand, 0);
  cpu.set_status(Cpu::Status::Unused, 1);
  cpu.set_status(Cpu::Status::InterruptDisable, 1);

  cpu.stack_push(nes, cpu.status);

  cpu.absolute_address = absolute_address;
  cpu.pc = nes.mem_read_u16(absolute_address);

  cpu.req_cycles = 7;
}

auto Cpu::irq(Nes& nes) -> void{
  if (get_status(Cpu::Status::InterruptDisable) == 0){
    cpu_interrupt(*this, nes, 0xFFFE);
  }
}

auto Cpu::nmi(Nes& nes) -> void{
  cpu_interrupt(*this, nes, 0xFFFA);
}

} //namespace nes
