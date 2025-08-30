#pragma once

#include "window.hpp"
#include "nes.hpp"

namespace nes{

struct Debugger{
  static constexpr auto Size = Ppu::ScreenSize;

  Texture texture;
  std::string opcode_names[256] = {
    /* 00 */ "BRK",   "ORA",   "*KIL",  "*SLO",  "NOP",   "ORA",   "ASL",   "*SLO",
    /* 08 */ "PHP",   "ORA",   "ASL",   "*ANC",  "NOP",   "ORA",   "ASL",   "*SLO",
    /* 10 */ "BPL",   "ORA",   "*KIL",  "*SLO",  "NOP",   "ORA",   "ASL",   "*SLO",
    /* 18 */ "CLC",   "ORA",   "NOP",   "*SLO",  "NOP",   "ORA",   "ASL",   "*SLO",
    /* 20 */ "JSR",   "AND",   "*KIL",  "*RLA",  "BIT",   "AND",   "ROL",   "*RLA",
    /* 28 */ "PLP",   "AND",   "ROL",   "*ANC",  "BIT",   "AND",   "ROL",   "*RLA",
    /* 30 */ "BMI",   "AND",   "*KIL",  "*RLA",  "NOP",   "AND",   "ROL",   "*RLA",
    /* 38 */ "SEC",   "AND",   "NOP",   "*RLA",  "NOP",   "AND",   "ROL",   "*RLA",
    /* 40 */ "RTI",   "EOR",   "*KIL",  "*SRE",  "NOP",   "EOR",   "LSR",   "*SRE",
    /* 48 */ "PHA",   "EOR",   "LSR",   "*ALR",  "JMP",   "EOR",   "LSR",   "*SRE",
    /* 50 */ "BVC",   "EOR",   "*KIL",  "*SRE",  "NOP",   "EOR",   "LSR",   "*SRE",
    /* 58 */ "CLI",   "EOR",   "NOP",   "*SRE",  "NOP",   "EOR",   "LSR",   "*SRE",
    /* 60 */ "RTS",   "ADC",   "*KIL",  "*RRA",  "NOP",   "ADC",   "ROR",   "*RRA",
    /* 68 */ "PLA",   "ADC",   "ROR",   "*ARR",  "JMP",   "ADC",   "ROR",   "*RRA",
    /* 70 */ "BVS",   "ADC",   "*KIL",  "*RRA",  "NOP",   "ADC",   "ROR",   "*RRA",
    /* 78 */ "SEI",   "ADC",   "NOP",   "*RRA",  "NOP",   "ADC",   "ROR",   "*RRA",
    /* 80 */ "NOP",   "STA",   "NOP",   "*SAX",  "STY",   "STA",   "STX",   "*SAX",
    /* 88 */ "DEY",   "NOP",   "TXA",   "*XAA",  "STY",   "STA",   "STX",   "*SAX",
    /* 90 */ "BCC",   "STA",   "*KIL",  "*AHX",  "STY",   "STA",   "STX",   "*SAX",
    /* 98 */ "TYA",   "STA",   "TXS",   "*TAS",  "*SHY",  "STA",   "*SHX",  "*AHX",
    /* A0 */ "LDY",   "LDA",   "LDX",   "*LAX",  "LDY",   "LDA",   "LDX",   "*LAX",
    /* A8 */ "TAY",   "LDA",   "TAX",   "*LAX",  "LDY",   "LDA",   "LDX",   "*LAX",
    /* B0 */ "BCS",   "LDA",   "*KIL",  "*LAX",  "LDY",   "LDA",   "LDX",   "*LAX",
    /* B8 */ "CLV",   "LDA",   "TSX",   "*LAS",  "LDY",   "LDA",   "LDX",   "*LAX",
    /* C0 */ "CPY",   "CMP",   "NOP",   "*DCP",  "CPY",   "CMP",   "DEC",   "*DCP",
    /* C8 */ "INY",   "CMP",   "DEX",   "*AXS",  "CPY",   "CMP",   "DEC",   "*DCP",
    /* D0 */ "BNE",   "CMP",   "*KIL",  "*DCP",  "NOP",   "CMP",   "DEC",   "*DCP",
    /* D8 */ "CLD",   "CMP",   "NOP",   "*DCP",  "NOP",   "CMP",   "DEC",   "*DCP",
    /* E0 */ "CPX",   "SBC",   "NOP",   "*ISC",  "CPX",   "SBC",   "INC",   "*ISC",
    /* E8 */ "INX",   "SBC",   "NOP",   "*SBC",  "CPX",   "SBC",   "INC",   "*ISC",
    /* F0 */ "BEQ",   "SBC",   "*KIL",  "*ISC",  "NOP",   "SBC",   "INC",   "*ISC",
    /* F8 */ "SED",   "SBC",   "NOP",   "*ISC",  "NOP",   "SBC",   "INC",   "*ISC",
  };

