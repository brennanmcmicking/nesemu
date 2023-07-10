#include <cstdint>

#include "cpu.hpp"

namespace cpu {

// generated using scripts/scrape-opcodes.py
// missing instructions are default initialized to zero (invalid instruction)
constexpr std::size_t instruction_sizes[256] = {
    1, 2, 0, 0, 0, 2, 2, 0, 1, 2, 1, 0, 0, 3, 3, 0, 2, 2, 0, 0, 0, 2, 2, 0,
    1, 3, 0, 0, 0, 3, 3, 0, 3, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0, 1, 2, 0, 0, 0, 2, 2, 0,
    1, 2, 1, 0, 3, 3, 3, 0, 2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,
    1, 2, 0, 0, 0, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0, 2, 2, 0, 0, 0, 2, 2, 0,
    1, 3, 0, 0, 0, 3, 3, 0, 0, 2, 0, 0, 2, 2, 2, 0, 1, 0, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 0, 3, 0, 0, 2, 2, 2, 0, 2, 2, 2, 0,
    1, 2, 1, 0, 3, 3, 3, 0, 2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0, 2, 2, 0, 0, 0, 2, 2, 0,
    1, 3, 0, 0, 0, 3, 3, 0, 2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0};

std::size_t CPU::byte_count(uint8_t opcode) {
  return instruction_sizes[opcode];
};

};  // namespace cpu