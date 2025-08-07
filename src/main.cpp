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
  nes.ppu.init_renderer();
  
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

    nes.ppu.renderer.render();
    window.use_vsync();
    window.update_buffer();

    delta_time = glfwGetTime() - start_frame_time;
  }

}
