#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include "renderer/renderer.hpp"
#include "nes.hpp"

auto main() -> int{
  if (!glfwInit()){
    throw std::runtime_error("Unable to initialise GLFW");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

  auto window = glfwCreateWindow(800, 600, "nes emulator", nullptr, nullptr);

  if (!window){
    throw std::runtime_error("Unable to create window");
  }
  
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
    throw std::runtime_error("Unable to initialise glad!");
  }

  auto renderer = nes::renderer::create_renderer(nes::Ppu::ScreenSize);
  glfwSetWindowSizeCallback(window, [](GLFWwindow*, int w, int h){
    glViewport(0, 0, w, h);
  });

  nes::Nes nes;
  nes.load_cardridge("nestest.nes");

  glfwShowWindow(window);

  while(!glfwWindowShouldClose(window)){
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

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
            nes::renderer::draw(renderer, nes::renderer::Pixel{
              gf::math::vec2(x * 8 + 7 - sprite_column, y * 8 + sprite_row),
              color.as_vec<4>(1.f)
            });
          }
        }
      }
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  nes::renderer::destroy_renderer(&renderer);
}
