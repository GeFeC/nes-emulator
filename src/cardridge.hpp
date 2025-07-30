#pragma once

#include "mappers.hpp"
#include "aliases.hpp"
#include "util.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <memory>

namespace nes{

struct CardridgeHeader{
  u8 name[4]; 
  u8 program_rom_chunks;
  u8 char_rom_chunks;
  u8 mapper1;
  u8 mapper2;
  u8 program_ram_size;
  u8 tv_system1;
  u8 tv_system2;
  u8 unused[5];
};

struct Cardridge{
  CardridgeHeader header;
  std::vector<u8> program_memory;
  std::vector<u8> char_memory;
  std::unique_ptr<Mapper> mapper;

  auto mapper_id() const{
    return (header.mapper2 & 0b11110000) | (header.mapper1 >> 4);
  }

  auto from_file(const std::string& filepath){
    auto file = file_open_for_reading(filepath, std::ios::binary);

    file.read(reinterpret_cast<char*>(&header), sizeof(CardridgeHeader));

    if (header.mapper1 & (1 << 2)){
      file.seekg(512, std::ios_base::cur);
    }

    const auto file_type = 1;

    if (file_type == 1){
      program_memory.resize(header.program_rom_chunks * 16_kb);
      file.read(reinterpret_cast<char*>(program_memory.data()), program_memory.size()); 

      char_memory.resize(header.char_rom_chunks * 8_kb);
      file.read(reinterpret_cast<char*>(char_memory.data()), char_memory.size()); 
    }

    switch(mapper_id()){
      case 0: mapper = std::make_unique<Mapper000>();
    }
  }

  auto write_program(u16 address, u8 value){
    const auto card_address = mapper->program_map(address, header.program_rom_chunks);

    if (card_address != std::nullopt){
      program_memory[card_address.value()] = value;
      return true;
    }

    return false;
  }

  auto read_program(u16 address) const -> std::optional<u8>{
    const auto card_address = mapper->program_map(address, header.program_rom_chunks);

    if (card_address != std::nullopt){
      return program_memory[card_address.value()];
    }

    return std::nullopt;
  }

  auto write_pattern(u16 address, u8 value) -> bool{
    const auto card_address = mapper->pattern_map(address);

    if (card_address != std::nullopt){
      char_memory[card_address.value()] = value;
      return true;
    }

    return false;
  }

  auto read_pattern(u16 address) const -> std::optional<u8>{
    const auto card_address = mapper->pattern_map(address);

    if (card_address != std::nullopt){
      return char_memory[card_address.value()];
    }

    return std::nullopt;
  }
};

} //namespace nes
