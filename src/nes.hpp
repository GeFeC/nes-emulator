#pragma once

#include <array>
#include "util.hpp"
#include "aliases.hpp"
#include "cardridge.hpp"
#include "ppu.hpp"
#include "cpu.hpp"
#include "apu.hpp"

namespace nes{

struct Nes{
  static constexpr auto AudioSampleRate = 44100.0;
  static constexpr auto CyclesPerSec = 5369318.0;

  static constexpr auto CpuMemAddressRange = std::make_pair(0x0000, 0x1FFF);
  static constexpr auto CpuMemSize = 0x0800;

  static constexpr auto PpuMemAddressRange = std::make_pair(0x2000, 0x3FFF);

  static constexpr auto Controller1Address = 0x4016;
  static constexpr auto DMAAddress = 0x4014;

  Cpu cpu;
  Ppu ppu;
  Apu apu = Apu(*this);
  Cardridge cardridge;
  std::array<u8, 1024 * 8> ram;
  
  u32 cycles = 0;

  u8 controllers[2]{};
  u8 controller_buffers[2]{};

  u8 dma_page = 0;
  u8 dma_address = 0;
  u8 dma_data = 0;

  bool dma_transfer_started = false;
  bool dma_dummy_cycle = true;

  double audio_time = 0.0;
  double audio_sample = 0.0;

  auto in_apu_range(u16 address) const{
    return in_range(address, std::make_pair(0x4000, 0x4013)) || address == 0x4015 || address == 0x4017;
  }

  auto load_cardridge(const std::string& filepath){
    cardridge.from_file(filepath);
    cpu.absolute_address = 0xFFFC;

    u16 lo = mem_read(cpu.absolute_address);
    u16 hi = mem_read(cpu.absolute_address + 1);

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
    else if (address == Controller1Address){
      const auto data = (controller_buffers[0] & 0x80) > 0;
      controller_buffers[0] <<= 1;

      return data;
    }
    else if (in_apu_range(address)){
      return apu.cpu_read(address);
    }

    return u8(0);
  }

  auto mem_read_u16(u16 address) -> u16{
    const auto low = mem_read(address);
    const auto high = mem_read(address + 1);

    return (high << 8) | low;
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
    else if (address == DMAAddress){
      dma_page = value;
      dma_address = 0;
      dma_transfer_started = true;
    }
    else if (address == Controller1Address){
      controller_buffers[0] = controllers[0];
    }
    else if (in_apu_range(address)){
      apu.cpu_write(address, value);
    }
  }
  
  auto ppu_read(u16 address) const{
    return ppu.mem_read(*this, address);
  }

  auto ppu_write(u16 address, u8 value){
    return ppu.mem_write(*this, address, value);
  }

  auto cpu_clock(){
    if (!dma_transfer_started){
      cpu.clock(*this);
      return;
    }

    if (dma_dummy_cycle){
      if (cycles % 2 == 1){
        dma_dummy_cycle = false;
        return;
      }
    } 

    if (cycles % 2 == 0){
      dma_data = mem_read(dma_page << 8 | dma_address);
    }
    else{
      reinterpret_cast<u8*>(ppu.oam)[dma_address] = dma_data;
      dma_address++;

      if (dma_address == 0){
        dma_transfer_started = false;
        dma_dummy_cycle = true;
      }
    }
  }

  auto clock(){
    apu.clock();
    ppu.clock(*this);

    if (cycles % 3 == 0){
      cpu_clock();      
    }

    if (ppu.nmi){
      ppu.nmi = false;
      cpu.nmi(*this);
    }

    auto audio_sample_ready = false;
    audio_time += 1.0 / CyclesPerSec;
    if (audio_time >= 1.0 / AudioSampleRate){
      audio_time -= 1.0 / AudioSampleRate;
      audio_sample_ready = true;
    }

    cycles++;

    return audio_sample_ready;
  }

  auto frame_complete(){
    return ppu.frame_complete;
  }

  auto ready_for_rendering(){
    return ppu.palettes_started_loading;
  }

};

} //namepace nes
