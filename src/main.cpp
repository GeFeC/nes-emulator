#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "window.hpp"
#include "renderer/renderer.hpp"
#include "nes.hpp"
#include <iostream>

auto draw_sprite(
    const nes::Nes& nes, 
    nes::Renderer& renderer,
    const gf::math::vec2& position, 
    const gf::math::vec2& pattern_position
) -> void{
  const auto [x, y] = pattern_position;
  const auto offset = 256 * y + x * 16;

  for (auto sprite_row : gf::math::range(8)){
    auto lsb = nes.ppu_read(offset + sprite_row);
    auto msb = nes.ppu_read(offset + sprite_row + 8);

    for (auto sprite_column : gf::math::range(8)){
      const auto pixel = (lsb & 0x01) + (msb & 0x01);
      lsb >>= 1;
      msb >>= 1;

      const auto palette_color = nes.ppu_read(
        nes::Ppu::PalettesAddressRange.first + pixel
      );

      const auto color = nes.ppu.colors[palette_color];
      renderer.set_pixel(
        gf::math::vec2(position.x + 7 - sprite_column, position.y + sprite_row),
        color
      );
    }
  }
}


auto main() -> int{
  auto window = nes::Window("Nes emulator", gf::math::vec2(800, 600));
  window.on_resize([](auto, int w, int h){
    glViewport(0, 0, w, h);
  });

  nes::Nes nes;
  nes.load_cardridge("nestest.nes");
  auto renderer = nes::Renderer(nes::Ppu::ScreenSize);
  
  window.show();
  auto delta_time = 0.f;

  while(!window.should_close()){
    const auto start_frame_time = glfwGetTime();

    if (glfwGetKey(window.window, GLFW_KEY_ENTER)){
      std::cerr << 1.0 / delta_time << '\n';
    }

    window.clear_buffer();
    do{
      nes.clock();
    }while(!nes.frame_complete());

    draw_sprite(nes, renderer, gf::math::vec2(240.f, 232.f), gf::math::vec2(2, 3));

    for (auto y : gf::math::range(30)){
      for (auto x : gf::math::range(32)){
        const auto sprite_offset = nes.ppu_read(
          nes::Ppu::NametablesAddressRange.first + y * 32 + x
        );

        const auto sprite_x = sprite_offset % 16;
        const auto sprite_y = sprite_offset / 16;

        draw_sprite(
          nes,
          renderer,
          gf::math::vec2(x, y) * 8.f,
          gf::math::vec2(sprite_x, sprite_y)
        );
      }
    }

    renderer.render();
    window.use_vsync();
    window.update_buffer();

    delta_time = glfwGetTime() - start_frame_time;
  }
}
