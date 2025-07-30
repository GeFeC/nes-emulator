#pragma once

namespace nes::shaders{

static auto fragment_shader_script = R"(
  #version 330 core

  out vec4 out_color;
  in vec3 f_color;

  void main(){
    out_color = vec4(f_color, 1.0);
  }
)";

} //namespace nes::shaders
