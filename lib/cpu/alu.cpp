#include <boost/log/trivial.hpp>
#include <format>

#include "cpu.hpp"
#include "util.hpp"

namespace cpu {

// detect whether a page boundary was crossed when addr is incremented by offset
bool crossed_page(uint16_t addr1, uint16_t addr2) {
  // pages in the NES are 256 bytes large
  // we only need to compare the upper 8 bits of the address
  return (addr1 >> 8) != (addr2 >> 8);
}

std::size_t CPU::cycle_count(uint8_t opcode) {
  switch (opcode) {
    case kADC_IMM:
      return 2;
    case kADC_ZP:
      return 3;
    case kADC_ZPX:
      return 4;
    case kADC_ABS:
      return 4;
    case kADC_ABSX: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kAbsolute);
      uint16_t index_addr = addr_fetch(kAbsoluteX);
      std::size_t cycles = 4;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kADC_ABSY: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kAbsolute);
      uint16_t index_addr = addr_fetch(kAbsoluteY);
      std::size_t cycles = 4;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kADC_INDX:
      return 6;
    case kADC_INDY: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kIndirect);
      uint16_t index_addr = addr_fetch(kIndirectIndexed);
      std::size_t cycles = 5;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kAND_IMM:
      return 2;
    case kAND_ZP:
      return 3;
    case kAND_ZPX:
      return 4;
    case kAND_ABS:
      return 4;
    case kAND_ABSX: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kAbsolute);
      uint16_t index_addr = addr_fetch(kAbsoluteX);
      std::size_t cycles = 4;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kAND_ABSY: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kAbsolute);
      uint16_t index_addr = addr_fetch(kAbsoluteY);
      std::size_t cycles = 4;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kAND_INDX:
      return 6;
    case kAND_INDY: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kIndirect);
      uint16_t index_addr = addr_fetch(kIndirectIndexed);
      std::size_t cycles = 5;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kASL_A:
      return 2;
    case kASL_ZP:
      return 5;
    case kASL_ZPX:
      return 6;
    case kASL_ABS:
      return 6;
    case kASL_ABSX:
      return 7;
    case kBCC_REL: {  // branching instruction
      std::size_t cycles = 2;
      // TODO fill in the function for kBCC_REL
      // uint16_t base_addr = PC_;
      // uint16_t branch_addr = addr_fetch(kRelative);
      // if (!()) return cycles;
      // cycles += 1;
      // if (crossed_page(base_addr, branch_addr)) cycles += 1;
      return cycles;
    }
    case kBCS_REL: {  // branching instruction
      std::size_t cycles = 2;
      /* TODO fill in the function for kBCS_REL
      uint16_t base_addr = PC_;
      uint16_t branch_addr = addr_fetch(kRelative);
      if (!BRANCH_INSTRUCTION()) return cycles;
      cycles += 1;
      if (crossed_page(base_addr, branch_addr)) cycles += 1;
      */
      return cycles;
    }
    case kBEQ_REL: {  // branching instruction
      std::size_t cycles = 2;
      /* TODO fill in the function for kBEQ_REL
      uint16_t base_addr = PC_;
      uint16_t branch_addr = addr_fetch(kRelative);
      if (!BRANCH_INSTRUCTION()) return cycles;
      cycles += 1;
      if (crossed_page(base_addr, branch_addr)) cycles += 1;
      */
      return cycles;
    }
    case kBIT_ZP:
      return 3;
    case kBIT_ABS:
      return 4;
    case kBMI_REL: {  // branching instruction
      std::size_t cycles = 2;
      /* TODO fill in the function for kBMI_REL
      uint16_t base_addr = PC_;
      uint16_t branch_addr = addr_fetch(kRelative);
      if (!BRANCH_INSTRUCTION()) return cycles;
      cycles += 1;
      if (crossed_page(base_addr, branch_addr)) cycles += 1;
      */
      return cycles;
    }
    case kBNE_REL: {  // branching instruction
      std::size_t cycles = 2;
      /* TODO fill in the function for kBNE_REL
      uint16_t base_addr = PC_;
      uint16_t branch_addr = addr_fetch(kRelative);
      if (!BRANCH_INSTRUCTION()) return cycles;
      cycles += 1;
      if (crossed_page(base_addr, branch_addr)) cycles += 1;
      */
      return cycles;
    }
    case kBPL_REL: {  // branching instruction
      std::size_t cycles = 2;
      /* TODO fill in the function for kBPL_REL
      uint16_t base_addr = PC_;
      uint16_t branch_addr = addr_fetch(kRelative);
      if (!BRANCH_INSTRUCTION()) return cycles;
      cycles += 1;
      if (crossed_page(base_addr, branch_addr)) cycles += 1;
      */
      return cycles;
    }
    case kBRK:
      return 7;
    case kBVC_REL: {  // branching instruction
      std::size_t cycles = 2;
      /* TODO fill in the function for kBVC_REL
      uint16_t base_addr = PC_;
      uint16_t branch_addr = addr_fetch(kRelative);
      if (!BRANCH_INSTRUCTION()) return cycles;
      cycles += 1;
      if (crossed_page(base_addr, branch_addr)) cycles += 1;
      */
      return cycles;
    }
    case kBVS_REL: {  // branching instruction
      std::size_t cycles = 2;
      /* TODO fill in the function for kBVS_REL
      uint16_t base_addr = PC_;
      uint16_t branch_addr = addr_fetch(kRelative);
      if (!BRANCH_INSTRUCTION()) return cycles;
      cycles += 1;
      if (crossed_page(base_addr, branch_addr)) cycles += 1;
      */
      return cycles;
    }
    case kCLC:
      return 2;
    case kCLD:
      return 2;
    case kCLI:
      return 2;
    case kCLV:
      return 2;
    case kCMP_IMM:
      return 2;
    case kCMP_ZP:
      return 3;
    case kCMP_ZPX:
      return 4;
    case kCMP_ABS:
      return 4;
    case kCMP_ABSX: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kAbsolute);
      uint16_t index_addr = addr_fetch(kAbsoluteX);
      std::size_t cycles = 4;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kCMP_ABSY: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kAbsolute);
      uint16_t index_addr = addr_fetch(kAbsoluteY);
      std::size_t cycles = 4;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kCMP_INDX:
      return 6;
    case kCMP_INDY: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kIndirect);
      uint16_t index_addr = addr_fetch(kIndirectIndexed);
      std::size_t cycles = 5;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kCPX_IMM:
      return 2;
    case kCPX_ZP:
      return 3;
    case kCPX_ABS:
      return 4;
    case kCPY_IMM:
      return 2;
    case kCPY_ZP:
      return 3;
    case kCPY_ABS:
      return 4;
    case kDEC_ZP:
      return 5;
    case kDEC_ZPX:
      return 6;
    case kDEC_ABS:
      return 6;
    case kDEC_ABSX:
      return 7;
    case kDEX:
      return 2;
    case kDEY:
      return 2;
    case kEOR_IMM:
      return 2;
    case kEOR_ZP:
      return 3;
    case kEOR_ZPX:
      return 4;
    case kEOR_ABS:
      return 4;
    case kEOR_ABSX: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kAbsolute);
      uint16_t index_addr = addr_fetch(kAbsoluteX);
      std::size_t cycles = 4;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kEOR_ABSY: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kAbsolute);
      uint16_t index_addr = addr_fetch(kAbsoluteY);
      std::size_t cycles = 4;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kEOR_INDX:
      return 6;
    case kEOR_INDY: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kIndirect);
      uint16_t index_addr = addr_fetch(kIndirectIndexed);
      std::size_t cycles = 5;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kINC_ZP:
      return 5;
    case kINC_ZPX:
      return 6;
    case kINC_ABS:
      return 6;
    case kINC_ABSX:
      return 7;
    case kINX:
      return 2;
    case kINY:
      return 2;
    case kJMP_ABS:
      return 3;
    case kJMP_IND:
      return 5;
    case kJSR_ABS:
      return 6;
    case kLDA_IMM:
      return 2;
    case kLDA_ZP:
      return 3;
    case kLDA_ZPX:
      return 4;
    case kLDA_ABS:
      return 4;
    case kLDA_ABSX: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kAbsolute);
      uint16_t index_addr = addr_fetch(kAbsoluteX);
      std::size_t cycles = 4;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kLDA_ABSY: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kAbsolute);
      uint16_t index_addr = addr_fetch(kAbsoluteY);
      std::size_t cycles = 4;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kLDA_INDX:
      return 6;
    case kLDA_INDY: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kIndirect);
      uint16_t index_addr = addr_fetch(kIndirectIndexed);
      std::size_t cycles = 5;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kLDX_IMM:
      return 2;
    case kLDX_ZP:
      return 3;
    case kLDX_ZPY:
      return 4;
    case kLDX_ABS:
      return 4;
    case kLDX_ABSY: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kAbsolute);
      uint16_t index_addr = addr_fetch(kAbsoluteY);
      std::size_t cycles = 4;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kLDY_IMM:
      return 2;
    case kLDY_ZP:
      return 3;
    case kLDY_ZPX:
      return 4;
    case kLDY_ABS:
      return 4;
    case kLDY_ABSX: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kAbsolute);
      uint16_t index_addr = addr_fetch(kAbsoluteX);
      std::size_t cycles = 4;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kLSR_A:
      return 2;
    case kLSR_ZP:
      return 5;
    case kLSR_ZPX:
      return 6;
    case kLSR_ABS:
      return 6;
    case kLSR_ABSX:
      return 7;
    case kNOP:
      return 2;
    case kORA_IMM:
      return 2;
    case kORA_ZP:
      return 3;
    case kORA_ZPX:
      return 4;
    case kORA_ABS:
      return 4;
    case kORA_ABSX: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kAbsolute);
      uint16_t index_addr = addr_fetch(kAbsoluteX);
      std::size_t cycles = 4;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kORA_ABSY: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kAbsolute);
      uint16_t index_addr = addr_fetch(kAbsoluteY);
      std::size_t cycles = 4;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kORA_INDX:
      return 6;
    case kORA_INDY: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kIndirect);
      uint16_t index_addr = addr_fetch(kIndirectIndexed);
      std::size_t cycles = 5;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kPHA:
      return 3;
    case kPHP:
      return 3;
    case kPLA:
      return 4;
    case kPLP:
      return 4;
    case kROL_A:
      return 2;
    case kROL_ZP:
      return 5;
    case kROL_ZPX:
      return 6;
    case kROL_ABS:
      return 6;
    case kROL_ABSX:
      return 7;
    case kROR_A:
      return 2;
    case kROR_ZP:
      return 5;
    case kROR_ZPX:
      return 6;
    case kROR_ABS:
      return 6;
    case kROR_ABSX:
      return 7;
    case kRTI:
      return 6;
    case kRTS:
      return 6;
    case kSBC_IMM:
      return 2;
    case kSBC_ZP:
      return 3;
    case kSBC_ZPX:
      return 4;
    case kSBC_ABS:
      return 4;
    case kSBC_ABSX: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kAbsolute);
      uint16_t index_addr = addr_fetch(kAbsoluteX);
      std::size_t cycles = 4;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kSBC_ABSY: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kAbsolute);
      uint16_t index_addr = addr_fetch(kAbsoluteY);
      std::size_t cycles = 4;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kSBC_INDX:
      return 6;
    case kSBC_INDY: {  // indexed addressing instruction
      uint16_t base_addr = addr_fetch(kIndirect);
      uint16_t index_addr = addr_fetch(kIndirectIndexed);
      std::size_t cycles = 5;
      if (crossed_page(base_addr, index_addr)) cycles += 1;
      return cycles;
    }
    case kSEC:
      return 2;
    case kSED:
      return 2;
    case kSEI:
      return 2;
    case kSTA_ZP:
      return 3;
    case kSTA_ZPX:
      return 4;
    case kSTA_ABS:
      return 4;
    case kSTA_ABSX:
      return 5;
    case kSTA_ABSY:
      return 5;
    case kSTA_INDX:
      return 6;
    case kSTA_INDY:
      return 6;
    case kSTX_ZP:
      return 3;
    case kSTX_ZPY:
      return 4;
    case kSTX_ABS:
      return 4;
    case kSTY_ZP:
      return 3;
    case kSTY_ZPX:
      return 4;
    case kSTY_ABS:
      return 4;
    case kTAX:
      return 2;
    case kTAY:
      return 2;
    case kTSX:
      return 2;
    case kTXA:
      return 2;
    case kTXS:
      return 2;
    case kTYA:
      return 2;
  }
  throw std::runtime_error(std::format("Invalid opcode: {} (PC = {})",
                                       util::fmt_hex(opcode),
                                       util::fmt_hex(PC_)));
}

