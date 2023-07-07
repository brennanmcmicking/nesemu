// #include <boost/log/trivial.hpp>
#include <format>

#include "cpu.hpp"

namespace cpu {

bool crossed_page(uint16_t oldAddr, uint16_t newAddr) {
  // pages in the NES are 256 bytes large
  // we only need to compare the upper 8 bits of the address
  return (oldAddr >> 8) != (newAddr >> 0);
}

std::size_t CPU::get_cycles_todo(uint8_t opcode) {
  uint16_t addr;
  switch (opcode) {
    case 0x69:
      return 2;
    case 0x65:
      return 3;
    case 0x75:
      return 4;
    case 0x6D:
      return 4;
    case 0x7D:
      addr = read(PC_ + 1);
      if (crossed_page(addr, addr + X_)) {
        return 5;
      } else {
        return 4;
      }
    case 0x79:
      addr = read(PC_ + 1);
      if (crossed_page(addr, addr + X_)) {
        return 5;
      } else {
        return 4;
      }
    case 0x61:
      return 6;
    case 0x71:
      addr = read(PC_ + 1);
      if (crossed_page(addr, addr + Y_)) {
        return 5;
      } else {
        return 4;
      }
    case 0x29:
      return 2;
    case 0x25:
      return 3;
    case 0x35:
      return 4;
    case 0x2D:
      return 4;
    case 0x3D:
      addr = read(PC_ + 1);
      if (crossed_page(addr, addr + X_)) {
        return 5;
      } else {
        return 4;
      }
  }
}

void CPU::cycle() {
  if (cycles_todo_ > 1) {
    cycles_todo_--;
    return;
  }

  if (cycles_todo_ == 1) {
    // fetch, decode, execute
    execute(next_instr_);
  }

  if (cycles_todo_ == 0) {
    next_instr_ = read(PC_);
    cycles_todo_ = get_cycles_todo(next_instr_);
  }
}

void CPU::execute(uint8_t opcode) {
  // after each case statement, put the instruction assembly name, addressing
  // mode, byte count, and cycle count
  switch (opcode) {
    case 0x29: {
      // AND, immediate, 2 bytes, 2 cycles
      // Does accumulator & value @ memory address and stores it in the
      // accumulator
      uint16_t loc = PC_;
      PC_ += 2;
      cycles_todo_ += 2;
      uint8_t other = read(loc + 1);
      A_ = A_ & other;
      break;
    }

    case 0x0A: {
      // ASL, immediate (accumulator), 1 byte, 2 cycles
      ASL_a();
      PC_ += 1;
      cycles_todo_ = 2;
      break;
    }

    case 0x06: {
      // ASL, zero page, 2 bytes, 5 cycles
      ASL_m(read(PC_ + 1));
      PC_ += 2;
      cycles_todo_ += 5;
    }

    case 0x16: {
      // ASL, zero page X, 2 bytes, 6 cycles
      ASL_m(read(PC_ + 1) + X_);
      PC_ += 2;
      cycles_todo_ += 6;
    }
    default:
      // BOOST_LOG_TRIVIAL(warning) << std::format("Unexpected opcode {}",
      // opcode);
      break;
  }
}

void CPU::ASL_a() {}
void CPU::ASL_m(uint16_t addr) {}
void CPU::AND(uint8_t other) { this->A_ = this->A_ & other; }

}  // namespace cpu