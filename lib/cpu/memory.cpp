#include "boost/log/trivial.hpp"
#include "cpu.hpp"
#include "util.hpp"

namespace cpu {
uint8_t CPU::read(uint16_t addr) {
  switch (addr) {
    case 0x0000 ... 0x1FFF:  // Internal RAM
      return ram_[addr % 0x800];
    case 0x2000 ... 0x3FFF: {  // PPU Registers
      if (!ppu_.has_value()) {
        BOOST_LOG_TRIVIAL(debug)
            << "Read from PPU register without attached PPU: " << addr;
        return 0xAA;
      }
      PPU& ppu = ppu_.value().get();
      switch (addr % 8) {
        case 0x02:  // PPUSTATUS
          return ppu.get_PPUSTATUS();
        case 0x04:  // OAMDATA (r/w)
          return ppu.get_OAMDATA();
        case 0x07:  // PPUDATA (r/w)
          return ppu.get_PPUDATA();
        default:
          BOOST_LOG_TRIVIAL(debug)
              << "Invalid PPU register read: " << util::fmt_hex(addr)
              << " (mod 8 is " << util::fmt_hex((uint16_t)(addr % 8)) << ")";
          return 0xAA;
      };
    }
    case 0x4000 ... 0x4015:  // Sound
      BOOST_LOG_TRIVIAL(debug) << "Sound not implemented: " << addr;
      return 0xAA;
    case 0x4016: {
      if (!controller_.has_value()) {
        BOOST_LOG_TRIVIAL(debug)
            << "Read from controller register without attached controller: "
            << addr;
        return 0xAA;
      }
      Controller& controller = controller_.value().get();
      return controller.read_joy1();
    }
    case 0x4017: {
      return 0x00;
    }
    case 0x4018 ... 0x401F:  // APU and I/O that is normally disabled
      BOOST_LOG_TRIVIAL(debug) << "Memory location disabled: " << addr;
      return 0xAA;
    case 0x4020 ... 0xFFFF:  // TODO: Cartridge space
      return cart_.cpu_read(addr);
    default:
      BOOST_LOG_TRIVIAL(fatal) << "Invalid memory address read: " << addr;
      return 0xAA;
  }
};

bool CPU::write(uint16_t addr, uint8_t data) {
  switch (addr) {
    case 0x0000 ... 0x1FFF:  // Internal RAM
      ram_[addr % 0x800] = data;
      return true;
    case 0x2000 ... 0x3FFF: {  // TODO: PPU Registers
      if (!ppu_.has_value()) {
        BOOST_LOG_TRIVIAL(debug)
            << "Read from PPU register without attached PPU: " << addr;
        return false;
      }
      PPU& ppu = ppu_.value().get();
      switch (addr % 8) {
        case 0x00:  // PPUCTRL
          ppu.set_PPUCTRL(data);
          return true;
        case 0x01:  // PPUMASK
          ppu.set_PPUMASK(data);
          return true;
        case 0x03:  // OAMADDR
          ppu.set_OAMADDR(data);
          return true;
        case 0x04:  // OAMDATA (r/w)
          ppu.set_OAMDATA(data);
          return true;
        case 0x05:  // PPUSCROLL
          ppu.set_PPUSCROLL(data);
          return true;
        case 0x06:  // PPUADDR
          ppu.set_PPUADDR(data);
          return true;
        case 0x07:  // PPUDATA (r/w)
          ppu.set_PPUDATA(data);
          return true;
      };
      return false;
    }
    case 0x4000 ... 0x4015:  // Sound
      BOOST_LOG_TRIVIAL(trace) << "Sound not implemented: " << addr;
      return false;
    case 0x4016: {
      if (!controller_.has_value()) {
        BOOST_LOG_TRIVIAL(debug)
            << "Write to controller register without attached controller: "
            << addr;
        return false;
      }
      Controller& controller = controller_.value().get();
      controller.write_strobe(data);
      return true;
    }
    case 0x4017: {
      return true;
    }
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

bool CPU::write16(uint16_t addr, uint16_t data) {
  return write(addr, data & 0xFF) && write(addr + 1, data >> 8);
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
    case kRelative:
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
