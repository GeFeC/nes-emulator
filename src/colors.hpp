#pragma once

#include "renderer/math.hpp"
#include <array>

namespace nes{

inline auto get_colors(){
  auto colors = std::array<gf::math::vec3, 64>();

  colors[0x00] = gf::math::vec3(84, 84, 84);
	colors[0x01] = gf::math::vec3(0, 30, 116);
	colors[0x02] = gf::math::vec3(8, 16, 144);
	colors[0x03] = gf::math::vec3(48, 0, 136);
	colors[0x04] = gf::math::vec3(68, 0, 100);
	colors[0x05] = gf::math::vec3(92, 0, 48);
	colors[0x06] = gf::math::vec3(84, 4, 0);
	colors[0x07] = gf::math::vec3(60, 24, 0);
	colors[0x08] = gf::math::vec3(32, 42, 0);
	colors[0x09] = gf::math::vec3(8, 58, 0);
	colors[0x0A] = gf::math::vec3(0, 64, 0);
	colors[0x0B] = gf::math::vec3(0, 60, 0);
	colors[0x0C] = gf::math::vec3(0, 50, 60);
	colors[0x0D] = gf::math::vec3(0, 0, 0);
	colors[0x0E] = gf::math::vec3(0, 0, 0);
	colors[0x0F] = gf::math::vec3(0, 0, 0);

	colors[0x10] = gf::math::vec3(152, 150, 152);
	colors[0x11] = gf::math::vec3(8, 76, 196);
	colors[0x12] = gf::math::vec3(48, 50, 236);
	colors[0x13] = gf::math::vec3(92, 30, 228);
	colors[0x14] = gf::math::vec3(136, 20, 176);
	colors[0x15] = gf::math::vec3(160, 20, 100);
	colors[0x16] = gf::math::vec3(152, 34, 32);
	colors[0x17] = gf::math::vec3(120, 60, 0);
	colors[0x18] = gf::math::vec3(84, 90, 0);
	colors[0x19] = gf::math::vec3(40, 114, 0);
	colors[0x1A] = gf::math::vec3(8, 124, 0);
	colors[0x1B] = gf::math::vec3(0, 118, 40);
	colors[0x1C] = gf::math::vec3(0, 102, 120);
	colors[0x1D] = gf::math::vec3(0, 0, 0);
	colors[0x1E] = gf::math::vec3(0, 0, 0);
	colors[0x1F] = gf::math::vec3(0, 0, 0);

	colors[0x20] = gf::math::vec3(236, 238, 236);
	colors[0x21] = gf::math::vec3(76, 154, 236);
	colors[0x22] = gf::math::vec3(120, 124, 236);
	colors[0x23] = gf::math::vec3(176, 98, 236);
	colors[0x24] = gf::math::vec3(228, 84, 236);
	colors[0x25] = gf::math::vec3(236, 88, 180);
	colors[0x26] = gf::math::vec3(236, 106, 100);
	colors[0x27] = gf::math::vec3(212, 136, 32);
	colors[0x28] = gf::math::vec3(160, 170, 0);
	colors[0x29] = gf::math::vec3(116, 196, 0);
	colors[0x2A] = gf::math::vec3(76, 208, 32);
	colors[0x2B] = gf::math::vec3(56, 204, 108);
	colors[0x2C] = gf::math::vec3(56, 180, 204);
	colors[0x2D] = gf::math::vec3(60, 60, 60);
	colors[0x2E] = gf::math::vec3(0, 0, 0);
	colors[0x2F] = gf::math::vec3(0, 0, 0);

	colors[0x30] = gf::math::vec3(236, 238, 236);
	colors[0x31] = gf::math::vec3(168, 204, 236);
	colors[0x32] = gf::math::vec3(188, 188, 236);
	colors[0x33] = gf::math::vec3(212, 178, 236);
	colors[0x34] = gf::math::vec3(236, 174, 236);
	colors[0x35] = gf::math::vec3(236, 174, 212);
	colors[0x36] = gf::math::vec3(236, 180, 176);
	colors[0x37] = gf::math::vec3(228, 196, 144);
	colors[0x38] = gf::math::vec3(204, 210, 120);
	colors[0x39] = gf::math::vec3(180, 222, 120);
	colors[0x3A] = gf::math::vec3(168, 226, 144);
	colors[0x3B] = gf::math::vec3(152, 226, 180);
	colors[0x3C] = gf::math::vec3(160, 214, 228);
	colors[0x3D] = gf::math::vec3(160, 162, 160);
	colors[0x3E] = gf::math::vec3(0, 0, 0);
	colors[0x3F] = gf::math::vec3(0, 0, 0);

  for (auto& color : colors){
    color /= 255;
  }

  return colors;
}

} //namespace nes
