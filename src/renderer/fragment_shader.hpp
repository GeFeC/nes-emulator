#pragma once

namespace nes::shaders{

static auto fragment_shader_script = R"(
  #version 330 core

  uniform vec4 color;

  out vec4 out_color;

  void main(){
    out_color = color;
  }
)";

} //namespace nes::shaders
