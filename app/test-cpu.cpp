#include <catch2/catch.hpp>
#include <cstdint>
#include <vector>

#include "cartridge.hpp"
#include "cpu.hpp"
#include "ppu.hpp"

class VectorMapper : public cartridge::Mapper {
 public:
  VectorMapper(std::vector<uint8_t> bytecode, uint16_t entrypoint = 0x8000u)
      : bytes_(bytecode), entrypoint_(entrypoint){};
  uint8_t prg_read(uint16_t addr) override {
    if (addr < 0x8000) {
      return 0xAA;
    } else if (addr == 0xFFFC) {
      return entrypoint_ & 0xFF;
    } else if (addr == 0xFFFD) {
      return entrypoint_ >> 8;
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
  uint16_t entrypoint_;
};

// #define MAKE_CPU(bytes)

// // bytecode are placed starting at
// cpu::CPU
// makeDummyCPU(cpu::CPU cpu()){return cpu;}

TEST_CASE("constructor") {
  std::vector<uint8_t> bytecode = {0x00, 0x01};
  std::unique_ptr<VectorMapper> mapper(new VectorMapper(bytecode));
  cartridge::Cartridge cart(std::move(mapper));
  ppu::PPU ppu;
  cpu::CPU cpu(ppu, cart);

  REQUIRE(cpu.PC() == 0x8000);

  // cpu = ;
  // cpu::CPU
};