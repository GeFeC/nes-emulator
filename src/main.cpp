#include <miniaudio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "window.hpp"
#include "renderer/renderer.hpp"
#include "nes.hpp"
#include <cassert>

auto main(int argc, char** argv) -> int{
  auto rom_path = std::string();
  if (argc < 2){
    std::cout << "Enter ROM path (without .nes extension): ";
    std::cin >> rom_path;
  }
  else{
    rom_path = argv[1];
  }

  auto window = nes::Window("Nes emulator", gf::math::vec2(800, 600));
  window.on_resize([](auto, int w, int h){
    static constexpr auto NesAspectRatio = nes::Ppu::ScreenSize.x / nes::Ppu::ScreenSize.y;
    const auto aspect_ratio = float(w) / h;
    if (aspect_ratio > NesAspectRatio){
      const auto view_w = h * NesAspectRatio;
      const auto center_x = w / 2.f - view_w / 2;
      glViewport(center_x, 0, view_w, h);
      return;
    }

    const auto view_h = w * NesAspectRatio;
    const auto center_y = h / 2.f - view_h / 2;
    glViewport(0, center_y, w, view_h);
  });

  nes::Nes nes;
  nes.load_cardridge(rom_path + ".nes");
  nes::Renderer renderer(nes::Ppu::ScreenSize);

  window.show();
  auto delta_time = 0.f;

  nes.apu.play([](nes::Nes& nes) -> float{
    static auto time = 0.0;

    while(!nes.clock()){
      time += 1.0 / nes::Nes::CyclesPerSec;
    }

    auto& apu = nes.apu;
    const auto pulse_out = 0.00752f * (apu.pulse1.output(time) + apu.pulse2.output(time));
    const auto noise_out = 0.00494f * apu.noise.output();
    const auto final_sample = pulse_out + noise_out;

    return std::clamp(final_sample, -1.f, 1.f);
  });

  while(!window.should_close()){
    const auto start_frame_time = glfwGetTime();

    nes.controllers[0] = 0;
    nes.controllers[0] |= (window.is_key_pressed(GLFW_KEY_Z) << 7);
    nes.controllers[0] |= (window.is_key_pressed(GLFW_KEY_X) << 6);
    nes.controllers[0] |= (window.is_key_pressed(GLFW_KEY_SPACE) << 5);
    nes.controllers[0] |= (window.is_key_pressed(GLFW_KEY_ENTER) << 4);
    nes.controllers[0] |= (window.is_key_pressed(GLFW_KEY_UP) << 3);
    nes.controllers[0] |= (window.is_key_pressed(GLFW_KEY_DOWN) << 2);
    nes.controllers[0] |= (window.is_key_pressed(GLFW_KEY_LEFT) << 1);
    nes.controllers[0] |= (window.is_key_pressed(GLFW_KEY_RIGHT) << 0);

    if (glfwGetKey(window.window, GLFW_KEY_TAB) == GLFW_PRESS){
      std::cerr << "FPS: " << 1.0 / delta_time << '\n';
    }

    window.clear_buffer();
    renderer.render_texture(nes.ppu.screen_texture, nes::gfm::vec2(0.f));
    window.swap_interval(1);
    window.update_buffer();
    delta_time = glfwGetTime() - start_frame_time;
  }

}
