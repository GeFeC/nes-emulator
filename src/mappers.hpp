#pragma once

#include "aliases.hpp"
#include "util.hpp"
#include <optional>

namespace nes{

inline auto mapper000_cpu_map(u16 address, u8 program_chunks) -> std::optional<u16>{
  if (in_range(address, std::make_pair(0x8000, 0xFFFF))){
    return address & (program_chunks > 1 ? 0x7FFF : 0x3FFF);
  }

  return std::nullopt;
}

inline auto mapper000_ppu_map(u16 address, u8 program_chunks) -> std::optional<u16>{
  if (in_range(address, std::make_pair(0x0000, 0x1FFF))){
    return address;
  }

  return std::nullopt;
}

} //namespace nes
