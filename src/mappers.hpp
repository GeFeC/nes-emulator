#pragma once

#include "aliases.hpp"
#include "util.hpp"
#include <fstream>

namespace nes{

struct MapperResult{
  enum class Status{
    Mapped,
    NotMapped,
    UsedCartridgeRam
  };

  u32 address;
  u32 data;
  Status status;

  MapperResult() {}
  MapperResult(u32 address) : address(address), status(Status::Mapped) {}

  auto has_address() const{
    return status == Status::Mapped;
  }

  auto used_cartridge_ram() const{
    return status == Status::UsedCartridgeRam;
  }

  static auto not_mapped(){
    auto r = MapperResult();
    r.status = Status::NotMapped;

    return r;
  }

  static auto used_cartridge_ram(u32 data){
    auto r = MapperResult();
    r.status = Status::UsedCartridgeRam;
    r.data = data;

    return r;
  }
};

struct Mapper{
  enum class Mirroring{
    OneScreenLow,
    OneScreenHigh,
    Horizontal,
    Vertical,
    Hardware
  };

  virtual auto cpu_write(u16 address, u8 data) -> MapperResult = 0;
  virtual auto cpu_read(u16 address) -> MapperResult = 0;
  virtual auto ppu_write(u16 address, u8 data) -> MapperResult = 0;
  virtual auto ppu_read(u16 address) -> MapperResult = 0;
  virtual auto mirroring() -> Mirroring{
    return Mirroring::Hardware;
  }

  virtual auto irq_state() -> bool{
    return false;
  }

  virtual auto irq_clear() -> void{}

  virtual auto update_irq_counter(u16 address) -> void{}

  virtual auto current_program_bank() -> u16 = 0;
  virtual ~Mapper() {}
};

struct Mapper000 : Mapper{
  u8 program_banks;

  Mapper000(u8 program_banks) : program_banks(program_banks) {}

  auto cpu_read(u16 address) -> MapperResult override{
    if (in_range(address, { 0x8000, 0xFFFF })){
      return address & (program_banks > 1 ? 0x7FFF : 0x3FFF);
    }

    return MapperResult::not_mapped();
  }

  auto cpu_write(u16 address, u8 data) -> MapperResult override{
    //The same as read
    return cpu_read(address);
  }

  auto ppu_read(u16 address) -> MapperResult override{
    if (in_range(address, { 0x0000, 0x1FFF })){
      return address;
    }

    return MapperResult::not_mapped();
  }

  auto ppu_write(u16 address, u8 data) -> MapperResult override{
    //The same as read
    return ppu_read(address);
  }

  auto current_program_bank() -> u16 override{
    return 0;
  }
};

struct Mapper001 : Mapper{
  u8 control = 0;
  u8 program_banks = 0;
  u8 char_banks = 0;

  u8 selected_program_bank16_low = 0;
  u8 selected_program_bank16_high = 0;
  u8 selected_program_bank32 = 0;

  u8 selected_char_bank4_low = 0;
  u8 selected_char_bank4_high = 0;
  u8 selected_char_bank8 = 0;

  u8 shift_buffer = 0;
  u8 shift_buffer_size = 0;

  Mirroring mirroring_buffer = Mirroring::OneScreenHigh;

  std::vector<u8> static_ram;
  std::string game_name;

  Mapper001(const std::string& game_name, u8 program_banks, u8 char_banks) 
  : program_banks(program_banks), char_banks(char_banks), game_name(game_name) {
    static_ram.resize(32_kb, 0);

    auto file = std::ifstream(game_name + ".sav", std::ios::binary);
    if (!file) return;

    file.read(reinterpret_cast<char*>(static_ram.data()), static_ram.size());
  }

  auto current_program_bank() -> u16 override{
    const auto program_mode = (control >> 2) & 0x03;
    if (program_mode < 2){
      return selected_program_bank32;
    }
    else if (program_mode == 2){
      return selected_program_bank16_low;
    }
    else if (program_mode == 3){
      return selected_program_bank16_high;
    }
    return 0;
  }

  auto in_static_ram_range(u16 address){
    return in_range(address, { 0x6000, 0x7FFF });
  }

