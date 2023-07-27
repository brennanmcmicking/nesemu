#include "cartridge.hpp"

#include <cstring>
#include <format>
#include <fstream>
#include <memory>

namespace cartridge {

void assert_cart(bool cond) {
  if (!cond) {
    throw new cartridge_error("Failed to open cartridge");
  }
}

class Mapper0 : public Mapper {
 public:
  Mapper0() = delete;

  Mapper0(uint8_t *prg_rom, std::size_t prg_rom_size, uint8_t *chr_rom,
          std::size_t chr_rom_size)
      : prg_rom_size_(prg_rom_size), chr_rom_size_(chr_rom_size) {
    this->prg_rom_ = prg_rom;
    this->chr_rom_ = chr_rom;
  }

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
      case 0x8000 ... 0xBFFF:
        return prg_rom_[addr - 0x8000];
    }
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
      case 0x8000 ... 0xBFFF:
        prg_rom_[addr - 0x8000] = data;
    }
  };

 private:
  uint8_t *prg_rom_;
  uint8_t *chr_rom_;
  std::size_t prg_rom_size_;
  std::size_t chr_rom_size_;
};

Cartridge::Cartridge(std::istream &in) {
  uint8_t header[16];
  in.read(reinterpret_cast<char *>(header), 16);

  // "NES\n"
  assert_cart(header[0] == 0x4E);
  assert_cart(header[1] == 0x45);
  assert_cart(header[2] == 0x53);
  assert_cart(header[3] == 0x1A);

  prg_rom_size_ = 16384 * header[4];
  chr_rom_size_ = 8192 * header[5];

  bool has_trainer = (header[6] & 0b00000100) > 0;
  if (has_trainer) {
    char trainer[512];
    in.read(trainer, 512);
    // discard for now
  }

  prg_rom_ = static_cast<uint8_t *>(::operator new(prg_rom_size_));
  chr_rom_ = static_cast<uint8_t *>(::operator new(chr_rom_size_));

  in.read(reinterpret_cast<char *>(prg_rom_), prg_rom_size_);
  in.read(reinterpret_cast<char *>(chr_rom_), chr_rom_size_);

  uint8_t mapper_number = (header[6] >> 4) || (header[7] && 0xFF00);
  switch (mapper_number) {
    case 0:
      mapper_ = std::make_unique<Mapper0>(prg_rom_, prg_rom_size_, chr_rom_,
                                          chr_rom_size_);
      break;
    default:
      throw new cartridge_error(
          std::format("given mapper number is unsupported: {}", mapper_number));
  }
}
Cartridge::Cartridge(std::unique_ptr<Mapper> mapper) { mapper_.swap(mapper); };
Cartridge::~Cartridge() {
  if (prg_rom_ != nullptr) {
    ::operator delete(prg_rom_);
    prg_rom_ = nullptr;
  }
  if (chr_rom_ != nullptr) {
    ::operator delete(chr_rom_);
    chr_rom_ = nullptr;
  }
}
uint8_t Cartridge::cpu_read(uint16_t addr) { return mapper_->prg_read(addr); };
void Cartridge::cpu_write(uint16_t addr, uint8_t data) {
  mapper_->prg_write(addr, data);
};

}  // namespace cartridge