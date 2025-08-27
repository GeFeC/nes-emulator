#pragma once

#include "math.hpp"
#include "../aliases.hpp"
#include "../util.hpp"
#include <glad/glad.h>

namespace nes{

struct Texture{
  using pixel_color = gf::math::vec<u8, 3>;

  std::vector<pixel_color> pixels;
  gf::math::vec2 size;
  GLuint id;

  Texture(const gf::math::vec2& size, bool visual_mode = true){
    if (!visual_mode) return;

    static_assert(sizeof(gf::math::vec<u8, 3>) == 3);

    this->size = size;
    pixels.resize(size.x * size.y);

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 240, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
  }

  auto set_pixel(const gf::math::vec2& position, const pixel_color& color){
    const auto [x, y] = position;

    if (!in_range(x, std::make_pair(0, size.x - 1))) return;
    if (!in_range(y, std::make_pair(0, size.y - 1))) return;

    pixels[y * size.x + x] = color;
  }
};

} //namespace nes