  auto cpu_read(u16 address) -> MapperResult override{
    if (address < 0x6000){
      return MapperResult::not_mapped();
    }

    if (in_static_ram_range(address)){
      return MapperResult::used_cartridge_ram(static_ram[address & 0x1FFF]);
    }

    if (control & 0b01000){
      //16Kb mode:
      if (in_range(address, { 0x8000, 0xBFFF })){
        return selected_program_bank16_low * 16_kb + (address & 0x3FFF);
      }

      if (in_range(address, { 0xC000, 0xFFFF })){
        return selected_program_bank16_high * 16_kb + (address & 0x3FFF);
      }

    }
    else{
      //32Kb mode:
      return selected_program_bank32 * 32_kb + (address & 0x7FFF);
    }

    return MapperResult::not_mapped();
  }

  auto cpu_write(u16 address, u8 data) -> MapperResult override{
    if (address < 0x6000){ 
      return MapperResult::not_mapped();
    }

    if (in_static_ram_range(address)){
      static_ram[address & 0x1FFF] = data;
      return MapperResult::used_cartridge_ram(0);
    }

    if (data & 0x80){
      shift_buffer = 0;
      shift_buffer_size = 0;
      control |= 0x0C;
    }
    else{
      shift_buffer >>= 1;
      shift_buffer |= (data & 0x01) << 4;
      shift_buffer_size++;

      if (shift_buffer_size < 5) return MapperResult::not_mapped();

      const auto target_register = (address >> 13) & 0x03;

      if (target_register == 0){
        control = shift_buffer & 0x1F;

        switch(control & 0x03){
          case 0: mirroring_buffer = Mirroring::OneScreenLow; break; 
          case 1: mirroring_buffer = Mirroring::OneScreenHigh; break; 
          case 2: mirroring_buffer = Mirroring::Vertical; break; 
          case 3: mirroring_buffer = Mirroring::Horizontal; break; 
        }
      }
      else if (target_register == 1){
        if (control & 0b10000){
          //4Kb mode:
          selected_char_bank4_low = shift_buffer & 0x1F;
        }
        else{
          //8Kb mode:
          selected_char_bank8 = shift_buffer & 0x1E;
        }
      }
      else if (target_register == 2){
        if (control & 0b10000){
          //4Kb mode:
          selected_char_bank4_high = shift_buffer & 0x1F;
        }
      }
      else if (target_register == 3){
        const auto program_mode = (control >> 2) & 0x03;

        if (program_mode < 2){
          selected_program_bank32 = (shift_buffer & 0x0E) >> 1;
        }
        else if (program_mode == 2){
          selected_program_bank16_low = 0;
          selected_program_bank16_high = shift_buffer & 0x0F;
        }
        else{
          selected_program_bank16_low = shift_buffer & 0x0F;
          selected_program_bank16_high = program_banks - 1;
        }
      }
      
      shift_buffer = 0;
      shift_buffer_size = 0;
    }

    return MapperResult::not_mapped();
  }

  auto ppu_read(u16 address) -> MapperResult override{
    if (address >= 0x2000) return MapperResult::not_mapped();

    if (char_banks == 0){
      return address;
    }

    if (control & 0b10000){
      if (in_range(address, { 0x0000, 0x0FFF })){
        return selected_char_bank4_low * 4_kb + (address & 0x0FFF);
      }
      if (in_range(address, { 0x1000, 0x1FFF })){
        return selected_char_bank4_high * 4_kb + (address & 0x0FFF);
      }
    }
    else{
      return selected_char_bank8 * 8_kb + (address & 0x1FFF);
    }

    return MapperResult::not_mapped();
  }

  auto ppu_write(u16 address, u8 data) -> MapperResult override{
    return ppu_read(address);
  }

  auto mirroring() -> Mirroring override{
    return mirroring_buffer;
  }

  ~Mapper001() override{
    auto file = std::ofstream(game_name + ".sav", std::ios::binary);
    file.write(reinterpret_cast<char*>(static_ram.data()), static_ram.size()); 
  }
};

struct Mapper002 : Mapper{
  u8 selected_program_bank = 0;
  u8 program_banks;
  u8 char_banks;

  Mapper002(u8 program_banks, u8 char_banks) 
  : program_banks(program_banks), char_banks(char_banks) {}

  auto cpu_read(u16 address) -> MapperResult override{
    if (in_range(address, { 0x8000, 0xBFFF })){
      return selected_program_bank * 16_kb + (address - 0x8000);
    }
    if (in_range(address, { 0xC000, 0xFFFF })){
      return (program_banks - 1) * 16_kb + (address - 0xC000);
    }

    return MapperResult::not_mapped();
  }

  auto cpu_write(u16 address, u8 data) -> MapperResult override{
    if (in_range(address, { 0x8000, 0xFFFF })){
      selected_program_bank = data & 0x0F;
    }
    return MapperResult::not_mapped();
  }