  enum class Page{
    Cpu,
    Ppu
  } page = Page::Cpu;

  Debugger() : texture(gf::math::vec2(Size)) {}

  auto render_pattern_table(Nes& nes, int index, const gf::math::vec2& position){
    for (auto [x, y] : gf::math::range({ 16, 16 })){
      const auto offset = 0x1000 * index + 256 * y + x * 16;
      for (auto sprite_row : gf::math::range(8)){
        auto lsb = nes.ppu_read(offset + sprite_row);
        auto msb = nes.ppu_read(offset + sprite_row + 8);

        for (auto sprite_column : gf::math::range(8)){
          const auto pixel = (lsb & 0x01) + ((msb & 0x01) << 1);
          lsb >>= 1;
          msb >>= 1;

          const auto palette_color = nes.ppu_read(
            nes::Ppu::PalettesAddressRange.first + pixel
          );

          const auto color = nes.ppu.colors[palette_color];
          texture.set_pixel(
            position + gf::math::vec2(x * 8 + 7 - sprite_column, y * 8 + sprite_row),
            color
          );
        }
      }
    }
  }

  auto render_palette_color(Nes& nes, const gf::math::vec2& position, const Texture::pixel_color& color){
    for (auto [x, y] : gf::math::range({ 8, 8 })){
      texture.set_pixel(position + gf::math::vec2(x, y), color);
    }
  }

  auto render_palettes(Nes& nes, const gf::math::vec2& position){
    for (auto [x, y] : gf::math::range({ 4, 8 })){
      const auto color_index = nes.ppu_read(
        nes::Ppu::PalettesAddressRange.first + x + y * 4
      );
      const auto color = nes.ppu.colors[color_index];

      const auto final_position = position + gf::math::vec2(x * 8.f, y * 9.f);
      render_palette_color(nes, final_position, color);
    }
  }

  auto get_instruction_length(Cpu::AddressMode address_mode){
    using AddressMode = Cpu::AddressMode;
    switch(address_mode){
      case AddressMode::Implied:
      case AddressMode::Accumulator: 
        return 1;

      case AddressMode::Immediate:
      case AddressMode::ZeroPage:
      case AddressMode::ZeroPageX:
      case AddressMode::ZeroPageY:
      case AddressMode::Relative:
      case AddressMode::XIndirect:
      case AddressMode::IndirectY:
        return 2;

      case AddressMode::Absolute:
      case AddressMode::AbsoluteX:
      case AddressMode::AbsoluteY:
      case AddressMode::Indirect:
        return 3;

      case AddressMode::None:
        return 0;
    }

    return 0;
  }

  auto render_instruction(Nes& nes, const gf::math::vec2& position, int address){
    auto opcode = nes.mem_read(address);

    const auto name = opcode_names[opcode];
    const auto address_mode = nes.cpu.instruction_lookup[opcode].address_mode;
    const auto size = get_instruction_length(address_mode);

    auto op_arg = std::string();
    auto op_str = hex_str(opcode);

    switch(size){
      case 2:
        op_arg = hex_str(nes.mem_read(address + 1));
        break;
      case 3:
        op_arg = hex_str(nes.mem_read_u16(address + 1));
        break;
    }

    auto arg_byte1 = std::string();
    auto arg_byte2 = std::string();
    if (op_arg.size() > 0) arg_byte1 = op_arg.substr(0, 2);
    if (op_arg.size() > 2) arg_byte2 = op_arg.substr(2, 2);

    using AddressMode = Cpu::AddressMode;
    switch(address_mode){
      default: 
        break;

      case AddressMode::Accumulator: 
        op_arg = "A"; break;

      case AddressMode::Immediate:
        op_arg = "#$" + op_arg; break;

      case AddressMode::ZeroPageX:
        op_arg = "$" + op_arg + " X"; break;

      case AddressMode::ZeroPageY:
        op_arg = "$" + op_arg + " Y"; break;

      case AddressMode::ZeroPage:
        op_arg = "$" + op_arg; break;

      case AddressMode::Relative:
        op_arg = "$" + op_arg; break;

      case AddressMode::XIndirect:
        op_arg = "*$" + op_arg + " X"; break;

      case AddressMode::IndirectY:
        op_arg = "*$" + op_arg + " Y"; break;

      case AddressMode::Absolute:
        op_arg = "$" + op_arg; break;

      case AddressMode::AbsoluteX:
        op_arg = "$" + op_arg + " X"; break;
        
      case AddressMode::AbsoluteY:
        op_arg = "$" + op_arg + " Y"; break;

      case AddressMode::Indirect:
        op_arg = "*$" + op_arg; break;

      case AddressMode::None:
        break;
    }

    auto str = name + "              ";
    str.insert(5, op_arg);
    texture.print(position, hex_str(u16(address)) + ":" + str);

    texture.print(
      position + gf::math::vec2(160.f, 0.f), 
      op_str + " " + arg_byte1 + " " + arg_byte2
    );

    return size;
  }

