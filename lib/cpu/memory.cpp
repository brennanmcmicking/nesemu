#include "boost/log/trivial.hpp"
#include "cpu.hpp"

namespace cpu {
uint8_t CPU::read(uint16_t addr) {
  switch (addr) {
    case 0x0000 ... 0x1FFF:  // Internal RAM
      return ram_[addr % 0x800];
    case 0x2000 ... 0x3FFF:  // TODO: PPU Registers
      switch (addr % 8) {};
      return 0xAA;
    case 0x4000 ... 0x4017:  // TODO: IO
      return 0xAA;
    case 0x4018 ... 0x401F:  // APU and I/O that is normally disabled
      BOOST_LOG_TRIVIAL(fatal) << "Memory location disabled: " << addr;
    case 0x4020 ... 0xFFFF:  // TODO: Cartridge space
      return cart_.cpu_read(addr);
    default:
      BOOST_LOG_TRIVIAL(fatal) << "Invalid memory address read: " << addr;
      return 0x00;
  }
};

bool CPU::write(uint16_t addr, uint8_t data) {
  switch (addr) {
    case 0x0000 ... 0x1FFF:  // Internal RAM
      ram_[addr % 0x800] = data;
      return true;
    case 0x2000 ... 0x3FFF:  // TODO: PPU Registers
      switch (addr % 8) {};
      return false;
    case 0x4000 ... 0x4017:  // TODO: IO
      return false;
    case 0x4018 ... 0x401F:  // APU and I/O that is normally disabled
      BOOST_LOG_TRIVIAL(fatal) << "Memory location disabled: " << addr;
      return false;
    case 0x4020 ... 0xFFFF:  // TODO: Cartridge space
      // cart_.cpu_write(addr, data);
      return false;  // for now, writing is not allowed
    default:
      BOOST_LOG_TRIVIAL(fatal) << "Invalid memory address read: " << addr;
      return false;
  }
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