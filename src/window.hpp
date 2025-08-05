#pragma once

#include "renderer/math.hpp"
#include <stdexcept>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

namespace nes{

namespace gfm = gf::math;

struct Window{
  GLFWwindow* window;

  Window(const std::string& title, const gfm::vec2& size){
    if (!glfwInit()){
      throw std::runtime_error("Unable to initialise GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    window = glfwCreateWindow(800, 600, "nes emulator", nullptr, nullptr);

    if (!window){
      throw std::runtime_error("Unable to create window");
    }
    
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
      throw std::runtime_error("Unable to initialise glad!");
    }
  }

  auto on_resize(GLFWwindowsizefun callback){
    glfwSetWindowSizeCallback(window, callback);
  }

  auto show(){
    glfwShowWindow(window);
  }

  auto should_close() const{
    return glfwWindowShouldClose(window);
  }

  auto clear_buffer(){
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  auto use_vsync(){
    glfwSwapInterval(1);
  }

  auto update_buffer(){
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
};

} //namespace nes
