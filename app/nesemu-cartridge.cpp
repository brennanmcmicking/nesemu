#include <format>
#include <fstream>
#include <iostream>

#include "util.hpp"

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  util::init_log_level();

  if (argc < 2) {
    exit(1);
  }

  std::ifstream in(argv[1]);
  if (!in.is_open()) {
    exit(1);
  }

  uint8_t header[16];
  if (!in.read(reinterpret_cast<char *>(header), 16)) {
    throw std::runtime_error("failed to read header from iNES ROM");
  }

  // "NES\n"
  if (!(header[0] == 0x4E && header[1] == 0x45 && header[2] == 0x53 &&
        header[3] == 0x1A)) {
    exit(1);
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
    exit(1);
  }
  if (!in.read(reinterpret_cast<char *>(chr_rom.data()), chr_rom.size())) {
    exit(1);
  }

  uint8_t mapper_number = (header[6] >> 4) || (header[7] && 0xFF00);

  std::cout << unsigned(mapper_number) << "\n";
  for (std::size_t i = 0; i < prg_rom_size_; i++) {
    std::cout << std::format("{:02X}", prg_rom[i]);
  }
  std::cout << "\n";

  for (std::size_t i = 0; i < chr_rom_size_; i++) {
    std::cout << std::format("{:02X}", chr_rom[i]);
  }
  std::cout << "\n";

  return 0;
}