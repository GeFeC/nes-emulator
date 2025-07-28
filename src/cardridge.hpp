#pragma once

#include "mappers.hpp"
#include "aliases.hpp"
#include "util.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>

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

  auto mapper_id() const{
    return (header.mapper2 & 0b11110000) | (header.mapper1 >> 4);
  }
};

inline auto cardridge_from_file(const std::string& filepath){
  Cardridge cardridge;
  auto file = file_open_for_reading(filepath, std::ios::binary);

  file.read(reinterpret_cast<char*>(&cardridge.header), sizeof(CardridgeHeader));

  if (cardridge.header.mapper1 & (1 << 2)){
    file.seekg(512, std::ios_base::cur);
  }

  const auto file_type = 1;

  auto& program_memory = cardridge.program_memory;
  auto& char_memory = cardridge.char_memory;

  if (file_type == 1){
    program_memory.resize(cardridge.header.program_rom_chunks * 16_kb);
    file.read(reinterpret_cast<char*>(program_memory.data()), program_memory.size()); 

    char_memory.resize(cardridge.header.char_rom_chunks * 8_kb);
    file.read(reinterpret_cast<char*>(char_memory.data()), char_memory.size()); 
  }

  return cardridge;
}

inline auto cardridge_cpu_map(const Cardridge& card, u16 address){
  switch(card.mapper_id()){
    case 0:
      return mapper000_cpu_map(address, card.header.program_rom_chunks);

    default:
      throw std::runtime_error("Unsupported mapper: " + std::to_string(card.mapper_id()));
  }
}

inline auto cardridge_ppu_map(const Cardridge& card, u16 address){
  switch(card.mapper_id()){
    case 0:
      return mapper000_ppu_map(address, card.header.program_rom_chunks);

    default:
      throw std::runtime_error("Unsupported mapper: " + std::to_string(card.mapper_id()));
  }
}

inline auto cardridge_cpu_write(Cardridge* cardridge, u16 address, u8 value){
  const auto card_address = cardridge_cpu_map(*cardridge, address);
  if (card_address != std::nullopt){
    cardridge->program_memory[card_address.value()] = value;
    return true;
  }

  return false;
}

inline auto cardridge_cpu_read(const Cardridge& cardridge, u16 address) -> std::optional<u8>{
  const auto card_address = cardridge_cpu_map(cardridge, address);
  if (card_address != std::nullopt){
    return cardridge.program_memory[card_address.value()];
  }

  return std::nullopt;
}

inline auto cardridge_ppu_read(const Cardridge& cardridge, u16 address) -> std::optional<u8>{
  const auto card_address = cardridge_ppu_map(cardridge, address);
  if (card_address != std::nullopt){
    return cardridge.char_memory[card_address.value()];
  }

  return std::nullopt;
}

} //namespace nes
