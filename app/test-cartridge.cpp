#include <catch2/catch.hpp>

#include "cartridge.hpp"

class DummyMapper : public cartridge::Mapper {
 public:
  DummyMapper() = delete;

  DummyMapper(std::vector<uint8_t> &&prg_rom) : prg_rom_(prg_rom) {}

  uint8_t prg_read(uint16_t addr) {
    switch (addr) {
      case 0x00 ... 0xFF:
        return prg_rom_[addr];
    }
    return 0xAA;
  }

  void prg_write(uint16_t addr, uint8_t val) {
    switch (addr) {
      case 0x00 ... 0xFF:
        //
        prg_rom_[addr] = val;
    }
  }

 private:
  std::vector<uint8_t> prg_rom_;
};

TEST_CASE("Basic cartridge read/write") {
  std::vector<uint8_t> ram(0xFF, 0);
  std::unique_ptr<DummyMapper> m(new DummyMapper(std::move(ram)));
  cartridge::Cartridge cart(std::move(m));
  for (uint8_t i = 1; i < 0xFF; i++) {
    REQUIRE(cart.cpu_read(i) == 0);
    cart.cpu_write(i, i);
    REQUIRE(cart.cpu_read(i) == i);
  }
}