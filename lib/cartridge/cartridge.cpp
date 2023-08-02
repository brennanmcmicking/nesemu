#include "cartridge.hpp"

#include <algorithm>
#include <cstring>
#include <format>
#include <fstream>
#include <memory>
#include <vector>

namespace cartridge {

void assert_cart(bool cond) {
  if (!cond) {
    throw new cartridge_error("Failed to open cartridge");
  }
}

class Mapper0 : public Mapper {
 public:
  Mapper0() = delete;

  Mapper0(std::vector<uint8_t> &&prg_rom, std::vector<uint8_t> &&chr_rom)
      : prg_rom_(prg_rom), chr_rom_(chr_rom) {}

  uint8_t prg_read(uint16_t addr) override {
    switch (addr) {
      case 0x00 ... 0x5FFF:
        // bad range
        throw new cartridge_error(
            std::format("bad memory address for mapper: {}", addr));
        break;
      case 0x6000 ... 0x7FFF:
        // TODO ?
        break;
      case 0x8000 ... 0xFFFF:
        if (prg_rom_.size() == 16 * (1 << 10)) {
          addr -= 0x4000;
        }
        return prg_rom_[addr - 0x8000];
    }
    return 0xAA;
  };
  void prg_write(uint16_t addr, uint8_t data) override {
    switch (addr) {
      case 0x00 ... 0x5FFF:
        // bad range
        throw new cartridge_error(
            std::format("bad memory address for mapper: {}", addr));
        break;
      case 0x6000 ... 0x7FFF:
        // TODO ?
        break;
      case 0x8000 ... 0xFFFF:
        if (prg_rom_.size() == 16 * (1 << 10)) {
          addr -= 0x4000;
        }
        prg_rom_[addr - 0x8000] = data;
        break;
    }
  };

 private:
  std::vector<uint8_t> prg_rom_;
  std::vector<uint8_t> chr_rom_;
};

Cartridge::Cartridge(std::istream &in) {
  uint8_t header[16];
  if (!in.read(reinterpret_cast<char *>(header), 16)) {
    throw std::runtime_error("failed to read header from iNES ROM");
  }

  // "NES\n"
  if (!(header[0] == 0x4E && header[1] == 0x45 && header[2] == 0x53 &&
        header[3] == 0x1A)) {
    throw std::runtime_error(
        "cartridge input doesn't appear to be an iNES ROM");
  }

  std::size_t prg_rom_size_ = 16 * (1 << 10) * header[4];
  std::size_t chr_rom_size_ = 8 * (1 << 10) * header[5];

  bool has_trainer = (header[6] & 0b00000100) > 0;
  if (has_trainer) {
    char trainer[512];
    in.read(trainer, 512);
    // discard >:3
  }

  std::vector<uint8_t> prg_rom(prg_rom_size_);
  std::vector<uint8_t> chr_rom(chr_rom_size_);

  if (!in.read(reinterpret_cast<char *>(prg_rom.data()), prg_rom.size())) {
    throw std::runtime_error("failed to read PRG from iNES ROM");
  }
  if (!in.read(reinterpret_cast<char *>(chr_rom.data()), chr_rom.size())) {
    throw std::runtime_error("failed to read CHR from iNES ROM");
  }

  uint8_t mapper_number = (header[6] >> 4) || (header[7] && 0xFF00);
  switch (mapper_number) {
    case 0:
      mapper_ =
          std::make_unique<Mapper0>(std::move(prg_rom), std::move(chr_rom));
      break;
    default:
      throw new cartridge_error(
          std::format("given mapper number is unsupported: {}", mapper_number));
  }
}
Cartridge::Cartridge(std::unique_ptr<Mapper> mapper) { mapper_.swap(mapper); };
Cartridge::~Cartridge() = default;
uint8_t Cartridge::cpu_read(uint16_t addr) { return mapper_->prg_read(addr); };
void Cartridge::cpu_write(uint16_t addr, uint8_t data) {
  mapper_->prg_write(addr, data);
};

}  // namespace cartridge