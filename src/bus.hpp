#pragma once

#include <array>
#include "util.hpp"
#include "aliases.hpp"
#include "cardridge.hpp"
#include "ppu.hpp"
#include "renderer/math.hpp"

namespace nes{

struct Bus{
  static constexpr auto CpuRamAddressRange = std::make_pair(0x0000, 0x1FFF);
  static constexpr auto CpuRamSize = 0x0800;

  static constexpr auto PpuRamAddressRange = std::make_pair(0x2000, 0x3FFF);

  Cardridge cardridge;
  Ppu ppu;
  std::array<uint8_t, 1024 * 8> ram;
};

inline auto bus_read(const Bus& bus, uint16_t address) -> u8{
  const auto cardridge_data = cardridge_cpu_read(bus.cardridge, address);
  if (cardridge_data != std::nullopt){
    return cardridge_data.value();
  }
  else if (in_range(address, Bus::CpuRamAddressRange)){
    return bus.ram[address & (Bus::CpuRamSize - 1)];
  }
  else if (in_range(address, Bus::PpuRamAddressRange)){
    //Can mutate PPU!!!
    auto& mutable_bus = const_cast<Bus&>(bus);
    return ppu_cpu_read(&mutable_bus.ppu, bus.cardridge, address);
  }

  return u8(0);
}

inline auto bus_write(Bus* bus, uint16_t address, uint8_t value){
  if (cardridge_cpu_write(&bus->cardridge, address, value)){

  }
  else if (in_range(address, Bus::CpuRamAddressRange)){
    bus->ram[address & (Bus::CpuRamSize - 1)] = value;
  }
  else if (in_range(address, Bus::PpuRamAddressRange)){
    return ppu_cpu_write(&bus->ppu, bus->cardridge, address, value);
  }
}

} //namepace nes
