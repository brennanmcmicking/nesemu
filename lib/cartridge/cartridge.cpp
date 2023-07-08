#include "cartridge.hpp"

#include <memory>

namespace cartridge {
Cartridge::Cartridge(std::unique_ptr<Mapper> mapper) { mapper_.swap(mapper); };
uint8_t Cartridge::cpu_read(uint16_t addr) { return mapper_->prg_read(addr); };
void Cartridge::cpu_write(uint16_t addr, uint8_t data) {
  mapper_->prg_write(addr, data);
};
}  // namespace cartridge