  auto render_cpu_page(Nes& nes){
    //Render code:
    const auto code_pos = gf::math::vec2(0.f, 0.f);
    auto code_height = 160.f;
    auto instruction_y = 0.f;

    auto i = 0;
    texture.text_color = Texture::pixel_color(255, 255, 0);
    const auto current_pc = nes.cpu.pc;

    while(instruction_y < code_pos.y + code_height){
      const auto op_size = render_instruction(
        nes, 
        code_pos + gf::math::vec2(0.f, instruction_y), 
        current_pc + i
      );
      
      instruction_y += 8.f;
      i += op_size;

      texture.text_color = Texture::pixel_color(255, 255, 255);
    }

    const auto registers_pos = gf::math::vec2(0.f, code_height + 8.f);

    //Render registers:
    texture.print(registers_pos + gf::math::vec2(0.f, 0), "X:" + hex_str(nes.cpu.x));
    texture.print(registers_pos + gf::math::vec2(40.f, 0), "Y:" + hex_str(nes.cpu.y));
    texture.print(registers_pos + gf::math::vec2(80.f, 0), "A:" + hex_str(nes.cpu.accumulator));
    texture.print(registers_pos + gf::math::vec2(120.f, 0), "SP:" + hex_str(nes.cpu.sp));
    texture.print(registers_pos + gf::math::vec2(168.f, 0), "STATUS:" + hex_str(nes.cpu.status));
  }

  auto render_ppu_page(Nes& nes){
    auto pattern_tables_pos = gf::math::vec2(0.f);
    render_pattern_table(nes, 0, pattern_tables_pos);
    render_pattern_table(nes, 1, pattern_tables_pos + gf::math::vec2(128.f, 0.f));

    texture.print(gf::math::vec2(0.f, 128.f), "PALETTES");

    for (auto i : gf::math::range(8)){
      texture.print(gf::math::vec2(0.f, 136.f + i * 9.f), std::string(1, '0' + i));
    }

    render_palettes(nes, gf::math::vec2(10.f, 136.f));

    auto offset = 0.f;
    auto offset_step = 0.f;

    auto registers_pos = gf::math::vec2(80.f, pattern_tables_pos.y + 128.f);
    auto step = 8.f;
    texture.print(registers_pos + gf::math::vec2(0.f, step * 0), "PPU");
    texture.print(registers_pos + gf::math::vec2(0.f, step * 1), "STATUS:" + hex_str(nes.ppu.status));
    texture.print(registers_pos + gf::math::vec2(0.f, step * 2), "MASK:" + hex_str(nes.ppu.mask));
    texture.print(registers_pos + gf::math::vec2(0.f, step * 3), "CONTROL:" + hex_str(nes.ppu.control));
    texture.print(registers_pos + gf::math::vec2(0.f, step * 4), "SPRITE0:" + hex_str(nes.ppu.oam[0].id));
  }

  auto render(Nes& nes){
    texture.clear();
    if (page == Page::Cpu){
      render_cpu_page(nes);
    }
    else{
      render_ppu_page(nes);
    }

    texture.print(gf::math::vec2(0.f, 240.f - 8.f), "CYCLES:" + std::to_string(nes.cycles));
  }

  auto loop(const Window& window){
    if (window.is_key_down(GLFW_KEY_1)) page = Page::Cpu;
    if (window.is_key_down(GLFW_KEY_2)) page = Page::Ppu;
  }
};


} //namespace nes