  auto ppu_read(u16 address) -> MapperResult override{
    if (in_range(address, { 0x0000, 0x1fff })){
      return (address);
    }

    return MapperResult::not_mapped();
  }

  auto ppu_write(u16 address, u8 data) -> MapperResult override{
    if (in_range(address, { 0x0000, 0x1fff })){
      if (char_banks == 0){
        return (address);
      }
    }

    return MapperResult::not_mapped();
  }

  auto current_program_bank() -> u16 override{
    return selected_program_bank;
  }
};

struct Mapper003 : Mapper{
  u8 selected_char_bank = 0;
  u8 program_banks;
  u8 char_banks;

  Mapper003(u8 program_banks, u8 char_banks) 
  : program_banks(program_banks), char_banks(char_banks) {
  }

  auto cpu_read(u16 address) -> MapperResult override{
    if (in_range(address, { 0x8000, 0xFFFF })){
      return address & (program_banks > 1 ? 0x7FFF : 0x3FFF);
    }

    return MapperResult::not_mapped();
  }

  auto cpu_write(u16 address, u8 data) -> MapperResult override{
    if (in_range(address, { 0x8000, 0xFFFF })){
      selected_char_bank = data & 0x03;
      return address;
    }

    return MapperResult::not_mapped();
  }

  auto ppu_read(u16 address) -> MapperResult override{
    if (in_range(address, { 0x0000, 0x1fff })){
      return selected_char_bank * 8_kb + address;
    }

    return MapperResult::not_mapped();
  }

  auto ppu_write(u16 address, u8 data) -> MapperResult override{
    return MapperResult::not_mapped();
  }

  auto current_program_bank() -> u16 override{
    return 0;
  }
};

struct Mapper004 : Mapper{
  std::vector<u8> static_ram;
  std::string game_name;
  u32 registers[8]{};
  u32 char_banks[8]{};
  u32 program_banks[4]{};

  Mirroring mirroring_buffer = Mirroring::Horizontal;

  u16 irq_counter = 0;
  u16 irq_reload = 0;

  u8 program_banks_count = 0;

  u8 current_program_bank_buffer = 0;

  u8 target_register = 0;
  bool program_bank_mode = 0;
  bool char_bank_mode = 0;

  bool irq_enabled = false;
  bool irq_active = false;


  Mapper004(const std::string& game_name, u8 program_banks) : game_name(game_name), program_banks_count(program_banks){
    static_ram.resize(32_kb);

    this->program_banks[0] = 0 * 0x2000;
    this->program_banks[1] = 1 * 0x2000;
    this->program_banks[2] = (program_banks_count * 2 - 2) * 0x2000;
    this->program_banks[3] = (program_banks_count * 2 - 1) * 0x2000;

    auto file = std::ifstream(game_name + ".sav", std::ios::binary);
    if (!file) return;

    file.read(reinterpret_cast<char*>(static_ram.data()), static_ram.size());
  }

  auto in_static_ram_range(u16 address){
    return in_range(address, { 0x6000, 0x7FFF });
  }

  auto cpu_read(u16 address) -> MapperResult override{
    if (address < 0x6000){
      return MapperResult::not_mapped();
    }

    if (in_static_ram_range(address)){
      return MapperResult::used_cartridge_ram(static_ram[address & 0x1FFF]);
    }

    if (address >= 0xE000) current_program_bank_buffer = program_banks_count - 1;

    else if (address >= 0xC000) {
        if (program_bank_mode == 0) current_program_bank_buffer = program_banks_count - 2;
        else current_program_bank_buffer = registers[6];
    }

    else if (address >= 0xA000) current_program_bank_buffer = registers[7];

    else if (address >= 0x8000) {
        if (program_bank_mode == 0) current_program_bank_buffer = registers[6];
        else current_program_bank_buffer = program_banks_count - 2;
    }

    const auto bank_index = (address - 0x8000) / 0x2000;
    return program_banks[bank_index] + (address & 0x1FFF);
  }