void CPU::advance_cycles(std::size_t cycles) {
  for (std::size_t i = 0; i < cycles; i++) {
    cycle();
  }
}

void CPU::advance_instruction() {
  uint8_t instruction = read(PC_);
  auto cycles = cycle_count(instruction);

  if (cycles_todo_ != 0) {
    BOOST_LOG_TRIVIAL(trace) << "Advancing one instruction before NMI";
    advance_cycles(cycles_todo_);
  }

  // Advancing the current instruction's number of cycles with no cycles_todo_
  // is equivalent to executing the command and leaving no remaining cycles
  advance_cycles(cycles);
}

void CPU::advance_frame() {
  if (ppu_.has_value()) {
    BOOST_LOG_TRIVIAL(debug) << "Rendering frame";
    // Render a frame using the current cpu data
    ppu_->get().render_to_window();
  }

  // Want to advance the cycles regardless of if ppu attached
  advance_cycles(kRenderCycles);

  if (ppu_.has_value() && ppu_->get().is_nmi_enabled()) {
    // BOOST_LOG_TRIVIAL(info) << "nmi enabled";
    trigger_nmi();
  } else
    // BOOST_LOG_TRIVIAL(info) << "nmi disabled";

    // TODO: what does 0.5 cycles mean and how to deal with that?
    advance_cycles(kVBlankCycles);
}

