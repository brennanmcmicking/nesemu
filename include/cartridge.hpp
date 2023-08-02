#ifndef CARTRIDGE_HPP
#define CARTRIDGE_HPP
#include <cstdint>
#include <ios>
#include <memory>
#include <vector>

namespace cartridge {

using cartridge_error = std::runtime_error;

class Mapper {
 public:
  virtual ~Mapper() = default;

  virtual uint8_t prg_read(uint16_t addr) = 0;
  virtual void prg_write(uint16_t addr, uint8_t data) = 0;

  // virtual uint8_t chr_read(uint16_t addr) = 0;
  // virtual void chr_write(uint16_t addr, uint8_t data) = 0;

  void print_debug(std::ostream &out);  // for nesemu-cartridge
};

class Cartridge {
 public:
  Cartridge() = delete;
  explicit Cartridge(std::istream &in);
  explicit Cartridge(
      std::unique_ptr<Mapper, std::default_delete<Mapper>> mapper);

  ~Cartridge();

  Cartridge(Cartridge &) = delete;
  Cartridge(Cartridge &&) = delete;
  Cartridge &operator=(Cartridge &) = delete;
  Cartridge operator=(Cartridge &&) = delete;

  uint8_t cpu_read(uint16_t addr);
  void cpu_write(uint16_t addr, uint8_t data);

  // uint8_t ppu_read(uint16_t addr);
  // void ppu_write(uint16_t addr, uint8_t data);

 private:
  std::unique_ptr<Mapper> mapper_;
};

}  // namespace cartridge
#endif  // CARTRIDGE_HPP