  auto cpu_write(u16 address, u8 data) -> MapperResult override{
    if (in_static_ram_range(address)){
      static_ram[address & 0x1FFF] = data;
      return MapperResult::used_cartridge_ram(0);
    }

    if (in_range(address, { 0x8000, 0x9FFF })){
      if (is_even(address)){
        target_register = data & 0x07;
        program_bank_mode = data & 0x40;
        char_bank_mode = data & 0x80;

        return MapperResult::not_mapped();
      }

      registers[target_register] = data;

      if (char_bank_mode){
        char_banks[0] = registers[2];
        char_banks[1] = registers[3];
        char_banks[2] = registers[4];
        char_banks[3] = registers[5];
        char_banks[4] = registers[0] & 0xFE;
        char_banks[5] = registers[0] + 1;
        char_banks[6] = registers[1] & 0xFE;
        char_banks[7] = registers[1] + 1;
      }
      else{
        char_banks[4] = registers[2];
        char_banks[5] = registers[3];
        char_banks[6] = registers[4];
        char_banks[7] = registers[5];
        char_banks[0] = registers[0] & 0xFE;
        char_banks[1] = registers[0] + 1;
        char_banks[2] = registers[1] & 0xFE;
        char_banks[3] = registers[1] + 1;
      }

      for (auto& bank : char_banks){
        bank *= 0x0400;
      }

      auto bank_index1 = 0;
      auto bank_index2 = 2;

      if (program_bank_mode){
        std::swap(bank_index1, bank_index2);
      }

      program_banks[bank_index1] = (registers[6] & 0x3F) * 0x2000;
      program_banks[bank_index2] = (program_banks_count * 2 - 2) * 0x2000;

      program_banks[1] = (registers[7] & 0x3F) * 0x2000;
			program_banks[3] = (program_banks_count * 2 - 1) * 0x2000;

      return MapperResult::not_mapped();
    }

    if (in_range(address, { 0xA000, 0xBFFF })){
      if (!is_even(address)) return MapperResult::not_mapped();

      if (is_even(data)){
        mirroring_buffer = Mirroring::Vertical;
      }
      else{
        mirroring_buffer = Mirroring::Horizontal;
      }

      return MapperResult::not_mapped();
    }

    if (in_range(address, { 0xC000, 0xDFFF })){
      if (is_even(address)){
        irq_counter = 0;
      }
      else{
        irq_reload = data;
      }

      return MapperResult::not_mapped();
    }

    if (in_range(address, { 0xE000, 0xFFFF })){
      if (is_even(address)){
        irq_enabled = false;
        irq_active = false;
      }
      else{
        irq_enabled = true;
      }

      return MapperResult::not_mapped();
    }

    return MapperResult::not_mapped();
  }

  auto ppu_read(u16 address) -> MapperResult override{
    if (address > 0x1FFF) return MapperResult::not_mapped();

    const auto bank_index = address / 0x400;

    return char_banks[bank_index] + (address & 0x03FF);
  }

  auto ppu_write(u16 address, u8 data) -> MapperResult override{
    return MapperResult::not_mapped();
  }

  auto mirroring() -> Mirroring override{
    return mirroring_buffer;
  }

  auto irq_state() -> bool override{
    return irq_active;
  }

  auto irq_clear() -> void override{
    irq_active = false;
  }

  auto update_irq_counter(u16 address) -> void override{
    if (irq_counter == 0) {
      irq_counter = irq_reload;
    } else {
      irq_counter--;
      if (irq_counter == 0 && irq_enabled) {
        irq_active = true;
      }
    }
  }

  auto current_program_bank() -> u16 override{
    return current_program_bank_buffer;
  }

  ~Mapper004(){
    auto file = std::ofstream(game_name + ".sav", std::ios::binary);
    file.write(reinterpret_cast<char*>(static_ram.data()), static_ram.size()); 
  }
};

struct Mapper066 : Mapper{
  u8 selected_program_bank = 0;
  u8 selected_char_bank = 0;
  u8 program_banks;
  u8 char_banks;

  Mapper066(u8 program_banks, u8 char_banks) 
  : program_banks(program_banks), char_banks(char_banks) {
  }

  auto cpu_read(u16 address) -> MapperResult override{
    if (in_range(address, { 0x8000, 0xFFFF })){
      return selected_program_bank * 32_kb + (address & 0x7FFF);
    }

    return MapperResult::not_mapped();
  }

  auto cpu_write(u16 address, u8 data) -> MapperResult override{
    if (in_range(address, { 0x8000, 0xFFFF })){
      selected_char_bank = data & 0x03;
      selected_program_bank = (data & 0x30) >> 4;
    }
    return MapperResult::not_mapped();
  }

  auto ppu_read(u16 address) -> MapperResult override{
    if (in_range(address, { 0x0000, 0x1fff })){
      return selected_char_bank * 8_kb + address;
    }

    return MapperResult::not_mapped();
  }

  auto ppu_write(u16 address, u8 data) -> MapperResult override{
    return MapperResult::not_mapped();
  }

  auto current_program_bank() -> u16 override{
    return selected_program_bank;
  }
};

} //namespace nes
