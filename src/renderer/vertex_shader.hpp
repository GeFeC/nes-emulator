#pragma once

namespace nes::shaders{

inline auto vertex_shader_script = R"(
  #version 330 core

  layout (location = 0) in vec2 position;
  layout (location = 1) in vec2 offset;
  layout (location = 2) in vec3 color;

  uniform mat4 projection;
  uniform mat4 model;

  out vec3 f_color;

  void main(){
    gl_Position = projection * vec4(position + offset, 0.0, 1.0);
    f_color = color;
  }
)";

} //namespace nes::shaders 
