#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "window.hpp"
#include "renderer/renderer.hpp"
#include "nes.hpp"

auto main() -> int{
  auto window = nes::Window("Nes emulator", gf::math::vec2(800, 600));
  window.on_resize([](auto, int w, int h){
    glViewport(0, 0, w, h);
  });

  nes::Nes nes;
  nes.load_cardridge("nestest.nes");
  auto renderer = nes::Renderer(nes::Ppu::ScreenSize);

  window.show();

  while(!window.should_close()){
    window.clear_buffer();
    do{
      nes.clock();
    }while(!nes.frame_complete());

    for (auto y : gf::math::range(16)){
      for (auto x : gf::math::range(16)){
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
            renderer.draw(nes::Renderer::Pixel{
              gf::math::vec2(x * 8 + 7 - sprite_column, y * 8 + sprite_row),
              color.as_vec<4>(1.f)
            });
          }
        }
      }
    }

    window.update_buffer();
  }
}
