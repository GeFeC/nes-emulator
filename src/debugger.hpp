#pragma once

#include "window.hpp"
#include "nes.hpp"

namespace nes{

struct Debugger{
  static constexpr auto Size = Ppu::ScreenSize;

  Texture texture;

  enum class Page{
    Cpu,
    Ppu
  } page = Page::Cpu;

  Debugger() : texture(gf::math::vec2(Size)) {}

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

  auto render_cpu_registers(Nes& nes, const gf::math::vec2& position){
  }

  auto render_ppu_registers(){
  }

  auto render_cpu_page(Nes& nes){
    auto registers_pos = gf::math::vec2(0.f, 0.f);
    auto step = 8.f;

    texture.print(registers_pos + gf::math::vec2(0.f, step * 0), "CPU");
    texture.print(registers_pos + gf::math::vec2(0.f, step * 1), "X:" + hex_str(nes.cpu.x));
    texture.print(registers_pos + gf::math::vec2(0.f, step * 2), "Y:" + hex_str(nes.cpu.y));
    texture.print(registers_pos + gf::math::vec2(0.f, step * 3), "A:" + hex_str(nes.cpu.accumulator));
    texture.print(registers_pos + gf::math::vec2(0.f, step * 4), "SP:" + hex_str(nes.cpu.sp));
    texture.print(registers_pos + gf::math::vec2(0.f, step * 5), "STATUS:" + hex_str(nes.cpu.status));
    texture.print(registers_pos + gf::math::vec2(0.f, step * 6), "PC:" + hex_str(nes.cpu.pc)); 
  }

  auto render_ppu_page(Nes& nes){
    auto pattern_tables_pos = gf::math::vec2(0.f);
    render_pattern_table(nes, 0, pattern_tables_pos);
    render_pattern_table(nes, 1, pattern_tables_pos + gf::math::vec2(128.f, 0.f));

    texture.print(gf::math::vec2(0.f, 128.f), "PALETTES");

    for (auto i : gf::math::range(8)){
      texture.print(gf::math::vec2(0.f, 136.f + i * 9.f), std::string(1, '0' + i));
    }

    render_palettes(nes, gf::math::vec2(10.f, 136.f));

    auto offset = 0.f;
    auto offset_step = 0.f;

    auto registers_pos = gf::math::vec2(80.f, pattern_tables_pos.y + 128.f);
    auto step = 8.f;
    texture.print(registers_pos + gf::math::vec2(0.f, step * 0), "PPU");
    texture.print(registers_pos + gf::math::vec2(0.f, step * 1), "STATUS:" + hex_str(nes.ppu.status));
    texture.print(registers_pos + gf::math::vec2(0.f, step * 2), "MASK:" + hex_str(nes.ppu.mask));
    texture.print(registers_pos + gf::math::vec2(0.f, step * 3), "CONTROL:" + hex_str(nes.ppu.control));
    texture.print(registers_pos + gf::math::vec2(0.f, step * 4), "SPRITE0:" + hex_str(nes.ppu.oam[0].id));
  }

  auto render(Nes& nes){
    texture.clear();
    if (page == Page::Cpu){
      render_cpu_page(nes);
    }
    else{
      render_ppu_page(nes);
    }

    texture.print(gf::math::vec2(0.f, 240.f - 8.f), "CYCLES:" + std::to_string(nes.cycles));
  }

  auto loop(const Window& window){
    if (window.is_key_pressed(GLFW_KEY_1)) page = Page::Cpu;
    if (window.is_key_pressed(GLFW_KEY_2)) page = Page::Ppu;
  }
};


} //namespace nes
