#include "ppu.hpp"
#include "nes.hpp"
#include "colors.hpp"

namespace nes{

Ppu::Ppu(){
  colors = get_colors();
}

auto Ppu::mem_read(const Nes& nes, u16 address) const -> u8{
  const auto cardridge_data = nes.cardridge.read_pattern(address);
  if (cardridge_data != std::nullopt){
    return cardridge_data.value(); 
  }
  else if (in_range(address, Ppu::LeftPatternTableAddressRange)){
    return left_pattern_table[address];
  }
  else if (in_range(address, Ppu::RightPatternTableAddressRange)){
    return right_pattern_table[address];
  }
  else if (in_range(address, Ppu::PalettesAddressRange)){
    address &= 0x001F; //address %= 32
    if (address > 16) address -= 16;

    return palettes[address];
  }

  return 0x00;
}

auto Ppu::mem_write(Nes& nes, u16 address, u8 value) -> void{
  if (nes.cardridge.write_pattern(address, value)){
  }
  else if (in_range(address, Ppu::LeftPatternTableAddressRange)){
    left_pattern_table[address] = value;
  }
  else if (in_range(address, Ppu::RightPatternTableAddressRange)){
    right_pattern_table[address] = value;
  }
  else if (in_range(address, Ppu::PalettesAddressRange)){
    palettes_started_loading = true;

    address &= 0x001F; //address %= 32
    if (address > 16) address -= 16;

    palettes[address] = value;
  }
}

auto Ppu::cpu_read(const Nes& nes, u16 address) -> u8{
  address &= 0x0007;
  switch(address){
    case 0x0002:{
      const auto status = this->status & 0b11100000;
      this->status &= ~Ppu::Status::VBlank;
      address_latch = Ppu::AddressLatch::MSB;
      return status;
    }

    case 0x0007:{
      auto data = data_buffer;
      data_buffer = mem_read(nes, this->address) | (data_buffer & 0x1F);

      if (this->address >= Ppu::PalettesAddressRange.first){
        data = data_buffer;
      }

      this->address++;
      return data;
    }
  }

  return 0x00;
}

auto Ppu::cpu_write(Nes& nes, u16 address, u8 value) -> void{
  address &= 0x0007;
  switch(address){
    case 0x0000:
      control = value;
      break;

    case 0x0001:
      mask = value;
      break;

    case 0x0006: 
      if (address_latch == Ppu::AddressLatch::LSB){
        this->address = (this->address & 0xFF00) | value;
        address_latch = Ppu::AddressLatch::MSB;
      }
      else{
        this->address = (this->address & 0x00FF) | (value << 8);
        address_latch = Ppu::AddressLatch::LSB;
      }
      break;
    case 0x0007:
      mem_write(nes, this->address, value);
      this->address++;
      break;
  }
}

auto Ppu::clock() -> void{
  frame_complete = false;

  cycles++;

  if (cycles > Ppu::CyclesPerScanline){
    cycles = 0;
    scanline++;
  }

  if (scanline > Ppu::ScreenSize.y && cycles == 0){
    status |= Ppu::Status::VBlank;
  }

  if (scanline > Ppu::MaxScanlines){
    scanline = -1;
    frame_complete = true;
  }
}



} //namespace nes
