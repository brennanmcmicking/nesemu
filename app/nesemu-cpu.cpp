#include <boost/log/trivial.hpp>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <iostream>
#include <vector>

#include "cartridge.hpp"
#include "cpu.hpp"
#include "util.hpp"

class VectorMapper : public cartridge::Mapper {
 public:
  VectorMapper() : prg_rom_(0x4000) {
    std::cin.read((char*)prg_rom_.data(), prg_rom_.size());
    // eof is expected, but not an error
    if (std::cin.fail() && !std::cin.eof()) {
      std::cout << "Failed to read PRG ROM\n";
      std::exit(1);
    }
    // set RESET vector to 0x8000 (start of PRG ROM)
    prg_rom_[0xFFFC % 0x4000] = 0x00;
    prg_rom_[0xFFFD % 0x4000] = 0x80;
  };
  uint8_t prg_read(uint16_t addr) override {
    if (addr < 0x8000) {
      return 0x00;
    }
    return prg_rom_.at(addr % 0x4000);
  };
  void prg_write(uint16_t addr, uint8_t data) override {
    if (addr == 0xFFFF) {
      std::cout << std::format("${:02x}\n", data);
    }
  };

 private:
  std::vector<uint8_t> prg_rom_;
};

int main(int argc, char* argv[]) {
  util::init_log_level();

  if (argc < 2) {
    std::cout << std::format(
        "Usage: {} <CYCLES>\n"
        "Emulate a NES CPU, where execution terminates after CYCLES cycles.\n"
        "The PRG ROM is read in through stdin. The following memory map is "
        "used:\n"
        "  $0000-$07FF: internal RAM\n"
        "  $8000-$BFFF: PRG ROM (read-only)\n"
        "  $C000-$FFFE: Mirrored PRG ROM (minus the last byte)\n"
        "  $FFFF-$FFFF: stdout (write-only)\n",
        argv[0]);
    return 1;
  }

  unsigned long cycles;
  try {
    cycles = std::stoul(argv[1]);
  } catch (const std::invalid_argument& e) {
    std::cerr << "Argument error: cycles should be a number.\n";
    return 1;
  } catch (const std::out_of_range& e) {
    std::cerr << "Argument error: cycles out of range for unsigned long.\n";
    return 1;
  }

  std::unique_ptr<VectorMapper> mapper(new VectorMapper());
  cartridge::Cartridge cart(std::move(mapper));
  cpu::CPU cpu(cart);

  cpu.advance_cycles(cycles);
  return 0;
}
