#pragma once

namespace nes::shaders{

static auto fragment_shader_script = R"(
  #version 330 core

  out vec4 color;
  uniform sampler2D pixels;
  in vec2 f_texture_position;

  void main(){
    color = texture(pixels, f_texture_position);
  }
)";

} //namespace nes::shaders
