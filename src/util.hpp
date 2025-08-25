#pragma once

#include "aliases.hpp"
#include <utility>
#include <string>
#include <fstream>

namespace nes{

inline auto file_open_for_reading(const std::string& filepath, std::ios::openmode openmode = std::ios::in){
  auto file = std::ifstream(filepath, openmode); 

  if (!file){
    throw std::runtime_error("Unable to open file: " + filepath);
  }

  return std::move(file);
}

inline auto make_u16(u8 high, u8 low){
  return (high << 8) | low;
}

template<typename T>
using range_type = std::pair<T, T>;

inline auto in_range(i32 value, const range_type<i32>& range){
  return value >= range.first && value <= range.second;
}

inline auto operator""_kb(unsigned long long v){
  return v * 1024;
}

inline auto flip_byte(u8 b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

inline auto fast_sin(float t){
  auto j = t * 0.15915f;
  j = j - (int)j;
  return 20.785 * j * (j - 0.5f) * (j - 1.0f);
}

} //namespace nes
