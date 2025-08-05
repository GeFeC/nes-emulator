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

  static constexpr auto CpuAddressRange = std::make_pair(0x2000, 0x3FFF);

  static constexpr auto PaletteSize = 4;
  static constexpr auto PalettesCount = 8;

  static constexpr auto PalettesAddressRange = std::make_pair(0x3F00, 0x3F1F);
  static constexpr auto NametablesAddressRange = std::make_pair(0x2000, 0x3EFF);

  static constexpr auto TopLeftNametableAddressRange = std::make_pair(0x0000, 0x03FF);
  static constexpr auto TopRightNametableAddressRange = std::make_pair(0x0400, 0x07FF);
  static constexpr auto BottomLeftNametableAddressRange = std::make_pair(0x0800, 0x0BFF);
  static constexpr auto BottomRightNametableAddressRange = std::make_pair(0x0C00, 0x0FFF);

  u8 current_palette = 0;

  u8 nametables[2][32 * 32];
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

  struct Scroll{
    u8 offset_x;
    u8 offset_y;
    u8 cell_offset_x;
    u8 cell_offset_y;
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
  i32 scanline = 0;
  bool frame_complete = false;
  bool palettes_started_loading = false;

  bool nmi = false;

  Ppu();
  auto mem_read(const Nes& nes, u16 address) const -> u8;
  auto mem_write(Nes& nes, u16 address, u8 value) -> void;
  auto cpu_read(const Nes& nes, u16 address) -> u8;
  auto cpu_write(Nes& nes, u16 address, u8 value) -> void;
  auto clock() -> void;
};

} //namespace nes
