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
  u8 selected_program_bank = 0;
  u8 program_chunks;
  u8 char_chunks;

  Mapper002(u8 program_chunks, u8 char_chunks) 
  : program_chunks(program_chunks), char_chunks(char_chunks) {
  }

  auto cpu_read(u16 address) -> return_t override{
    if (in_range(address, { 0x8000, 0xBFFF })){
      return selected_program_bank * 16_kb + (address - 0x8000);
    }
    if (in_range(address, { 0xC000, 0xFFFF })){
      return (program_chunks - 1) * 16_kb + (address - 0xC000);
    }

    return std::nullopt;
  }

  auto cpu_write(u16 address, u8 data) -> return_t override{
    if (in_range(address, { 0x8000, 0xFFFF })){
      selected_program_bank = data & 0x0F;
    }
    return std::nullopt;
  }

  auto ppu_read(u16 address) -> return_t override{
    if (in_range(address, { 0x0000, 0x1fff })){
      return (address);
    }

    return std::nullopt;
  }

  auto ppu_write(u16 address, u8 data) -> return_t override{
    if (in_range(address, { 0x0000, 0x1fff })){
      if (char_chunks == 0){
        return (address);
      }
    }

    return std::nullopt;
  }
};

struct Mapper003 : Mapper{
  u8 selected_char_bank = 0;
  u8 program_chunks;
  u8 char_chunks;

  Mapper003(u8 program_chunks, u8 char_chunks) 
  : program_chunks(program_chunks), char_chunks(char_chunks) {
  }

  auto cpu_read(u16 address) -> return_t override{
    if (in_range(address, { 0x8000, 0xFFFF })){
      return address & (program_chunks > 1 ? 0x7FFF : 0x3FFF);
    }

    return std::nullopt;
  }

  auto cpu_write(u16 address, u8 data) -> return_t override{
    if (in_range(address, { 0x8000, 0xFFFF })){
      selected_char_bank = data & 0x03;
      return address;
    }

    return std::nullopt;
  }

  auto ppu_read(u16 address) -> return_t override{
    if (in_range(address, { 0x0000, 0x1fff })){
      return selected_char_bank * 8_kb + address;
    }

    return std::nullopt;
  }

  auto ppu_write(u16 address, u8 data) -> return_t override{
    return std::nullopt;
  }
};

struct Mapper066 : Mapper{
  u8 selected_program_bank = 0;
  u8 selected_char_bank = 0;
  u8 program_chunks;
  u8 char_chunks;

  Mapper066(u8 program_chunks, u8 char_chunks) 
  : program_chunks(program_chunks), char_chunks(char_chunks) {
  }

  auto cpu_read(u16 address) -> return_t override{
    if (in_range(address, { 0x8000, 0xFFFF })){
      return selected_program_bank * 32_kb + (address & 0x7FFF);
    }

    return std::nullopt;
  }

  auto cpu_write(u16 address, u8 data) -> return_t override{
    if (in_range(address, { 0x8000, 0xFFFF })){
      selected_char_bank = data & 0x03;
      selected_program_bank = (data & 0x30) >> 4;
    }
    return std::nullopt;
  }

  auto ppu_read(u16 address) -> return_t override{
    if (in_range(address, { 0x0000, 0x1fff })){
      return selected_char_bank * 8_kb + address;
    }

    return std::nullopt;
  }

  auto ppu_write(u16 address, u8 data) -> return_t override{
    return std::nullopt;
  }
};



} //namespace nes
