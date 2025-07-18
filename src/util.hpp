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

template<typename T>
using range_type = std::pair<T, T>;

inline auto in_range(u16 value, const range_type<u16>& range){
  return value >= range.first && value <= range.second;
}

inline auto operator""_kb(unsigned long long v){
  return v * 1024;
}

} //namespace nes