void CPU::cycle() {
  // BOOST_LOG_TRIVIAL(trace) << "cycle()";
  // BOOST_LOG_TRIVIAL(trace) << std::format(
  //     "PC: {}, SP: {}, A: {}, X: {}, Y: {}, P: {}", util::fmt_hex(PC_),
  //     util::fmt_hex(SP_), util::fmt_hex(A_), util::fmt_hex(X_),
  //     util::fmt_hex(Y_), util::fmt_hex(P_));
  // BOOST_LOG_TRIVIAL(trace) << std::format("cycles_todo: {}", cycles_todo_);

  if (cycles_todo_ == 1) {
    // BOOST_LOG_TRIVIAL(trace) << std::format(
    //     "PC: {}, SP: {}, A: {}, X: {}, Y: {}, P: {}", util::fmt_hex(PC_),
    //     util::fmt_hex(SP_), util::fmt_hex(A_), util::fmt_hex(X_),
    //     util::fmt_hex(Y_), util::fmt_hex(P_));
    // fetch, decode, execute
    execute(read(PC_));
  } else if (cycles_todo_ == 0) {
    // BOOST_LOG_TRIVIAL(trace) << "fetching next instruction";
    cycles_todo_ = cycle_count(read(PC_));
    // BOOST_LOG_TRIVIAL(trace)
    //     << std::format("next_instr: {}", print_instruction());
  }
  cycles_todo_--;
}

