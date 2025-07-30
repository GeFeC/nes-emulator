#include "ppu.hpp"
#include "nes.hpp"

namespace nes{

Ppu::Ppu(){
  colors[0x00] = gf::math::vec3(84, 84, 84);
	colors[0x01] = gf::math::vec3(0, 30, 116);
	colors[0x02] = gf::math::vec3(8, 16, 144);
	colors[0x03] = gf::math::vec3(48, 0, 136);
	colors[0x04] = gf::math::vec3(68, 0, 100);
	colors[0x05] = gf::math::vec3(92, 0, 48);
	colors[0x06] = gf::math::vec3(84, 4, 0);
	colors[0x07] = gf::math::vec3(60, 24, 0);
	colors[0x08] = gf::math::vec3(32, 42, 0);
	colors[0x09] = gf::math::vec3(8, 58, 0);
	colors[0x0A] = gf::math::vec3(0, 64, 0);
	colors[0x0B] = gf::math::vec3(0, 60, 0);
	colors[0x0C] = gf::math::vec3(0, 50, 60);
	colors[0x0D] = gf::math::vec3(0, 0, 0);
	colors[0x0E] = gf::math::vec3(0, 0, 0);
	colors[0x0F] = gf::math::vec3(0, 0, 0);

	colors[0x10] = gf::math::vec3(152, 150, 152);
	colors[0x11] = gf::math::vec3(8, 76, 196);
	colors[0x12] = gf::math::vec3(48, 50, 236);
	colors[0x13] = gf::math::vec3(92, 30, 228);
	colors[0x14] = gf::math::vec3(136, 20, 176);
	colors[0x15] = gf::math::vec3(160, 20, 100);
	colors[0x16] = gf::math::vec3(152, 34, 32);
	colors[0x17] = gf::math::vec3(120, 60, 0);
	colors[0x18] = gf::math::vec3(84, 90, 0);
	colors[0x19] = gf::math::vec3(40, 114, 0);
	colors[0x1A] = gf::math::vec3(8, 124, 0);
	colors[0x1B] = gf::math::vec3(0, 118, 40);
	colors[0x1C] = gf::math::vec3(0, 102, 120);
	colors[0x1D] = gf::math::vec3(0, 0, 0);
	colors[0x1E] = gf::math::vec3(0, 0, 0);
	colors[0x1F] = gf::math::vec3(0, 0, 0);

	colors[0x20] = gf::math::vec3(236, 238, 236);
	colors[0x21] = gf::math::vec3(76, 154, 236);
	colors[0x22] = gf::math::vec3(120, 124, 236);
	colors[0x23] = gf::math::vec3(176, 98, 236);
	colors[0x24] = gf::math::vec3(228, 84, 236);
	colors[0x25] = gf::math::vec3(236, 88, 180);
	colors[0x26] = gf::math::vec3(236, 106, 100);
	colors[0x27] = gf::math::vec3(212, 136, 32);
	colors[0x28] = gf::math::vec3(160, 170, 0);
	colors[0x29] = gf::math::vec3(116, 196, 0);
	colors[0x2A] = gf::math::vec3(76, 208, 32);
	colors[0x2B] = gf::math::vec3(56, 204, 108);
	colors[0x2C] = gf::math::vec3(56, 180, 204);
	colors[0x2D] = gf::math::vec3(60, 60, 60);
	colors[0x2E] = gf::math::vec3(0, 0, 0);
	colors[0x2F] = gf::math::vec3(0, 0, 0);

	colors[0x30] = gf::math::vec3(236, 238, 236);
	colors[0x31] = gf::math::vec3(168, 204, 236);
	colors[0x32] = gf::math::vec3(188, 188, 236);
	colors[0x33] = gf::math::vec3(212, 178, 236);
	colors[0x34] = gf::math::vec3(236, 174, 236);
	colors[0x35] = gf::math::vec3(236, 174, 212);
	colors[0x36] = gf::math::vec3(236, 180, 176);
	colors[0x37] = gf::math::vec3(228, 196, 144);
	colors[0x38] = gf::math::vec3(204, 210, 120);
	colors[0x39] = gf::math::vec3(180, 222, 120);
	colors[0x3A] = gf::math::vec3(168, 226, 144);
	colors[0x3B] = gf::math::vec3(152, 226, 180);
	colors[0x3C] = gf::math::vec3(160, 214, 228);
	colors[0x3D] = gf::math::vec3(160, 162, 160);
	colors[0x3E] = gf::math::vec3(0, 0, 0);
	colors[0x3F] = gf::math::vec3(0, 0, 0);

  for (auto& color : colors){
    color /= 255;
  }
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
