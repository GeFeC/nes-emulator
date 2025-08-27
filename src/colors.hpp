#pragma once

#include "renderer/texture.hpp"
#include <array>

namespace nes{

inline auto get_colors(){
  auto colors = std::array<Texture::pixel_color, 64>();

  colors[0x00] = Texture::pixel_color(84, 84, 84);
	colors[0x01] = Texture::pixel_color(0, 30, 116);
	colors[0x02] = Texture::pixel_color(8, 16, 144);
	colors[0x03] = Texture::pixel_color(48, 0, 136);
	colors[0x04] = Texture::pixel_color(68, 0, 100);
	colors[0x05] = Texture::pixel_color(92, 0, 48);
	colors[0x06] = Texture::pixel_color(84, 4, 0);
	colors[0x07] = Texture::pixel_color(60, 24, 0);
	colors[0x08] = Texture::pixel_color(32, 42, 0);
	colors[0x09] = Texture::pixel_color(8, 58, 0);
	colors[0x0A] = Texture::pixel_color(0, 64, 0);
	colors[0x0B] = Texture::pixel_color(0, 60, 0);
	colors[0x0C] = Texture::pixel_color(0, 50, 60);
	colors[0x0D] = Texture::pixel_color(0, 0, 0);
	colors[0x0E] = Texture::pixel_color(0, 0, 0);
	colors[0x0F] = Texture::pixel_color(0, 0, 0);

	colors[0x10] = Texture::pixel_color(152, 150, 152);
	colors[0x11] = Texture::pixel_color(8, 76, 196);
	colors[0x12] = Texture::pixel_color(48, 50, 236);
	colors[0x13] = Texture::pixel_color(92, 30, 228);
	colors[0x14] = Texture::pixel_color(136, 20, 176);
	colors[0x15] = Texture::pixel_color(160, 20, 100);
	colors[0x16] = Texture::pixel_color(152, 34, 32);
	colors[0x17] = Texture::pixel_color(120, 60, 0);
	colors[0x18] = Texture::pixel_color(84, 90, 0);
	colors[0x19] = Texture::pixel_color(40, 114, 0);
	colors[0x1A] = Texture::pixel_color(8, 124, 0);
	colors[0x1B] = Texture::pixel_color(0, 118, 40);
	colors[0x1C] = Texture::pixel_color(0, 102, 120);
	colors[0x1D] = Texture::pixel_color(0, 0, 0);
	colors[0x1E] = Texture::pixel_color(0, 0, 0);
	colors[0x1F] = Texture::pixel_color(0, 0, 0);

	colors[0x20] = Texture::pixel_color(236, 238, 236);
	colors[0x21] = Texture::pixel_color(76, 154, 236);
	colors[0x22] = Texture::pixel_color(120, 124, 236);
	colors[0x23] = Texture::pixel_color(176, 98, 236);
	colors[0x24] = Texture::pixel_color(228, 84, 236);
	colors[0x25] = Texture::pixel_color(236, 88, 180);
	colors[0x26] = Texture::pixel_color(236, 106, 100);
	colors[0x27] = Texture::pixel_color(212, 136, 32);
	colors[0x28] = Texture::pixel_color(160, 170, 0);
	colors[0x29] = Texture::pixel_color(116, 196, 0);
	colors[0x2A] = Texture::pixel_color(76, 208, 32);
	colors[0x2B] = Texture::pixel_color(56, 204, 108);
	colors[0x2C] = Texture::pixel_color(56, 180, 204);
	colors[0x2D] = Texture::pixel_color(60, 60, 60);
	colors[0x2E] = Texture::pixel_color(0, 0, 0);
	colors[0x2F] = Texture::pixel_color(0, 0, 0);

	colors[0x30] = Texture::pixel_color(236, 238, 236);
	colors[0x31] = Texture::pixel_color(168, 204, 236);
	colors[0x32] = Texture::pixel_color(188, 188, 236);
	colors[0x33] = Texture::pixel_color(212, 178, 236);
	colors[0x34] = Texture::pixel_color(236, 174, 236);
	colors[0x35] = Texture::pixel_color(236, 174, 212);
	colors[0x36] = Texture::pixel_color(236, 180, 176);
	colors[0x37] = Texture::pixel_color(228, 196, 144);
	colors[0x38] = Texture::pixel_color(204, 210, 120);
	colors[0x39] = Texture::pixel_color(180, 222, 120);
	colors[0x3A] = Texture::pixel_color(168, 226, 144);
	colors[0x3B] = Texture::pixel_color(152, 226, 180);
	colors[0x3C] = Texture::pixel_color(160, 214, 228);
	colors[0x3D] = Texture::pixel_color(160, 162, 160);
	colors[0x3E] = Texture::pixel_color(0, 0, 0);
	colors[0x3F] = Texture::pixel_color(0, 0, 0);

  for (auto& c : colors){
    auto increase_brightness = [](auto& x){
      auto modified = x * 1.2f;

      modified = std::min(modified, 255.f);
      x = modified;
    };

    increase_brightness(c.r);
    increase_brightness(c.g);
    increase_brightness(c.b);
  }

  return colors;
}

} //namespace nes
