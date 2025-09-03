#pragma once

#include "aliases.hpp"
#include "renderer/renderer.hpp"
#include <array>

namespace nes{

struct Nes;

struct Ppu{
  static constexpr auto ScreenSize = vec2(256, 240);
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

  static constexpr auto OAMSize = 64;

  static constexpr auto CpuControlPort = 0x0000;
  static constexpr auto CpuMaskPort = 0x0001;
  static constexpr auto CpuStatusPort = 0x0002;
  static constexpr auto CpuOAMAddressPort = 0x0003;
  static constexpr auto CpuOAMDataPort = 0x0004;
  static constexpr auto CpuScrollPort = 0x0005;
  static constexpr auto CpuAddressPort = 0x0006;
  static constexpr auto CpuDataPort = 0x0007;

  static constexpr auto MaxSpritesOnScanline = 8;

  Texture screen_texture;
  Texture buffer_texture;

  u8 current_palette = 0;
  bool sprite0hit_occured = false;

  u8 nametables[2][32 * 32];
  u8 palettes[PalettesCount * PaletteSize];
  std::array<Texture::pixel_color, 64> colors;

  enum class Status{
    SpriteOverflow = (1 << 5),
    Sprite0Hit = (1 << 6),
    VBlank = (1 << 7)
  };

  enum class Mask{
    Greyscale = 1,
    RenderBackgroundLeft = (1 << 1),
    RenderSpritesLeft = (1 << 2),
    RenderBackground = (1 << 3),
    RenderSprites = (1 << 4),
    Red = (1 << 5),
    Green = (1 << 6),
    Blue = (1 << 7)
  };

  enum class Control{
    NametableX = 1,
    NametableY = (1 << 1),
    IncrementMode = (1 << 2),
    SpritePattern = (1 << 3),
    BackgroundPattern = (1 << 4),
    SpriteSize = (1 << 5),
    SlaveMode = (1 << 6),
    EnableNmi = (1 << 7)
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

  struct OAMEntry{
    u8 y;
    u8 id;
    u8 attribute;
    u8 x;
  } oam[OAMSize]{};

  OAMEntry sprites_on_scanline[MaxSpritesOnScanline]{};
  u8 scanline_sprites_count = 0;

  u8 oam_address = 0;

  u8 sprite_shifter_pattern_low[MaxSpritesOnScanline];
  u8 sprite_shifter_pattern_high[MaxSpritesOnScanline];

  //Registers:
  Register<u8, Control> control;
  Register<u8, Mask> mask;
  Register<u8, Status> status;

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

  bool sprite0hit_possible = false;
  bool sprite0_being_rendered = false;

  bool nmi = false;

  Ppu(bool visual_mode = true);
  auto mem_read(const Nes& nes, u16 address) const -> u8;
  auto mem_write(Nes& nes, u16 address, u8 value) -> void;
  auto cpu_read(const Nes& nes, u16 address) -> u8;
  auto cpu_write(Nes& nes, u16 address, u8 value) -> void;
  auto clock(const Nes& nes) -> void;

  auto set_loopy_reg(u16& reg, u16 data) -> void;
  auto get_loopy_reg(u16& reg) -> void;
};

} //namespace nes
