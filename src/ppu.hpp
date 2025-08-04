#pragma once

#include "aliases.hpp"
#include "renderer/math.hpp"
#include <array>

namespace nes{

struct Nes;

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
  std::array<gf::math::vec3, 64> colors;

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

  enum class AddressLatch{
    LSB,
    MSB
  };

  //Registers:
  u8 control = 0;
  u8 mask = 0;
  u8 status = 0;

  AddressLatch address_latch = AddressLatch::MSB;
  u8 data_buffer = 0;
  u16 address = 0;

  u32 cycles = 0;
  u32 scanline = 0;
  bool frame_complete = false;
  bool palettes_started_loading = false;

  Ppu();
  auto mem_read(const Nes& nes, u16 address) const -> u8;
  auto mem_write(Nes& nes, u16 address, u8 value) -> void;
  auto cpu_read(const Nes& nes, u16 address) -> u8;
  auto cpu_write(Nes& nes, u16 address, u8 value) -> void;
  auto clock() -> void;
};

} //namespace nes