void CPU::trigger_nmi() {
  // Program finishes current instruction before the interrupt happens
  if (cycles_todo_ > 1) {
    advance_cycles(cycles_todo_ - 1);
  }
  push_stack16(PC_);
  push_stack(P_);
  PC_ = read16(0xFFFA);
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
bool CPU::get_break() { return P_ & 0b00010000; }
void CPU::set_break(bool value) {
  if (value) {
    P_ |= 0b00010000;
  } else {
    P_ &= 0b11101111;
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

bool detectFlowAddition(uint8_t a, uint8_t b, uint8_t result) {
  bool aNeg = (a & 0b10000000) != 0;
  bool bNeg = (b & 0b10000000) != 0;
  if (aNeg != bNeg) {
    // If the signs are different, overflow cannot occur.
    return false;
  }

  // Then, check if the sign of the result is different from the signs of a and
  // b.
  bool resultNeg = (result & 0b10000000) != 0;
  bool differentSigns = (aNeg != resultNeg) && (bNeg != resultNeg);

  // If the signs are the same and the result has a different sign, overflow
  // occurred.
  return differentSigns;
}

bool detectFlowSubtraction(uint8_t a, uint8_t b, uint8_t result) {
  bool aNeg = (a & 0b10000000) != 0;
  bool bNeg = (b & 0b10000000) != 0;
  if (aNeg && bNeg) {
    // If the signs are the same, overflow cannot occur.
    return false;
  }

  // - - + = +
  // + - - = -

  bool resultNeg = (result & 0b1000'0000) != 0;
  if (aNeg && !bNeg) {
    return !resultNeg;
  }

  if (!aNeg && bNeg) {
    return resultNeg;
  }

  return false;
}

void CPU::ADC(uint8_t value) {
  uint16_t newA_ = A_ + value + get_carry();

  set_overflow(detectFlowAddition(A_, value, newA_));
  set_carry((newA_ >> 8) != 0);
  set_zero((newA_ & 0xFF) == 0);
  set_negative((newA_ & 0b10000000) > 0);

  A_ = newA_ & 0xFF;
}

void CPU::ASL_a() {
  set_carry((A_ & 0b10000000) > 0);

  A_ = A_ << 1;

  set_zero(A_ == 0);
  set_negative((A_ & 0b10000000) > 0);
}
void CPU::ASL_m(uint16_t addr) {
  uint8_t val = read(addr);

  set_carry((val & 0b10000000) > 0);

  val = val << 1;

  set_zero(val == 0);
  set_negative((val & 0b10000000) > 0);

  write(addr, val);
}
void CPU::AND(uint8_t other) {
  A_ = A_ & other;

  set_zero(A_ == 0);
  set_negative((A_ & 0b10000000) > 0);
}

void CPU::BIT(AddrMode addressingMode) {
  uint8_t v = value_fetch(addressingMode);
  uint8_t r = A_ & v;

  set_zero(r == 0);
  set_overflow((v & 0b01000000) > 0);
  set_negative((v & 0b10000000) > 0);
}

void CPU::BRANCH(OpCode opcode, bool doBranch) {
  // we must the read value as a signed integer to support backwards jumps
  // uint8_t unsigned_offset = value_fetch(kRelative);
  int8_t offset = static_cast<int8_t>(value_fetch(kRelative));
  // from the docs: "As the program counter itself is incremented during
  // instruction execution by two the effective address range for the target
  // instruction must be with -126 to +129 bytes of the branch."
  // i.e. we must increment the PC *before* applying the offset
  PC_ += byte_count(opcode);
  if (doBranch) {
    // BOOST_LOG_TRIVIAL(trace)
    //     << "Branch succeeded, offset: " << signed(offset)
    //     << ", unsigned offset: " << unsigned(unsigned_offset);
    PC_ += offset;
  }
}

void CPU::CMP(uint8_t reg, uint8_t other) {
  set_carry(reg >= other);
  set_zero(reg == other);
  set_negative(reg < other);
}

void CPU::DEC(AddrMode addressingMode) {
  uint16_t addr = addr_fetch(addressingMode);
  uint8_t res = read(addr) - 1;
  write(addr, res);
  set_zero(res == 0);
  set_negative((res & 0b10000000) > 0);
}

void CPU::DEX() {
  X_ -= 1;
  set_zero(X_ == 0);
  set_negative((X_ & 0b10000000) > 0);
}

void CPU::EOR(AddrMode addressingMode) {
  uint8_t v = value_fetch(addressingMode);
  A_ ^= v;
  set_zero(A_ == 0);
  set_negative((A_ & 0b10000000) > 0);
}

void CPU::INC(AddrMode addressingMode) {
  uint16_t addr = addr_fetch(addressingMode);
  uint8_t res = read(addr) + 1;
  write(addr, res);
  set_zero(res == 0);
  set_negative((res & 0b10000000) > 0);
}

void CPU::INX() {
  X_ += 1;
  set_zero(X_ == 0);
  set_negative((X_ & 0b10000000) > 0);
}

void CPU::LSR_a() {
  set_carry((A_ & 0b1) == 1);
  A_ = A_ >> 1;
  set_zero(A_ == 0);
  set_negative(false);
}

void CPU::LSR_m(AddrMode addressingMode) {
  uint16_t addr = addr_fetch(addressingMode);
  uint8_t val = read(addr);  // could use value_fetch here too but not necessary
  set_carry((val & 0b1) == 1);
  val = val >> 1;
  set_zero(val == 0);
  set_negative(false);
  write(addr, val);
}

void CPU::ORA(AddrMode addressingMode) {
  A_ |= value_fetch(addressingMode);
  set_zero(A_ == 0);
  set_negative((A_ & 0b10000000) > 0);
}

void CPU::ROL_m(AddrMode addressingMode) {
  bool old_carry = get_carry();
  uint16_t addr = addr_fetch(addressingMode);
  uint8_t val = read(addr);
  set_carry((val & 0b10000000) > 0);
  val = val << 1;
  val |= old_carry ? 1 : 0;
  set_zero(val == 0);
  set_negative((val & 0b10000000) > 0);
  write(addr, val);
}

void CPU::ROR_m(AddrMode addressingMode) {
  bool old_carry = get_carry();
  uint16_t addr = addr_fetch(addressingMode);
  uint8_t val = read(addr);
  set_carry((val & 0b00000001) > 0);
  val = val >> 1;
  val |= old_carry ? 0b10000000 : 0;
  set_zero(A_ == 0);
  set_negative((A_ & 0b10000000) > 0);
  write(addr, val);
}

void CPU::SBC(AddrMode addressingMode) {
  uint8_t val = value_fetch(addressingMode);
  uint16_t newA_ = A_ - (val + !get_carry());

  set_overflow(detectFlowSubtraction(A_, val, newA_));
  set_carry((newA_ >> 8) != 0);
  set_zero((newA_ & 0xFF) == 0);
  set_negative((newA_ & 0b10000000) > 0);

  A_ = newA_ & 0xFF;
}

void CPU::STA(AddrMode addressingMode) {
  write(addr_fetch(addressingMode), A_);
}

void CPU::STX(AddrMode addressingMode) {
  write(addr_fetch(addressingMode), X_);
}

void CPU::STY(AddrMode addressingMode) {
  write(addr_fetch(addressingMode), Y_);
}

}  // namespace cpu