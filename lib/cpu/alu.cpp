#include <boost/log/trivial.hpp>
#include <format>

#include "cpu.hpp"

namespace cpu {

// detect whether a page boundary was crossed when addr is incremented by offset
bool crossed_page(uint16_t addr, uint8_t offset) {
  // pages in the NES are 256 bytes large
  // we only need to compare the upper 8 bits of the address
  return (addr >> 8) != ((addr + offset) >> 0);
}

std::size_t CPU::cycle_count(uint8_t opcode) {
  uint16_t addr;
  switch (opcode) {
    case 0xA9:
      return 2;
    case 0xA5:
      return 3;
    case 0x85:
      return 3;
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
      if (crossed_page(addr, X_)) {
        return 5;
      } else {
        return 4;
      }
    case 0x79:
      addr = read(PC_ + 1);
      if (crossed_page(addr, X_)) {
        return 5;
      } else {
        return 4;
      }
    case 0x61:
      return 6;
    case 0x71:
      addr = read(PC_ + 1);
      if (crossed_page(addr, Y_)) {
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
  return 0;
}

void CPU::advance(std::size_t cycles) {
  for (std::size_t i = 0; i < cycles; i++) {
    cycle();
  }
}

void CPU::cycle() {
  BOOST_LOG_TRIVIAL(trace) << "cycle()";
  BOOST_LOG_TRIVIAL(trace) << std::format(
      "PC: {:04X}, SP: {:02X}, A: {:02X}, X: {:02X}, Y: {:02X}, P: {:02X}", PC_,
      SP_, A_, X_, Y_, P_);
  BOOST_LOG_TRIVIAL(trace) << std::format("cycles_todo: {}", cycles_todo_);

  if (cycles_todo_ == 1) {
    // fetch, decode, execute
    execute(next_instr_);
  } else if (cycles_todo_ == 0) {
    BOOST_LOG_TRIVIAL(trace) << "fetching next instruction";
    next_instr_ = read(PC_);
    cycles_todo_ = cycle_count(next_instr_);
    BOOST_LOG_TRIVIAL(trace) << std::format("next_instr: {:02X}", next_instr_);
  }
  cycles_todo_--;
}

void CPU::execute(uint8_t opcode) {
  BOOST_LOG_TRIVIAL(trace) << std::format("execute({:02X})", opcode);
  // after each case statement, put the instruction assembly name,
  // addressing mode, byte count, and cycle count
  switch (opcode) {
    case kADC_IMM: {
      // ADC, Immediate, 2 bytes, 2 cycles
      ADC(value_fetch(kImmediate));
      PC_ += byte_count(kADC_IMM);
      break;
    }
    case kADC_ZP: {
      // ADC, Zero Page, 2 bytes, 3 cycles
      ADC(value_fetch(kZeroPage));
      PC_ += byte_count(kADC_ZP);
      break;
    }
    case kADC_ZPX: {
      // ADC, Zero Page,X, 2 bytes, 4 cycles
      ADC(value_fetch(kZeroPageX));
      PC_ += byte_count(kADC_ZPX);
      break;
    }
    case kADC_ABS: {
      // ADC, Absolute, 3 bytes, 4 cycles
      ADC(value_fetch(kAbsolute));
      PC_ += byte_count(kADC_ABS);
      break;
    }
    case kADC_ABSX: {
      // ADC, Absolute,X, 3 bytes, 4 (+1 if page crossed)
      ADC(value_fetch(kAbsoluteX));
      PC_ += byte_count(kADC_ABSX);
      break;
    }
    case kADC_ABSY: {
      // ADC, Absolute,Y, 3 bytes, 4 (+1 if page crossed)
      ADC(value_fetch(kAbsoluteY));
      PC_ += byte_count(kADC_ABSY);
      break;
    }
    case 0x61: {
      // ADC, (Indirect,X), 2 bytes, 6 cycles
      // TODO
      break;
    }
    case 0x71: {
      // ADC, (Indirect),Y, 2 bytes, 5 (+1 if page crossed)
      // TODO
      break;
    }
    case kAND_IMM: {
      // AND, Immediate, 2 bytes, 2 cycles
      // Does accumulator & value @ memory address and stores it in the
      // accumulator
      AND(value_fetch(kImmediate));
      PC_ += byte_count(kAND_IMM);
      break;
    }
    case kAND_ZP: {
      // AND, Zero Page, 2 bytes, 3 cycles
      AND(value_fetch(kZeroPage));
      PC_ += byte_count(kAND_ZP);
      break;
    }
    case kAND_ZPX: {
      // AND, Zero Page,X, 2 bytes, 4 cycles
      AND(value_fetch(kZeroPageX));
      PC_ += byte_count(kAND_ZPX);
      break;
    }
    case kAND_ABS: {
      // AND, Absolute, 3 bytes, 4 cycles
      AND(value_fetch(kAbsolute));
      PC_ += byte_count(kAND_ABS);
      break;
    }
    case kAND_ABSX: {
      // AND, Absolute,X, 3 bytes, 4 (+1 if page crossed)
      AND(value_fetch(kAbsoluteX));
      PC_ += byte_count(kAND_ABSX);
      break;
    }
    case kAND_ABSY: {
      // AND, Absolute,Y, 3 bytes, 4 (+1 if page crossed)
      AND(kAbsoluteY);
      PC_ += byte_count(kAND_ABSY);
      break;
    }
    case kAND_INDX: {
      // AND, (Indirect,X), 2 bytes, 6 cycles
      // TODO
      break;
    }
    case kAND_INDY: {
      // AND, (Indirect),Y, 2 bytes, 5 (+1 if page crossed)
      // TODO
      break;
    }
    case kASL_A: {
      // ASL, Accumulator, 1 bytes, 2 cycles
      ASL_a();
      PC_ += byte_count(kASL_A);
      break;
    }
    case kASL_ZP: {
      // ASL, Zero Page, 2 bytes, 5 cycles
      // since ASL writes back to memory, we pass it the address that we want to
      // to shift instead of the value to shift
      ASL_m(addr_fetch(kZeroPage));
      PC_ += byte_count(kASL_ZP);
      break;
    }
    case kASL_ZPX: {
      // ASL, Zero Page,X, 2 bytes, 6 cycles
      ASL_m(addr_fetch(kZeroPageX));
      PC_ += byte_count(kASL_ZPX);
      break;
    }
    case kASL_ABS: {
      // ASL, Absolute, 3 bytes, 6 cycles
      ASL_m(addr_fetch(kAbsolute));
      PC_ += byte_count(kASL_ABS);
      break;
    }
    case kASL_ABSX: {
      // ASL, Absolute,X, 3 bytes, 7 cycles
      ASL_m(addr_fetch(kAbsoluteX));
      PC_ += byte_count(kASL_ABSX);
      break;
    }
    case kBCC_REL: {
      // BCC, Relative, 2 bytes, 2 (+1 if branch succeeds +2 if to a new
      // page)
      BRANCH(kBCC_REL, !get_carry());
      break;
    }
    case kBCS_REL: {
      // BCS, Relative, 2 bytes, 2 (+1 if branch succeeds +2 if to a new
      // page)
      // See the comments on kBCC_REL to understand what's going on here
      BRANCH(kBCS_REL, get_carry());
      break;
    }
    case kBEQ_REL: {
      // BEQ, Relative, 2 bytes, 2 (+1 if branch succeeds +2 if to a new
      // page)
      BRANCH(kBEQ_REL, get_zero());
      break;
    }
    case kBIT_ZP: {
      // BIT, Zero Page, 2 bytes, 3 cycles
      BIT(kZeroPage);
      break;
    }
    case kBIT_ABS: {
      // BIT, Absolute, 3 bytes, 4 cycles
      BIT(kAbsolute);
      break;
    }
    case kBMI_REL: {
      // BMI, Relative, 2 bytes, 2 (+1 if branch succeeds +2 if to a new
      // page)
      BRANCH(kBMI_REL, get_negative());
      break;
    }
    case kBNE_REL: {
      // BNE, Relative, 2 bytes, 2 (+1 if branch succeeds +2 if to a new
      // page)
      BRANCH(kBNE_REL, !get_zero());
      break;
    }
    case kBPL_REL: {
      // BPL, Relative, 2 bytes, 2 (+1 if branch succeeds +2 if to a new
      // page)
      BRANCH(kBPL_REL, !get_negative());
      break;
    }
    case 0x00: {
      // BRK, Implied, 1 bytes, 7 cycles
      // TODO
      break;
    }
    case kBVC_REL: {
      // BVC, Relative, 2 bytes, 2 (+1 if branch succeeds +2 if to a new
      // page)
      BRANCH(kBVC_REL, !get_overflow());
      break;
    }
    case kBVS_REL: {
      // BVS, Relative, 2 bytes, 2 (+1 if branch succeeds +2 if to a new
      // page)
      BRANCH(kBVS_REL, get_overflow());
      break;
    }
    case kCLC: {
      // CLC, Implied, 1 bytes, 2 cycles
      set_carry(false);
      break;
    }
    case kCLD: {
      // CLD, Implied, 1 bytes, 2 cycles
      set_decimal(false);
      break;
    }
    case 0x58: {
      // CLI, Implied, 1 bytes, 2 cycles
      set_interrupt_disable(false);
      break;
    }
    case 0xB8: {
      // CLV, Implied, 1 bytes, 2 cycles
      set_overflow(false);
      break;
    }
    case 0xC9: {
      // CMP, Immediate, 2 bytes, 2 cycles
      // TODO
      break;
    }
    case 0xC5: {
      // CMP, Zero Page, 2 bytes, 3 cycles
      // TODO
      break;
    }
    case 0xD5: {
      // CMP, Zero Page,X, 2 bytes, 4 cycles
      // TODO
      break;
    }
    case 0xCD: {
      // CMP, Absolute, 3 bytes, 4 cycles
      // TODO
      break;
    }
    case 0xDD: {
      // CMP, Absolute,X, 3 bytes, 4 (+1 if page crossed)
      // TODO
      break;
    }
    case 0xD9: {
      // CMP, Absolute,Y, 3 bytes, 4 (+1 if page crossed)
      // TODO
      break;
    }
    case 0xC1: {
      // CMP, (Indirect,X), 2 bytes, 6 cycles
      // TODO
      break;
    }
    case 0xD1: {
      // CMP, (Indirect),Y, 2 bytes, 5 (+1 if page crossed)
      // TODO
      break;
    }
    case 0xE0: {
      // CPX, Immediate, 2 bytes, 2 cycles
      // TODO
      break;
    }
    case 0xE4: {
      // CPX, Zero Page, 2 bytes, 3 cycles
      // TODO
      break;
    }
    case 0xEC: {
      // CPX, Absolute, 3 bytes, 4 cycles
      // TODO
      break;
    }
    case 0xC0: {
      // CPY, Immediate, 2 bytes, 2 cycles
      // TODO
      break;
    }
    case 0xC4: {
      // CPY, Zero Page, 2 bytes, 3 cycles
      // TODO
      break;
    }
    case 0xCC: {
      // CPY, Absolute, 3 bytes, 4 cycles
      // TODO
      break;
    }
    case 0xC6: {
      // DEC, Zero Page, 2 bytes, 5 cycles
      // TODO
      break;
    }
    case 0xD6: {
      // DEC, Zero Page,X, 2 bytes, 6 cycles
      // TODO
      break;
    }
    case 0xCE: {
      // DEC, Absolute, 3 bytes, 6 cycles
      // TODO
      break;
    }
    case 0xDE: {
      // DEC, Absolute,X, 3 bytes, 7 cycles
      // TODO
      break;
    }
    case 0xCA: {
      // DEX, Implied, 1 bytes, 2 cycles
      // TODO
      break;
    }
    case 0x88: {
      // DEY, Implied, 1 bytes, 2 cycles
      // TODO
      break;
    }
    case 0x49: {
      // EOR, Immediate, 2 bytes, 2 cycles
      // TODO
      break;
    }
    case 0x45: {
      // EOR, Zero Page, 2 bytes, 3 cycles
      // TODO
      break;
    }
    case 0x55: {
      // EOR, Zero Page,X, 2 bytes, 4 cycles
      // TODO
      break;
    }
    case 0x4D: {
      // EOR, Absolute, 3 bytes, 4 cycles
      // TODO
      break;
    }
    case 0x5D: {
      // EOR, Absolute,X, 3 bytes, 4 (+1 if page crossed)
      // TODO
      break;
    }
    case 0x59: {
      // EOR, Absolute,Y, 3 bytes, 4 (+1 if page crossed)
      // TODO
      break;
    }
    case 0x41: {
      // EOR, (Indirect,X), 2 bytes, 6 cycles
      // TODO
      break;
    }
    case 0x51: {
      // EOR, (Indirect),Y, 2 bytes, 5 (+1 if page crossed)
      // TODO
      break;
    }
    case 0xE6: {
      // INC, Zero Page, 2 bytes, 5 cycles
      // TODO
      break;
    }
    case 0xF6: {
      // INC, Zero Page,X, 2 bytes, 6 cycles
      // TODO
      break;
    }
    case 0xEE: {
      // INC, Absolute, 3 bytes, 6 cycles
      // TODO
      break;
    }
    case 0xFE: {
      // INC, Absolute,X, 3 bytes, 7 cycles
      // TODO
      break;
    }
    case 0xE8: {
      // INX, Implied, 1 bytes, 2 cycles
      // TODO
      break;
    }
    case 0xC8: {
      // INY, Implied, 1 bytes, 2 cycles
      // TODO
      break;
    }
    case 0x4C: {
      // JMP, Absolute, 3 bytes, 3 cycles
      // TODO
      break;
    }
    case 0x6C: {
      // JMP, Indirect, 3 bytes, 5 cycles
      // TODO
      break;
    }
    case 0x20: {
      // JSR, Absolute, 3 bytes, 6 cycles
      // TODO
      break;
    }
    case kLDA_IMM: {
      // LDA, Immediate, 2 bytes, 2 cycles
      A_ = value_fetch(kImmediate);
      PC_ += byte_count(kLDA_IMM);
      break;
    }
    case kLDA_ZP: {
      // LDA, Zero Page, 2 bytes, 3 cycles
      A_ = value_fetch(kZeroPage);
      PC_ += byte_count(kLDA_ZP);
      break;
    }
    case 0xB5: {
      // LDA, Zero Page,X, 2 bytes, 4 cycles
      // TODO
      break;
    }
    case 0xAD: {
      // LDA, Absolute, 3 bytes, 4 cycles
      // TODO
      break;
    }
    case 0xBD: {
      // LDA, Absolute,X, 3 bytes, 4 (+1 if page crossed)
      // TODO
      break;
    }
    case 0xB9: {
      // LDA, Absolute,Y, 3 bytes, 4 (+1 if page crossed)
      // TODO
      break;
    }
    case 0xA1: {
      // LDA, (Indirect,X), 2 bytes, 6 cycles
      // TODO
      break;
    }
    case 0xB1: {
      // LDA, (Indirect),Y, 2 bytes, 5 (+1 if page crossed)
      // TODO
      break;
    }
    case 0xA2: {
      // LDX, Immediate, 2 bytes, 2 cycles
      // TODO
      break;
    }
    case 0xA6: {
      // LDX, Zero Page, 2 bytes, 3 cycles
      // TODO
      break;
    }
    case 0xB6: {
      // LDX, Zero Page,Y, 2 bytes, 4 cycles
      // TODO
      break;
    }
    case 0xAE: {
      // LDX, Absolute, 3 bytes, 4 cycles
      // TODO
      break;
    }
    case 0xBE: {
      // LDX, Absolute,Y, 3 bytes, 4 (+1 if page crossed)
      // TODO
      break;
    }
    case 0xA0: {
      // LDY, Immediate, 2 bytes, 2 cycles
      // TODO
      break;
    }
    case 0xA4: {
      // LDY, Zero Page, 2 bytes, 3 cycles
      // TODO
      break;
    }
    case 0xB4: {
      // LDY, Zero Page,X, 2 bytes, 4 cycles
      // TODO
      break;
    }
    case 0xAC: {
      // LDY, Absolute, 3 bytes, 4 cycles
      // TODO
      break;
    }
    case 0xBC: {
      // LDY, Absolute,X, 3 bytes, 4 (+1 if page crossed)
      // TODO
      break;
    }
    case 0x4A: {
      // LSR, Accumulator, 1 bytes, 2 cycles
      // TODO
      break;
    }
    case 0x46: {
      // LSR, Zero Page, 2 bytes, 5 cycles
      // TODO
      break;
    }
    case 0x56: {
      // LSR, Zero Page,X, 2 bytes, 6 cycles
      // TODO
      break;
    }
    case 0x4E: {
      // LSR, Absolute, 3 bytes, 6 cycles
      // TODO
      break;
    }
    case 0x5E: {
      // LSR, Absolute,X, 3 bytes, 7 cycles
      // TODO
      break;
    }
    case 0xEA: {
      // NOP, Implied, 1 bytes, 2 cycles
      // TODO
      break;
    }
    case 0x09: {
      // ORA, Immediate, 2 bytes, 2 cycles
      // TODO
      break;
    }
    case 0x05: {
      // ORA, Zero Page, 2 bytes, 3 cycles
      // TODO
      break;
    }
    case 0x15: {
      // ORA, Zero Page,X, 2 bytes, 4 cycles
      // TODO
      break;
    }
    case 0x0D: {
      // ORA, Absolute, 3 bytes, 4 cycles
      // TODO
      break;
    }
    case 0x1D: {
      // ORA, Absolute,X, 3 bytes, 4 (+1 if page crossed)
      // TODO
      break;
    }
    case 0x19: {
      // ORA, Absolute,Y, 3 bytes, 4 (+1 if page crossed)
      // TODO
      break;
    }
    case 0x01: {
      // ORA, (Indirect,X), 2 bytes, 6 cycles
      // TODO
      break;
    }
    case 0x11: {
      // ORA, (Indirect),Y, 2 bytes, 5 (+1 if page crossed)
      // TODO
      break;
    }
    case 0x48: {
      // PHA, Implied, 1 bytes, 3 cycles
      // TODO
      break;
    }
    case 0x08: {
      // PHP, Implied, 1 bytes, 3 cycles
      // TODO
      break;
    }
    case 0x68: {
      // PLA, Implied, 1 bytes, 4 cycles
      // TODO
      break;
    }
    case 0x28: {
      // PLP, Implied, 1 bytes, 4 cycles
      // TODO
      break;
    }
    case 0x2A: {
      // ROL, Accumulator, 1 bytes, 2 cycles
      // TODO
      break;
    }
    case 0x26: {
      // ROL, Zero Page, 2 bytes, 5 cycles
      // TODO
      break;
    }
    case 0x36: {
      // ROL, Zero Page,X, 2 bytes, 6 cycles
      // TODO
      break;
    }
    case 0x2E: {
      // ROL, Absolute, 3 bytes, 6 cycles
      // TODO
      break;
    }
    case 0x3E: {
      // ROL, Absolute,X, 3 bytes, 7 cycles
      // TODO
      break;
    }
    case 0x6A: {
      // ROR, Accumulator, 1 bytes, 2 cycles
      // TODO
      break;
    }
    case 0x66: {
      // ROR, Zero Page, 2 bytes, 5 cycles
      // TODO
      break;
    }
    case 0x76: {
      // ROR, Zero Page,X, 2 bytes, 6 cycles
      // TODO
      break;
    }
    case 0x6E: {
      // ROR, Absolute, 3 bytes, 6 cycles
      // TODO
      break;
    }
    case 0x7E: {
      // ROR, Absolute,X, 3 bytes, 7 cycles
      // TODO
      break;
    }
    case 0x40: {
      // RTI, Implied, 1 bytes, 6 cycles
      // TODO
      break;
    }
    case 0x60: {
      // RTS, Implied, 1 bytes, 6 cycles
      // TODO
      break;
    }
    case 0xE9: {
      // SBC, Immediate, 2 bytes, 2 cycles
      // TODO
      break;
    }
    case 0xE5: {
      // SBC, Zero Page, 2 bytes, 3 cycles
      // TODO
      break;
    }
    case 0xF5: {
      // SBC, Zero Page,X, 2 bytes, 4 cycles
      // TODO
      break;
    }
    case 0xED: {
      // SBC, Absolute, 3 bytes, 4 cycles
      // TODO
      break;
    }
    case 0xFD: {
      // SBC, Absolute,X, 3 bytes, 4 (+1 if page crossed)
      // TODO
      break;
    }
    case 0xF9: {
      // SBC, Absolute,Y, 3 bytes, 4 (+1 if page crossed)
      // TODO
      break;
    }
    case 0xE1: {
      // SBC, (Indirect,X), 2 bytes, 6 cycles
      // TODO
      break;
    }
    case 0xF1: {
      // SBC, (Indirect),Y, 2 bytes, 5 (+1 if page crossed)
      // TODO
      break;
    }
    case 0x38: {
      // SEC, Implied, 1 bytes, 2 cycles
      // TODO
      break;
    }
    case 0xF8: {
      // SED, Implied, 1 bytes, 2 cycles
      // TODO
      break;
    }
    case 0x78: {
      // SEI, Implied, 1 bytes, 2 cycles
      // TODO
      break;
    }
    case kSTA_ZP: {
      // STA, Zero Page, 2 bytes, 3 cycles
      write(addr_fetch(kZeroPage), A_);
      PC_ += byte_count(kSTA_ZP);
      break;
    }
    case 0x95: {
      // STA, Zero Page,X, 2 bytes, 4 cycles
      // TODO
      break;
    }
    case 0x8D: {
      // STA, Absolute, 3 bytes, 4 cycles
      // TODO
      break;
    }
    case 0x9D: {
      // STA, Absolute,X, 3 bytes, 5 cycles
      // TODO
      break;
    }
    case 0x99: {
      // STA, Absolute,Y, 3 bytes, 5 cycles
      // TODO
      break;
    }
    case 0x81: {
      // STA, (Indirect,X), 2 bytes, 6 cycles
      // TODO
      break;
    }
    case 0x91: {
      // STA, (Indirect),Y, 2 bytes, 6 cycles
      // TODO
      break;
    }
    case 0x86: {
      // STX, Zero Page, 2 bytes, 3 cycles
      // TODO
      break;
    }
    case 0x96: {
      // STX, Zero Page,Y, 2 bytes, 4 cycles
      // TODO
      break;
    }
    case 0x8E: {
      // STX, Absolute, 3 bytes, 4 cycles
      // TODO
      break;
    }
    case 0x84: {
      // STY, Zero Page, 2 bytes, 3 cycles
      // TODO
      break;
    }
    case 0x94: {
      // STY, Zero Page,X, 2 bytes, 4 cycles
      // TODO
      break;
    }
    case 0x8C: {
      // STY, Absolute, 3 bytes, 4 cycles
      // TODO
      break;
    }
    case 0xAA: {
      // TAX, Implied, 1 bytes, 2 cycles
      // TODO
      break;
    }
    case 0xA8: {
      // TAY, Implied, 1 bytes, 2 cycles
      // TODO
      break;
    }
    case 0xBA: {
      // TSX, Implied, 1 bytes, 2 cycles
      // TODO
      break;
    }
    case 0x8A: {
      // TXA, Implied, 1 bytes, 2 cycles
      // TODO
      break;
    }
    case 0x9A: {
      // TXS, Implied, 1 bytes, 2 cycles
      // TODO
      break;
    }
    case 0x98: {
      // TYA, Implied, 1 bytes, 2 cycles
      // TODO
      break;
    }

    default:
      // BOOST_LOG_TRIVIAL(warning) << std::format("Unexpected opcode {}",
      // opcode);
      break;
  }
}

bool CPU::get_carry() { return (P_ & 0x1) > 0; }
void CPU::set_carry(bool value) {
  if (value) {
    P_ |= 0b00000001;
  } else {
    P_ &= 0b11111110;
  }
}
bool CPU::get_zero() { return (P_ & 0b00000010) > 0; }
void CPU::set_zero(bool value) {
  if (value) {
    P_ |= 0b00000010;
  } else {
    P_ &= 0b11111101;
  }
}

bool CPU::get_interrupt_disable() { return (P_ & 0b00000100) > 0; }
void CPU::set_interrupt_disable(bool value) {
  if (value) {
    P_ |= 0b00000100;
  } else {
    P_ &= 0b11111011;
  }
}

bool CPU::get_decimal() { return (P_ & 0b00001000) > 0; }
void CPU::set_decimal(bool value) {
  if (value) {
    P_ |= 0b00001000;
  } else {
    P_ &= 0b11110111;
  }
}
bool CPU::get_overflow() { return (P_ & 0b01000000) > 0; }
void CPU::set_overflow(bool value) {
  if (value) {
    P_ |= 0b01000000;
  } else {
    P_ &= 0b10111111;
  }
}
bool CPU::get_negative() { return (P_ & 0b10000000) > 0; }
void CPU::set_negative(bool value) {
  if (value) {
    P_ |= 0b10000000;
  } else {
    P_ &= 0b01111111;
  }
}

void CPU::ADC(uint8_t value) {
  A_ += value;

  set_carry(A_ < (static_cast<uint16_t>(A_) + value));
  set_zero(A_ == 0);
  // set_overflow( ??? );
  set_negative((A_ & 0b10000000) > 0);
}

void CPU::ASL_a() {
  set_carry((A_ & 0b10000000) > 0);
  A_ = A_ << 1;
}
void CPU::ASL_m(uint16_t addr) {
  uint8_t val = read(addr);
  set_carry((val & 0b10000000) > 0);
  val = val << 1;
  write(addr, val);
}
void CPU::AND(uint8_t other) { A_ = A_ & other; }

void CPU::BIT(AddrMode addressingMode) {
  uint8_t v = value_fetch(addressingMode);
  uint8_t r = A_ & v;

  set_zero(r == 0);
  set_overflow((r & 0b01000000) > 0);
  set_negative((r & 0b10000000) > 0);
}

void CPU::BRANCH(OpCode opcode, bool doBranch) {
  // we must the read value as a signed integer to support backwards jumps
  int8_t offset = static_cast<int8_t>(value_fetch(kRelative));
  // from the docs: "As the program counter itself is incremented during
  // instruction execution by two the effective address range for the target
  // instruction must be with -126 to +129 bytes of the branch."
  // i.e. we must increment the PC *before* applying the offset
  PC_ += byte_count(opcode);
  if (doBranch) {
    PC_ += offset;
  }
}

void CPU::LDA(uint8_t other) { A_ = other; }

}  // namespace cpu