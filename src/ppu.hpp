#pragma once

#include "aliases.hpp"
#include "cardridge.hpp"
#include "util.hpp"
#include "renderer/math.hpp"

namespace nes{

struct Ppu{
  static constexpr auto ScreenSize = gf::math::vec2(256, 240);
  static constexpr auto CyclesPerScanline = 341;
  static constexpr auto MaxScanlines = 261;

  static constexpr auto TileSize = 8;
  static constexpr auto TileGridsCount = 2;
  static constexpr auto SpritesGridSize = 16;
  static constexpr auto PatternTableSize = TileSize * TileGridsCount * SpritesGridSize * SpritesGridSize;

  static constexpr auto LeftPatternTableAddressRange = std::make_pair(0x0000, 0x0FFF);
  static constexpr auto RightPatternTableAddressRange = std::make_pair(0x1000, 0x1FFF);
  static constexpr auto CpuAddressRange = std::make_pair(0x2000, 0x3FFF);

  static constexpr auto PaletteSize = 4;
  static constexpr auto PalettesCount = 8;

  static constexpr auto PalettesAddressRange = std::make_pair(0x3F00, 0x3F1F);

  u8 left_pattern_table[PatternTableSize];
  u8 right_pattern_table[PatternTableSize];
  u8 current_palette = 0;

  u8 palettes[PalettesCount * PaletteSize];
  gf::math::vec3 colors[64];

  //Registers:

  struct Status{
    enum{
      SpriteOverflow = (1 << 5),
      Sprite0Hit = (1 << 6),
      VBlank = (1 << 7)
    };
  };

  struct Mask{
    enum{
      Greyscale = 1,
      RenderBackgroundLeft = (1 << 1),
      RenderSpritesLeft = (1 << 2),
      RenderBackground = (1 << 3),
      RenderSprites = (1 << 4),
      Red = (1 << 5),
      Green = (1 << 6),
      Blue = (1 << 7)
    };
  };

  struct Control{
    enum{
      NametableX = 1,
      NametableY = (1 << 1),
      IncrementMode = (1 << 2),
      SpritePattern = (1 << 3),
      BackgroundPattern = (1 << 4),
      SpriteSize = (1 << 5),
      SlaveMode = (1 << 6),
      EnableNmi = (1 << 7)
    };
  };

  u8 control = 0;
  u8 mask = 0;
  u8 status = 0;

  enum class AddressLatch{
    LSB,
    MSB
  };

  AddressLatch address_latch = AddressLatch::MSB;
  u8 data_buffer = 0;
  u16 address = 0;

  u32 cycles = 0;
  u32 scanline = 0;
  bool frame_complete = false;

  Ppu();
};

inline Ppu::Ppu(){
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

inline auto ppu_read(Ppu* ppu, const Cardridge& cardridge, u16 address) -> u8{
  const auto cardridge_data = cardridge_ppu_read(cardridge, address);
  if (cardridge_data != std::nullopt){
    return cardridge_data.value(); 
  }
  else if (in_range(address, Ppu::LeftPatternTableAddressRange)){
    return ppu->left_pattern_table[address];
  }
  else if (in_range(address, Ppu::RightPatternTableAddressRange)){
    return ppu->right_pattern_table[address];
  }
  else if (in_range(address, Ppu::PalettesAddressRange)){
    address &= 0x001F; //address %= 32
    if (address > 16) address -= 16;

    return ppu->palettes[address];
  }

  return 0x00;
}

inline auto ppu_write(Ppu* ppu, const Cardridge& cardridge, u16 address, u8 value) -> u8{
  const auto card_address = cardridge_ppu_map(cardridge, address);
  if (card_address != std::nullopt){
  }
  else if (in_range(address, Ppu::LeftPatternTableAddressRange)){
    ppu->left_pattern_table[address] = value;
  }
  else if (in_range(address, Ppu::RightPatternTableAddressRange)){
    ppu->right_pattern_table[address] = value;
  }
  else if (in_range(address, Ppu::PalettesAddressRange)){
    address &= 0x001F; //address %= 32
    if (address > 16) address -= 16;

    ppu->palettes[address] = value;
  }

  return 0x00;
}

inline auto ppu_cpu_read(Ppu* ppu, const Cardridge& cardridge, u16 address) -> u8{
  address &= 0x0007;
  switch(address){
    case 0x0002:{
      const auto status = ppu->status & 0b11100000;
      ppu->status &= ~Ppu::Status::VBlank;
      ppu->address_latch = Ppu::AddressLatch::MSB;
      return status;
    }

    case 0x0007:{
      auto data = ppu->data_buffer;
      ppu->data_buffer = ppu_read(ppu, cardridge, ppu->address) | (ppu->data_buffer & 0x1F);
      ppu->address++;

      if (ppu->address >= Ppu::PalettesAddressRange.first){
        data = ppu->data_buffer;
      }

      return data;
    }
  }

  return 0x00;
}

inline auto ppu_cpu_write(Ppu* ppu, const Cardridge& cardridge, u16 address, u8 value){
  address &= 0x0007;
  switch(address){
    case 0x0000:
      ppu->control = value;
      break;

    case 0x0001:
      ppu->mask = value;
      break;

    case 0x0006: 
      if (ppu->address_latch == Ppu::AddressLatch::LSB){
        ppu->address = (ppu->address & 0xFF00) | value;
        ppu->address_latch = Ppu::AddressLatch::MSB;
      }
      else{
        ppu->address = (ppu->address & 0x00FF) | (value << 8);
        ppu->address_latch = Ppu::AddressLatch::LSB;
      }
    case 0x0007:
      ppu_write(ppu, cardridge, ppu->address, value);
      ppu->address++;
      break;
  }
}

inline auto ppu_clock(Ppu* ppu){
  ppu->frame_complete = false;

  ppu->cycles++;

  if (ppu->cycles > Ppu::CyclesPerScanline){
    ppu->cycles = 0;
    ppu->scanline++;
  }

  if (ppu->scanline > Ppu::ScreenSize.y && ppu->cycles == 0){
    ppu->status |= Ppu::Status::VBlank;
  }

  if (ppu->scanline > Ppu::MaxScanlines){
    ppu->scanline = -1;
    ppu->frame_complete = true;
  }
}

} //namespace nes
