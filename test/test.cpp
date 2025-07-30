#include <iostream>
#include "../src/nes.hpp"
#include <sstream>
#include <string>
#include <vector>

namespace nes{

struct LogData{
  std::string cmd, a, x, y, p, sp, pc;
  int cycles;
};

inline auto ichar_equals(char a, char b) { 
  return 
    std::tolower(static_cast<unsigned char>(a)) ==
    std::tolower(static_cast<unsigned char>(b));
}

inline auto hex(const std::string& str){
  std::stringstream ss;
  ss << std::hex << str;
  u16 x;
  ss >> x;
  return x;
}

inline auto test(const std::string& name, u16 cmds, u16 expected, u16 got){
  if (got != expected){
    throw std::runtime_error(std::to_string(cmds + 1) + "   Expected " + name + ": " + std::to_string(expected) + " but got " + std::to_string(got));
  }
}

inline auto test_cpu(){
  Nes nes;

  nes.load_cardridge("nestest.nes");
  auto log_file = file_open_for_reading("nestest.log");

  auto log_data = std::vector<LogData>{};

  auto line = std::string();
  while(std::getline(log_file, line)){
    auto& data = log_data.emplace_back();
    data.pc = line.substr(0, 4);
    data.cmd = line.substr(6, 2); 
    data.a = line.substr(50, 2);
    data.x = line.substr(55, 2);
    data.y = line.substr(60, 2);
    data.p = line.substr(65, 2);
    data.sp = line.substr(71, 2);
    data.cycles = std::stoi(line.substr(90));
  }

  nes.cpu.pc = 0xC000;
  nes.cpu.set_status(Cpu::Status::Unused, 1);
  nes.cpu.set_status(Cpu::Status::InterruptDisable, 1);

  auto cmds = 0;
  while(cmds < log_data.size()){
    const auto& data = log_data[cmds];

    auto& cpu = nes.cpu;
    test("PC", cmds, hex(data.pc), cpu.pc);
    test("CMD", cmds, hex(data.cmd), (nes.mem_read(cpu.pc)));
    test("A", cmds, hex(data.a), (cpu.accumulator));
    test("X", cmds, hex(data.x), (cpu.x));
    test("Y", cmds, hex(data.y), (cpu.y));
    test("P", cmds, hex(data.p), (cpu.status));
    test("SP", cmds, hex(data.sp), (cpu.sp));
    test("CYC", cmds, data.cycles, cpu.cycles);

    try{
      do{
        nes.cpu.clock(nes);
        cpu.cycles++;
      }while(cpu.req_cycles);
    }
    catch(std::runtime_error){
      throw std::runtime_error("Line number: " + std::to_string(cmds + 1));
    }

    cmds++;
  }

  std::cerr << "CPU TESTS PASSED!\n";
  std::cerr << "0x02: " << int(nes.ram[2]) << '\n';
  std::cerr << "0x03: " << int(nes.ram[3]) << '\n';
}

} //namespace nes

auto main() -> int{
  nes::test_cpu();
}
