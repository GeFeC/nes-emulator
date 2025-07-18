#pragma once

#include <array>
#include "util.hpp"
#include "aliases.hpp"
#include "cartridge.hpp"

namespace nes{

struct Bus{
  static constexpr auto CpuRamAddressRange = std::make_pair(0x0000, 0x1FFF);
  static constexpr auto CpuRamSize = 0x0800;

  Cardridge cardridge;
  std::array<uint8_t, 1024 * 8> ram;
};

inline auto bus_read(const Bus& bus, uint16_t address){
  const auto card_address = cardridge_map(bus.cardridge, address);
  if (card_address != std::nullopt){
    return bus.cardridge.program_memory[card_address.value()];
  }
  else if (in_range(address, Bus::CpuRamAddressRange)){
    return bus.ram[address & (Bus::CpuRamSize - 1)];
  }

  return u8(0);
}

inline auto bus_write(Bus* bus, uint16_t address, uint8_t value){
  const auto card_address = cardridge_map(bus->cardridge, address);
  if (card_address != std::nullopt){
    bus->cardridge.program_memory[card_address.value()] = value;
  }
  else if (in_range(address, Bus::CpuRamAddressRange)){
    bus->ram[address & (Bus::CpuRamSize - 1)] = value;
  }
}

} //namepace nes
