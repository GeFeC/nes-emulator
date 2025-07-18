#pragma once

#include <array>
#include "util.hpp"
#include "aliases.hpp"

namespace nes{

struct Bus{
  static constexpr auto CpuRamAddressRange = std::make_pair(0x0000, 0x1FFF);
  static constexpr auto CpuRamSize = 0x0800;

  std::array<uint8_t, 1024 * 8> ram;
};

inline auto bus_read(const Bus& bus, uint16_t address){
  if (in_range(address, Bus::CpuRamAddressRange)){
    return bus.ram[address & (Bus::CpuRamSize - 1)];
  }

  return u8(0);
}

inline auto bus_write(Bus* bus, uint16_t address, uint8_t value){
  if (in_range(address, Bus::CpuRamAddressRange)){
    bus->ram[address & (Bus::CpuRamSize - 1)] = value;
  }
}

} //namespace nes
