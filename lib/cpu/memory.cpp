#include "boost/log/trivial.hpp"
#include "cpu.hpp"

namespace cpu {
uint8_t CPU::read(uint16_t addr) {
  if (addr < 0x2000) {
    return ram_[addr % 0x800];
  } else if (addr < 0x4000) {
    switch (addr % 8) {};
    return 0xAA;  // TODO ppu
  } else if (addr < 0x4020) {
    return 0xAA;  // TODO io
  } else {
    return cart_.cpu_read(addr);
  }
};

bool CPU::write(uint16_t addr, uint8_t data) {
  if (addr < 0x2000) {
    ram_[addr % 0x800] = data;
    return true;
  } else if (addr < 0x4000) {
    switch (addr % 8) {};
    return false;  // TODO ppu
  } else if (addr < 0x4020) {
    return false;  // TODO io
  } else {
    // cart_.cpu_write(addr, data);
    return false;  // for now, writing is not allowed
  }
  return true;
}

uint16_t CPU::read16(uint16_t addr) {
  uint16_t lo = read(addr);
  uint16_t hi = read(addr + 1);
  return (hi << 8) | lo;
};

void CPU::write16(uint16_t addr, uint16_t data) {
  write(addr, data & 0xFF);
  write(addr + 1, data >> 8);
};

uint16_t CPU::addr_fetch(AddrMode mode) {
  switch (mode) {
    case kZeroPage: {
      uint16_t addr = read(PC_ + 1);
      return addr;
    }
    case kZeroPageX: {
      uint16_t addr = read(PC_ + 1);
      return (addr + X_) & 0xFF;
    }
    case kZeroPageY: {
      uint16_t addr = read(PC_ + 1);
      return (addr + Y_) & 0xFF;
    }
    case kRelative: {
      int8_t offset = static_cast<int8_t>(read(PC_ + 1));
      uint8_t instruction_length = 1 + 1;  // opcode + operand
      return PC_ + instruction_length + offset;
    }
    case kAbsolute: {
      uint16_t addr = read16(PC_ + 1);
      return addr;
    }
    case kAbsoluteX: {
      uint16_t addr = read16(PC_ + 1);
      return addr + X_;
    }
    case kAbsoluteY: {
      uint16_t addr = read16(PC_ + 1);
      return addr + Y_;
    }
    case kIndirect: {
      uint16_t addr = read16(PC_ + 1);
      return read16(addr);
    }
    case kIndexedIndirect: {
      uint16_t addr = read(PC_ + 1);
      return read16((addr + X_) & 0xFF);
    }
    case kIndirectIndexed: {
      uint16_t addr = read(PC_ + 1);
      return read16(addr) + Y_;
    }
    default: {
      BOOST_LOG_TRIVIAL(fatal) << "Invalid addressing mode: " << mode;
      return 0;
    }
  }
}

uint8_t CPU::value_fetch(AddrMode mode) {
  switch (mode) {
    case kAccumulator: {
      return A_;
    }
    case kImmediate: {
      return read(PC_ + 1);
    }
    default: {
      uint16_t addr = addr_fetch(mode);
      return read(addr);
    }
  }
}

}  // namespace cpu