#pragma once

#include "aliases.hpp"
#include "util.hpp"
#include <optional>

namespace nes{

struct Mapper{
  virtual auto cpu_write(u16 address) -> std::optional<u16> = 0;
  virtual auto cpu_read(u16 address) -> std::optional<u16> = 0;
  virtual auto ppu_write(u16 address) -> std::optional<u16> = 0;
  virtual auto ppu_read(u16 address) -> std::optional<u16> = 0;
};

struct Mapper000 : Mapper{
  u8 program_chunks;

  Mapper000(u8 program_chunks) : program_chunks(program_chunks) {}

  auto cpu_read(u16 address) -> std::optional<u16> override{
    if (in_range(address, std::make_pair(0x8000, 0xFFFF))){
      return address & (program_chunks > 1 ? 0x7FFF : 0x3FFF);
    }

    return std::nullopt;
  }

  auto cpu_write(u16 address) -> std::optional<u16> override{
    //The same as read
    return cpu_read(address);
  }

  auto ppu_read(u16 address) -> std::optional<u16> override{
    if (in_range(address, std::make_pair(0x0000, 0x1FFF))){
      return address;
    }

    return std::nullopt;
  }

  auto ppu_write(u16 address) -> std::optional<u16> override{
    //The same as read
    return ppu_read(address);
  }
};

} //namespace nes
