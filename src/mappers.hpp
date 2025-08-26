#pragma once

#include "aliases.hpp"
#include "util.hpp"
#include <optional>

namespace nes{

struct Mapper{
  using return_t = std::optional<u32>;
  virtual auto cpu_write(u16 address, u8 data) -> return_t = 0;
  virtual auto cpu_read(u16 address) -> return_t = 0;
  virtual auto ppu_write(u16 address, u8 data) -> return_t = 0;
  virtual auto ppu_read(u16 address) -> return_t = 0;
};

struct Mapper000 : Mapper{
  u8 program_chunks;

  Mapper000(u8 program_chunks) : program_chunks(program_chunks) {}

  auto cpu_read(u16 address) -> return_t override{
    if (in_range(address, { 0x8000, 0xFFFF })){
      return address & (program_chunks > 1 ? 0x7FFF : 0x3FFF);
    }

    return std::nullopt;
  }

  auto cpu_write(u16 address, u8 data) -> return_t override{
    //The same as read
    return cpu_read(address);
  }

  auto ppu_read(u16 address) -> return_t override{
    if (in_range(address, { 0x0000, 0x1FFF })){
      return address;
    }

    return std::nullopt;
  }

  auto ppu_write(u16 address, u8 data) -> return_t override{
    //The same as read
    return ppu_read(address);
  }
};

struct Mapper002 : Mapper{
  u8 selected_program_bank_low = 0;
  u8 selected_program_bank_high = 0;
  u8 program_chunks;
  u8 char_chunks;

  Mapper002(u8 program_chunks, u8 char_chunks) 
  : program_chunks(program_chunks), char_chunks(char_chunks) {
    selected_program_bank_high = program_chunks - 1;
  }

  auto cpu_read(u16 address) -> return_t override{
    if (in_range(address, { 0x8000, 0xBFFF })){
      return selected_program_bank_low * 16_kb + (address & 0x3FFF);
    }
    if (in_range(address, { 0xC000, 0xFFFF })){
      return selected_program_bank_high * 16_kb + (address & 0x3FFF);
    }

    return std::nullopt;
  }

  auto cpu_write(u16 address, u8 data) -> return_t override{
    if (in_range(address, { 0x8000, 0xFFFF })){
      selected_program_bank_low = data & 0x0F;
    }
    return std::nullopt;
  }

  auto ppu_read(u16 address) -> return_t override{
    if (in_range(address, { 0x0000, 0x1fff })){
      return (address & 0x1fff);
    }

    return std::nullopt;
  }

  auto ppu_write(u16 address, u8 data) -> return_t override{
    if (in_range(address, { 0x0000, 0x1fff })){
      if (char_chunks == 0){
        return (address & 0x1fff);
      }
    }

    return std::nullopt;
  }
};

} //namespace nes
