#pragma once

#include "renderer/renderer.hpp"
#include <array>

namespace nes{

inline auto get_colors(){
  auto colors = std::array<Renderer::pixel_color, 64>();

  colors[0x00] = Renderer::pixel_color(84, 84, 84);
	colors[0x01] = Renderer::pixel_color(0, 30, 116);
	colors[0x02] = Renderer::pixel_color(8, 16, 144);
	colors[0x03] = Renderer::pixel_color(48, 0, 136);
	colors[0x04] = Renderer::pixel_color(68, 0, 100);
	colors[0x05] = Renderer::pixel_color(92, 0, 48);
	colors[0x06] = Renderer::pixel_color(84, 4, 0);
	colors[0x07] = Renderer::pixel_color(60, 24, 0);
	colors[0x08] = Renderer::pixel_color(32, 42, 0);
	colors[0x09] = Renderer::pixel_color(8, 58, 0);
	colors[0x0A] = Renderer::pixel_color(0, 64, 0);
	colors[0x0B] = Renderer::pixel_color(0, 60, 0);
	colors[0x0C] = Renderer::pixel_color(0, 50, 60);
	colors[0x0D] = Renderer::pixel_color(0, 0, 0);
	colors[0x0E] = Renderer::pixel_color(0, 0, 0);
	colors[0x0F] = Renderer::pixel_color(0, 0, 0);

	colors[0x10] = Renderer::pixel_color(152, 150, 152);
	colors[0x11] = Renderer::pixel_color(8, 76, 196);
	colors[0x12] = Renderer::pixel_color(48, 50, 236);
	colors[0x13] = Renderer::pixel_color(92, 30, 228);
	colors[0x14] = Renderer::pixel_color(136, 20, 176);
	colors[0x15] = Renderer::pixel_color(160, 20, 100);
	colors[0x16] = Renderer::pixel_color(152, 34, 32);
	colors[0x17] = Renderer::pixel_color(120, 60, 0);
	colors[0x18] = Renderer::pixel_color(84, 90, 0);
	colors[0x19] = Renderer::pixel_color(40, 114, 0);
	colors[0x1A] = Renderer::pixel_color(8, 124, 0);
	colors[0x1B] = Renderer::pixel_color(0, 118, 40);
	colors[0x1C] = Renderer::pixel_color(0, 102, 120);
	colors[0x1D] = Renderer::pixel_color(0, 0, 0);
	colors[0x1E] = Renderer::pixel_color(0, 0, 0);
	colors[0x1F] = Renderer::pixel_color(0, 0, 0);

	colors[0x20] = Renderer::pixel_color(236, 238, 236);
	colors[0x21] = Renderer::pixel_color(76, 154, 236);
	colors[0x22] = Renderer::pixel_color(120, 124, 236);
	colors[0x23] = Renderer::pixel_color(176, 98, 236);
	colors[0x24] = Renderer::pixel_color(228, 84, 236);
	colors[0x25] = Renderer::pixel_color(236, 88, 180);
	colors[0x26] = Renderer::pixel_color(236, 106, 100);
	colors[0x27] = Renderer::pixel_color(212, 136, 32);
	colors[0x28] = Renderer::pixel_color(160, 170, 0);
	colors[0x29] = Renderer::pixel_color(116, 196, 0);
	colors[0x2A] = Renderer::pixel_color(76, 208, 32);
	colors[0x2B] = Renderer::pixel_color(56, 204, 108);
	colors[0x2C] = Renderer::pixel_color(56, 180, 204);
	colors[0x2D] = Renderer::pixel_color(60, 60, 60);
	colors[0x2E] = Renderer::pixel_color(0, 0, 0);
	colors[0x2F] = Renderer::pixel_color(0, 0, 0);

	colors[0x30] = Renderer::pixel_color(236, 238, 236);
	colors[0x31] = Renderer::pixel_color(168, 204, 236);
	colors[0x32] = Renderer::pixel_color(188, 188, 236);
	colors[0x33] = Renderer::pixel_color(212, 178, 236);
	colors[0x34] = Renderer::pixel_color(236, 174, 236);
	colors[0x35] = Renderer::pixel_color(236, 174, 212);
	colors[0x36] = Renderer::pixel_color(236, 180, 176);
	colors[0x37] = Renderer::pixel_color(228, 196, 144);
	colors[0x38] = Renderer::pixel_color(204, 210, 120);
	colors[0x39] = Renderer::pixel_color(180, 222, 120);
	colors[0x3A] = Renderer::pixel_color(168, 226, 144);
	colors[0x3B] = Renderer::pixel_color(152, 226, 180);
	colors[0x3C] = Renderer::pixel_color(160, 214, 228);
	colors[0x3D] = Renderer::pixel_color(160, 162, 160);
	colors[0x3E] = Renderer::pixel_color(0, 0, 0);
	colors[0x3F] = Renderer::pixel_color(0, 0, 0);

  return colors;
}

} //namespace nes
