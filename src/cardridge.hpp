#pragma once

#include "mappers.hpp"
#include "aliases.hpp"
#include "util.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <optional>

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

  auto mirroring() const{
    if (mapper->mirroring() == Mapper::Mirroring::Hardware){
      return (header.mapper1 & 0x01) 
        ? Mapper::Mirroring::Vertical 
        : Mapper::Mirroring::Horizontal;
    }

    return mapper->mirroring();
  }

  auto get_game_name(std::string filepath){
    //Remove extension:
    filepath.pop_back();
    filepath.pop_back();
    filepath.pop_back();
    filepath.pop_back();

    const auto slash_pos = filepath.rfind('/');

    if (slash_pos == std::string::npos){
      return filepath;
    }
    return filepath.substr(slash_pos);
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

      if (header.char_rom_chunks == 0){
        char_memory.resize(8_kb, 0);
      }
      else{
        char_memory.resize(header.char_rom_chunks * 8_kb);
        file.read(reinterpret_cast<char*>(char_memory.data()), char_memory.size()); 
      }
    }
    else{
      throw std::runtime_error("Unsupported file type: " + std::to_string(file_type));
    }

    const auto game_name = get_game_name(filepath);
    switch(mapper_id()){
      case 0: 
        mapper = std::make_unique<Mapper000>(header.program_rom_chunks); break;
      case 1: 
        mapper = std::make_unique<Mapper001>(game_name, header.program_rom_chunks, header.char_rom_chunks); break;
      case 2:
        mapper = std::make_unique<Mapper002>(header.program_rom_chunks, header.char_rom_chunks); break;
      case 3:
        mapper = std::make_unique<Mapper003>(header.program_rom_chunks, header.char_rom_chunks); break;
      case 4:
        mapper = std::make_unique<Mapper004>(game_name, header.program_rom_chunks); break;
      case 66:
        mapper = std::make_unique<Mapper066>(header.program_rom_chunks, header.char_rom_chunks); break;
      default:
        throw std::runtime_error("Unsupported mapper: " + std::to_string(mapper_id()));
    }
  }

  auto cpu_write(u16 address, u8 value){
    const auto mapped = mapper->cpu_write(address, value);

    if (mapped.used_cartridge_ram()) return true;
    if (mapped.has_address()){
      program_memory[mapped.address] = value;
      return true;
    }

    return false;
  }

  auto cpu_read(u16 address) const -> std::optional<u8>{
    const auto mapped = mapper->cpu_read(address);

    if (mapped.used_cartridge_ram()){
      return mapped.data;
    }

    if (mapped.has_address()){
      return program_memory[mapped.address];
    }

    return std::nullopt;
  }

  auto ppu_write(u16 address, u8 value) -> bool{
    const auto mapped = mapper->ppu_write(address, value);

    if (mapped.has_address()){
      char_memory[mapped.address] = value;
      return true;
    }

    return false;
  }

  auto ppu_read(u16 address) const -> std::optional<u8>{
    const auto mapped = mapper->ppu_read(address);

    if (mapped.has_address()){
      return char_memory[mapped.address];
    }

    return std::nullopt;
  }
};

} //namespace nes
