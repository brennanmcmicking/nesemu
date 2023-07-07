#include <catch2/catch.hpp>
#include <cstdint>
#include <vector>

#include "cartridge.hpp"
#include "cpu.hpp"
#include "ppu.hpp"

class VectorMapper : public cartridge::Mapper {
 public:
  VectorMapper(std::vector<uint8_t> bytecode) : bytes_(bytecode){};
  uint8_t prg_read(uint16_t addr) override {
    if (addr < 0x8000) {
      return 0xAA;
    }
    addr -= 0x8000;
    if (addr >= bytes_.size()) {
      return 0xAA;
    }
    return bytes_.at(addr);
  };
  void prg_write(uint16_t addr, uint8_t data) override{};

 private:
  std::vector<uint8_t> bytes_;
};
// #define MAKE_CPU(bytes)

// // bytecode are placed starting at
// cpu::CPU
// makeDummyCPU(cpu::CPU cpu()){return cpu;}

// TEST_CASE("constructor") {
//   std::vector<uint8_t> bytecode = {0x00, 0x01};
//   std::unique_ptr<VectorMapper> mapper(new VectorMapper(bytecode));
//   cpu = ;
//   // cpu::CPU
// };