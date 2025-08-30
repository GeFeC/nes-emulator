#pragma once

#include <stdexcept>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include "aliases.hpp"

namespace nes{

struct Window{
  struct KeyState{
    bool released = true;
  };

  mutable KeyState key_states[512];

  GLFWwindow* window;

  Window(const std::string& title, const vec2& size){
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

  auto is_key_down(int key) const{
    return glfwGetKey(window, key) == GLFW_PRESS;
  }

  auto is_key_pressed(int key) const{
    if (is_key_down(key) && key_states[key].released){
      key_states[key].released = false;
      return true;
    }
    if (!is_key_down(key)){
      key_states[key].released = true;
    }

    return false;
  }

  auto clear_buffer(){
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  auto swap_interval(int interval){
    glfwSwapInterval(interval);
  }

  auto update_buffer(){
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ~Window(){
    glfwDestroyWindow(window);
  }
};

} //namespace nes
