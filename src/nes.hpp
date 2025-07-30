#pragma once

#include <array>
#include "util.hpp"
#include "aliases.hpp"
#include "cardridge.hpp"
#include "ppu.hpp"
#include "cpu.hpp"

namespace nes{

struct Nes{
  static constexpr auto CpuMemAddressRange = std::make_pair(0x0000, 0x1FFF);
  static constexpr auto CpuMemSize = 0x0800;

  static constexpr auto PpuMemAddressRange = std::make_pair(0x2000, 0x3FFF);

  Cpu cpu;
  Ppu ppu;
  Cardridge cardridge;
  std::array<u8, 1024 * 8> ram;
  
  u32 cycles = 0;

  auto load_cardridge(const std::string& filepath){
    cardridge.from_file(filepath);

    u16 lo = mem_read(0xFFFC);
    u16 hi = mem_read(0xFFFD);

    cpu.pc = (hi << 8) | lo;
  }

  auto mem_read(u16 address) -> u8{
    const auto cardridge_data = cardridge.read_program(address);
    if (cardridge_data != std::nullopt){
      return cardridge_data.value();
    }
    else if (in_range(address, CpuMemAddressRange)){
      return ram[address & (Nes::CpuMemSize - 1)];
    }
    else if (in_range(address, PpuMemAddressRange)){
      //Can mutate PPU!!!
      return ppu.cpu_read(*this, address);
    }

    return u8(0);
  }

  auto mem_write(u16 address, u8 value){
    if (cardridge.write_program(address, value)){

    }
    else if (in_range(address, CpuMemAddressRange)){
      ram[address & (CpuMemSize - 1)] = value;
    }
    else if (in_range(address, PpuMemAddressRange)){
      return ppu.cpu_write(*this, address, value);
    }
  }
  
  auto ppu_read(u16 address) const{
    return ppu.mem_read(*this, address);
  }

  auto ppu_write(u16 address, u8 value){
    return ppu.mem_write(*this, address, value);
  }

  auto clock(){
    ppu.clock();

    if (cycles % 3 == 0){
      cpu.clock(*this);
    }
    cycles++;
  }

  auto frame_complete(){
    return ppu.frame_complete;
  }

};

} //namepace nes
