#pragma once

#include "aliases.hpp"
#include "renderer/math.hpp"
#include "renderer/renderer.hpp"
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

  Renderer renderer;

  u8 current_palette = 0;

  u8 nametables[2][32 * 32];
  u8 palettes[PalettesCount * PaletteSize];
  std::array<Renderer::pixel_color, 64> colors;

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

  struct LoopyRegisterProps{
    u16 scroll_x : 5;
    u16 scroll_y : 5;
    u16 nametable_x : 1;
    u16 nametable_y : 1;
    u16 cell_scroll_y : 3;
    u16 unused : 1;
  };

  union LoopyRegister{
    LoopyRegisterProps props;
    u16 data = 0;
  };

  //Registers:
  u8 control = 0;
  u8 mask = 0;
  u8 status = 0;

  AddressLatch address_latch = AddressLatch::MSB;
  u8 data_buffer = 0;

  LoopyRegister vram_address;
  LoopyRegister tram_address;
  u8 cell_scroll_x = 0;

  u8 bg_next_tile_id = 0;
  u8 bg_next_tile_attribute = 0;
  u8 bg_next_tile_lsb = 0;
  u8 bg_next_tile_msb = 0;

  u16 bg_shifter_pattern_low = 0;
  u16 bg_shifter_pattern_high = 0;
  u16 bg_shifter_attribute_low = 0;
  u16 bg_shifter_attribute_high = 0;

  i32 cycles = 0;
  i32 scanline = 0;
  bool frame_complete = false;
  bool palettes_started_loading = false;

  bool nmi = false;

  Ppu();
  auto mem_read(const Nes& nes, u16 address) const -> u8;
  auto mem_write(Nes& nes, u16 address, u8 value) -> void;
  auto cpu_read(const Nes& nes, u16 address) -> u8;
  auto cpu_write(Nes& nes, u16 address, u8 value) -> void;
  auto clock(const Nes& nes) -> void;

  auto set_loopy_reg(u16& reg, u16 data) -> void;
  auto get_loopy_reg(u16& reg) -> void;

  auto init_renderer() -> void;
};

} //namespace nes
