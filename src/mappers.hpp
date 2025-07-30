#pragma once

#include "aliases.hpp"
#include "util.hpp"
#include <optional>

namespace nes{

struct Mapper{
  virtual auto program_map(u16 address, u8 program_chunks) -> std::optional<u16> = 0;
  virtual auto pattern_map(u16 address) -> std::optional<u16> = 0;
};

struct Mapper000 : Mapper{
  auto program_map(u16 address, u8 program_chunks) -> std::optional<u16> override{
    if (in_range(address, std::make_pair(0x8000, 0xFFFF))){
      return address & (program_chunks > 1 ? 0x7FFF : 0x3FFF);
    }

    return std::nullopt;
  }

  auto pattern_map(u16 address) -> std::optional<u16> override{
    if (in_range(address, std::make_pair(0x0000, 0x1FFF))){
      return address;
    }

    return std::nullopt;
  }
};

} //namespace nes
