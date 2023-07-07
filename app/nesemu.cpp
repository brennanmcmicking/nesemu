#include <boost/log/trivial.hpp>
#include <cstdint>
#include <memory>

#include "cartridge.hpp"
#include "cpu.hpp"
#include "ppu.hpp"

// this is just here to make it compile
class DummyMapper : public cartridge::Mapper {
 public:
  DummyMapper(){};
  uint8_t prg_read(uint16_t addr) override { return 0; };
  void prg_write(uint16_t addr, uint8_t data) override{};
};

int main(int argc, char* argv[]) {
  ppu::PPU ppu;
  cartridge::Cartridge cart(std::make_unique<DummyMapper>());
  cpu::CPU cpu(ppu, cart);
  BOOST_LOG_TRIVIAL(info) << "hello world\n";
  BOOST_LOG_TRIVIAL(info)
      << "this program will be the fully integrated nes emulator!\n";

  cpu.cycle();
}