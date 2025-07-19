#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <iostream>
#include "renderer/renderer.hpp"

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

  glViewport(0, 0, 256, 240);

  auto renderer = nes::renderer::create_renderer(gf::math::vec2(256, 240));
  glfwSetWindowSizeCallback(window, [](GLFWwindow*, int w, int h){
    glViewport(0, 0, w, h);
  });

  glfwShowWindow(window);

  while(!glfwWindowShouldClose(window)){
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    auto s = clock();
    for (int x = 0; x < 256; ++x){
      for (int y = 0; y < 240; ++y){
        const auto rnd = []{ return rand() % 100 / 100.f; };
        nes::renderer::draw(renderer, nes::renderer::Pixel{ gf::math::vec2(x, y), gf::math::vec4(rnd(), rnd(), rnd(), 1.f) });
      }
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  nes::renderer::destroy_renderer(&renderer);
}
