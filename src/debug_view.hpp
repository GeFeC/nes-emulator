#pragma once

#include "nes.hpp"

namespace nes{

struct DebugView{
  static constexpr auto Size = Ppu::ScreenSize;

  Texture texture;

  DebugView() : texture(gf::math::vec2(Size)) {}

  auto render_pattern_table(Nes& nes, int index, const gf::math::vec2& position){
    for (auto [x, y] : gf::math::range({ 16, 16 })){
      const auto offset = 0x1000 * index + 256 * y + x * 16;
      for (auto sprite_row : gf::math::range(8)){
        auto lsb = nes.ppu_read(offset + sprite_row);
        auto msb = nes.ppu_read(offset + sprite_row + 8);

        for (auto sprite_column : gf::math::range(8)){
          const auto pixel = (lsb & 0x01) + ((msb & 0x01) << 1);
          lsb >>= 1;
          msb >>= 1;

          const auto palette_color = nes.ppu_read(
            nes::Ppu::PalettesAddressRange.first + pixel
          );

          const auto color = nes.ppu.colors[palette_color];
          texture.set_pixel(
            position + gf::math::vec2(x * 8 + 7 - sprite_column, y * 8 + sprite_row),
            color
          );
        }
      }
    }
  }

  auto render_palette_color(Nes& nes, const gf::math::vec2& position, const Texture::pixel_color& color){
    for (auto [x, y] : gf::math::range({ 8, 8 })){
      texture.set_pixel(position + gf::math::vec2(x, y), color);
    }
  }

  auto render_palettes(Nes& nes, const gf::math::vec2& position){
    for (auto [x, y] : gf::math::range({ 4, 8 })){
      const auto color_index = nes.ppu_read(
        nes::Ppu::PalettesAddressRange.first + x + y * 4
      );
      const auto color = nes.ppu.colors[color_index];

      const auto final_position = position + gf::math::vec2(x * 8.f, y * 9.f);
      render_palette_color(nes, final_position, color);
    }
  }

  auto render_registers(Nes& nes, const gf::math::vec2& position){
    texture.print(position + gf::math::vec2(0.f, 0.f), "CPU");
    texture.print(position + gf::math::vec2(0.f, 8.f), "X:" + hex_str(nes.cpu.x));
    texture.print(position + gf::math::vec2(0.f, 16.f), "Y:" + hex_str(nes.cpu.y));
    texture.print(position + gf::math::vec2(0.f, 24.f), "A:" + hex_str(nes.cpu.accumulator));
    texture.print(position + gf::math::vec2(0.f, 32.f), "SP:" + hex_str(nes.cpu.sp));
    texture.print(position + gf::math::vec2(0.f, 40.f), "STATUS:" + hex_str(nes.cpu.status));
    texture.print(position + gf::math::vec2(0.f, 48.f), "PC:" + hex_str(nes.cpu.pc));

    texture.print(position + gf::math::vec2(80.f, 0.f), "PPU");
    texture.print(position + gf::math::vec2(80.f, 8.f), "STATUS:" + hex_str(nes.ppu.status));
    texture.print(position + gf::math::vec2(80.f, 16.f), "MASK:" + hex_str(nes.ppu.mask));
    texture.print(position + gf::math::vec2(80.f, 24.f), "CONTROL:" + hex_str(nes.ppu.control));
    texture.print(position + gf::math::vec2(80.f, 32.f), "SPRITE0:" + hex_str(nes.ppu.oam[0].id));
  }

  auto render(Nes& nes){
    render_pattern_table(nes, 0, gf::math::vec2(0.f));
    render_pattern_table(nes, 1, gf::math::vec2(Size.x / 2.f, 0.f));

    texture.print(gf::math::vec2(0.f, 128.f), "PALETTES");
    texture.print(gf::math::vec2(0.f, 208.f), "CYCLES:" + std::to_string(nes.cycles));

    for (auto i : gf::math::range(8)){
      texture.print(gf::math::vec2(0.f, 136.f + i * 9.f), std::string(1, '0' + i));
    }

    render_palettes(nes, gf::math::vec2(10.f, 136.f));
    render_registers(nes, gf::math::vec2(72.f, 128.f));
  }
};


} //namespace nes
