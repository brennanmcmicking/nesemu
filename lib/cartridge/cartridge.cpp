#include "cartridge.hpp"

#include <memory>

namespace cartridge {
Cartridge::Cartridge(std::unique_ptr<Mapper> mapper) { mapper_.swap(mapper); };
}  // namespace cartridge