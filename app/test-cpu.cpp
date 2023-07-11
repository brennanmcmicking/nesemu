#include <catch2/catch.hpp>
#include <cstdint>
#include <vector>

#include "cartridge.hpp"
#include "cpu.hpp"
#include "ppu.hpp"

using namespace cpu;

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
  void prg_write(uint16_t addr, uint8_t data) override {
    (void)addr;  // explicitly unused
    (void)data;
  };

 private:
  std::vector<uint8_t> bytes_;
  uint16_t entrypoint_;
};

#define U16(x) ((x)&0xFF), ((x) >> 8)

#define MAKE_CPU(bytecode)                                            \
  std::unique_ptr<VectorMapper> __mapper(new VectorMapper(bytecode)); \
  cartridge::Cartridge __cart(std::move(__mapper));                   \
  ppu::PPU __ppu;                                                     \
  CPU cpu(__ppu, __cart);

TEST_CASE("trivial load and store") {
  // clang-format off
  std::vector<uint8_t> bytecode = {
      // LDA #$01
      kLDA_IMM, 0x01,
      // STA $0000
      kSTA_ZP, 0x00,
      // LDA #$02
      kLDA_IMM, 0x02,
      // LDA $0000
      kLDA_ZP, 0x00, 
  };  // clang-format on

  MAKE_CPU(bytecode);

  REQUIRE(cpu.PC() == 0x8000);
  REQUIRE(cpu.A() == 0);

  // LDA #$01
  cpu.cycle();
  REQUIRE(cpu.PC() == 0x8000);
  REQUIRE(cpu.A() == 0);

  cpu.cycle();
  REQUIRE(cpu.PC() == 0x8002);
  REQUIRE(cpu.A() == 1);

  REQUIRE(cpu.read(0x0000) == 0x00);

  // STA $0000
  cpu.cycle();
  REQUIRE(cpu.PC() == 0x8002);
  REQUIRE(cpu.A() == 1);

  cpu.cycle();
  REQUIRE(cpu.PC() == 0x8002);
  REQUIRE(cpu.A() == 1);

  cpu.cycle();
  REQUIRE(cpu.PC() == 0x8004);
  REQUIRE(cpu.A() == 1);

  REQUIRE(cpu.read(0x0000) == 0x01);

  // LDA #$02
  cpu.cycle();
  REQUIRE(cpu.PC() == 0x8004);
  REQUIRE(cpu.A() == 1);

  cpu.cycle();
  REQUIRE(cpu.PC() == 0x8006);
  REQUIRE(cpu.A() == 2);

  // LDA $0000
  cpu.cycle();
  REQUIRE(cpu.PC() == 0x8006);
  REQUIRE(cpu.A() == 2);

  cpu.cycle();
  REQUIRE(cpu.PC() == 0x8006);
  REQUIRE(cpu.A() == 2);

  cpu.cycle();
  REQUIRE(cpu.PC() == 0x8008);
  REQUIRE(cpu.A() == 1);
};

TEST_CASE("trivial memory") {
  std::vector<uint8_t> bytecode = {};

  uint8_t val8 = 123;
  uint16_t val16 = 12345;

  MAKE_CPU(bytecode);

  // RAM
  REQUIRE(cpu.read(0x0000) == 0x00);
  REQUIRE(cpu.write(0x0800, val8));
  REQUIRE(cpu.read(0x0000) == val8);
  REQUIRE(cpu.write16(0x0900, val16));
  REQUIRE(cpu.read16(0x0100) == val16);
};