#include <boost/log/trivial.hpp>
#include <format>

#include "cpu.hpp"

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
      /* TODO fill in the function for kBCC_REL
      uint16_t base_addr = PC_;
      uint16_t branch_addr = addr_fetch(kRelative);
      if (!BRANCH_INSTRUCTION()) return cycles;
      cycles += 1;
      if (crossed_page(base_addr, branch_addr)) cycles += 1;
      */
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
  return 0;  // by default, don't wait, just skip over invalid opcodes
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
    // TODO: is this a problem?
    BOOST_LOG_TRIVIAL(info) << "Step: previous command not finished cycling\n";
    // advance them manually (doesn't execute) //TODO: test
    advance_cycles(cycles_todo_);
  }

  // Advancing the current instruction's number of cycles with no cycles_todo_
  // is equivalent to executing the command and leaving no remaining cycles
  advance_cycles(cycles);
}

void CPU::advance_frame() {
  // TODO: PPU stuff for the frame
  // TODO: what does 0.5 cycles mean and how to deal with that?
  advance_cycles(static_cast<std::size_t>(kCyclesPerFrame));
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
    case kADC_INDX: {
      // ADC, (Indirect,X), 2 bytes, 6 cycles
      ADC(value_fetch(kIndexedIndirect));
      PC_ += byte_count(kADC_INDX);
      break;
    }
    case kADC_INDY: {
      // ADC, (Indirect),Y, 2 bytes, 5 (+1 if page crossed)
      ADC(value_fetch(kIndirectIndexed));
      PC_ += byte_count(kADC_INDY);
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
      AND(kIndexedIndirect);
      PC_ += byte_count(kAND_INDX);
      break;
    }
    case kAND_INDY: {
      // AND, (Indirect),Y, 2 bytes, 5 (+1 if page crossed)
      AND(kIndirectIndexed);
      PC_ += byte_count(kAND_INDY);
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
      PC_ += byte_count(kBIT_ZP);
      break;
    }
    case kBIT_ABS: {
      // BIT, Absolute, 3 bytes, 4 cycles
      BIT(kAbsolute);
      PC_ += byte_count(kBIT_ABS);
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
      PC_ += byte_count(kCLC);
      break;
    }
    case kCLD: {
      // CLD, Implied, 1 bytes, 2 cycles
      set_decimal(false);
      PC_ += byte_count(kCLD);
      break;
    }
    case kCLI: {
      // CLI, Implied, 1 bytes, 2 cycles
      set_interrupt_disable(false);
      break;
    }
    case kCLV: {
      // CLV, Implied, 1 bytes, 2 cycles
      set_overflow(false);
      PC_ += byte_count(kCLV);
      break;
    }
    case kCMP_IMM: {
      // CMP, Immediate, 2 bytes, 2 cycles
      CMP(A_, value_fetch(kImmediate));
      PC_ += byte_count(kCMP_IMM);
      break;
    }
    case kCMP_ZP: {
      // CMP, Zero Page, 2 bytes, 3 cycles
      CMP(A_, value_fetch(kZeroPage));
      PC_ += byte_count(kCMP_ZP);
      break;
    }
    case kCMP_ZPX: {
      // CMP, Zero Page,X, 2 bytes, 4 cycles
      CMP(A_, value_fetch(kZeroPageX));
      PC_ += byte_count(kCMP_ZPX);
      break;
    }
    case kCMP_ABS: {
      // CMP, Absolute, 3 bytes, 4 cycles
      CMP(A_, value_fetch(kAbsolute));
      PC_ += byte_count(kCMP_ABS);
      break;
    }
    case kCMP_ABSX: {
      // CMP, Absolute,X, 3 bytes, 4 (+1 if page crossed)
      CMP(A_, value_fetch(kAbsoluteX));
      PC_ += byte_count(kCMP_ABSX);
      break;
    }
    case kCMP_ABSY: {
      // CMP, Absolute,Y, 3 bytes, 4 (+1 if page crossed)
      CMP(A_, value_fetch(kAbsoluteY));
      PC_ += byte_count(kCMP_ABSY);
      break;
    }
    case kCMP_INDX: {
      // CMP, (Indirect,X), 2 bytes, 6 cycles
      CMP(A_, value_fetch(kIndexedIndirect));
      PC_ += byte_count(kCMP_INDX);
      break;
    }
    case kCMP_INDY: {
      // CMP, (Indirect),Y, 2 bytes, 5 (+1 if page crossed)
      CMP(A_, value_fetch(kIndirectIndexed));
      PC_ += byte_count(kCMP_INDY);
      break;
    }
    case kCPX_IMM: {
      // CPX, Immediate, 2 bytes, 2 cycles
      CMP(X_, value_fetch(kImmediate));
      PC_ += byte_count(kCPX_IMM);
      break;
    }
    case kCPX_ZP: {
      // CPX, Zero Page, 2 bytes, 3 cycles
      CMP(X_, value_fetch(kZeroPage));
      PC_ += byte_count(kCPX_ZP);
      break;
    }
    case kCPX_ABS: {
      // CPX, Absolute, 3 bytes, 4 cycles
      CMP(X_, value_fetch(kAbsolute));
      PC_ += byte_count(kCPX_ABS);
      break;
    }
    case kCPY_IMM: {
      // CPY, Immediate, 2 bytes, 2 cycles
      CMP(Y_, value_fetch(kImmediate));
      PC_ += byte_count(kCPY_IMM);
      break;
    }
    case kCPY_ZP: {
      // CPY, Zero Page, 2 bytes, 3 cycles
      CMP(Y_, value_fetch(kZeroPage));
      PC_ += byte_count(kCPY_ZP);
      break;
    }
    case kCPY_ABS: {
      // CPY, Absolute, 3 bytes, 4 cycles
      CMP(Y_, value_fetch(kAbsolute));
      PC_ += byte_count(kCPY_ABS);
      break;
    }
    case kDEC_ZP: {
      // DEC, Zero Page, 2 bytes, 5 cycles
      DEC(kZeroPage);
      PC_ += byte_count(kDEC_ZP);
      break;
    }
    case kDEC_ZPX: {
      // DEC, Zero Page,X, 2 bytes, 6 cycles
      DEC(kZeroPageX);
      PC_ += byte_count(kDEC_ZPX);
      break;
    }
    case kDEC_ABS: {
      // DEC, Absolute, 3 bytes, 6 cycles
      DEC(kAbsolute);
      PC_ += byte_count(kDEC_ABS);
      break;
    }
    case kDEC_ABSX: {
      // DEC, Absolute,X, 3 bytes, 7 cycles
      DEC(kAbsoluteX);
      PC_ += byte_count(kDEC_ABSX);
      break;
    }
    case kDEX: {
      // DEX, Implied, 1 bytes, 2 cycles
      X_ -= 1;
      set_zero(X_ == 0);
      set_negative((X_ & 0b10000000) > 0);
      PC_ += byte_count(kDEX);
      break;
    }
    case kDEY: {
      // DEY, Implied, 1 bytes, 2 cycles
      Y_ -= 1;
      set_zero(Y_ == 0);
      set_negative((Y_ & 0b10000000) > 0);
      break;
    }
    case kEOR_IMM: {
      // EOR, Immediate, 2 bytes, 2 cycles
      EOR(kImmediate);
      PC_ += byte_count(kEOR_IMM);
      break;
    }
    case kEOR_ZP: {
      // EOR, Zero Page, 2 bytes, 3 cycles
      EOR(kZeroPage);
      PC_ += byte_count(kEOR_ZP);
      break;
    }
    case kEOR_ZPX: {
      // EOR, Zero Page,X, 2 bytes, 4 cycles
      EOR(kZeroPageX);
      PC_ += byte_count(kEOR_ZPX);
      break;
    }
    case kEOR_ABS: {
      // EOR, Absolute, 3 bytes, 4 cycles
      EOR(kAbsolute);
      PC_ += byte_count(kEOR_ABS);
      break;
    }
    case kEOR_ABSX: {
      // EOR, Absolute,X, 3 bytes, 4 (+1 if page crossed)
      EOR(kAbsoluteX);
      PC_ += byte_count(kEOR_ABSX);
      break;
    }
    case kEOR_ABSY: {
      // EOR, Absolute,Y, 3 bytes, 4 (+1 if page crossed)
      EOR(kAbsoluteY);
      PC_ += byte_count(kEOR_ABSY);
      break;
    }
    case kEOR_INDX: {
      // EOR, (Indirect,X), 2 bytes, 6 cycles
      EOR(kIndexedIndirect);
      PC_ += byte_count(kEOR_INDX);
      break;
    }
    case kEOR_INDY: {
      // EOR, (Indirect),Y, 2 bytes, 5 (+1 if page crossed)
      EOR(kIndirectIndexed);
      PC_ += byte_count(kEOR_INDY);
      break;
    }
    case kINC_ZP: {
      // INC, Zero Page, 2 bytes, 5 cycles
      INC(kZeroPage);
      PC_ += byte_count(kINC_ZP);
      break;
    }
    case kINC_ZPX: {
      // INC, Zero Page,X, 2 bytes, 6 cycles
      INC(kZeroPageX);
      PC_ += byte_count(kINC_ZPX);
      break;
    }
    case kINC_ABS: {
      // INC, Absolute, 3 bytes, 6 cycles
      INC(kAbsolute);
      PC_ += byte_count(kINC_ABS);
      break;
    }
    case kINC_ABSX: {
      // INC, Absolute,X, 3 bytes, 7 cycles
      INC(kAbsoluteX);
      PC_ += byte_count(kINC_ABSX);
      break;
    }
    case kINX: {
      // INX, Implied, 1 bytes, 2 cycles
      X_ += 1;
      set_zero(X_ == 0);
      set_negative((X_ & 0b10000000) > 0);
      PC_ += byte_count(kINX);
      break;
    }
    case kINY: {
      // INY, Implied, 1 bytes, 2 cycles
      Y_ -= 1;
      set_zero(Y_ == 0);
      set_negative((Y_ & 0b10000000) > 0);
      PC_ += byte_count(kINY);
      break;
    }
    case kJMP_ABS: {
      // JMP, Absolute, 3 bytes, 3 cycles
      PC_ = addr_fetch(kAbsolute);
      break;
    }
    case kJMP_IND: {
      // JMP, Indirect, 3 bytes, 5 cycles
      PC_ = value_fetch(kIndirect);
      break;
    }
    case kJSR_ABS: {
      // JSR, Absolute, 3 bytes, 6 cycles
      push_stack16(PC_ + byte_count(kJSR_ABS) - 1);
      PC_ = addr_fetch(kAbsolute);
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
    case kLDA_ZPX: {
      // LDA, Zero Page,X, 2 bytes, 4 cycles
      A_ = value_fetch(kZeroPageX);
      PC_ += byte_count(kLDA_ZPX);
      break;
    }
    case kLDA_ABS: {
      // LDA, Absolute, 3 bytes, 4 cycles
      A_ = value_fetch(kAbsolute);
      PC_ += byte_count(kLDA_ABS);
      break;
    }
    case kLDA_ABSX: {
      // LDA, Absolute,X, 3 bytes, 4 (+1 if page crossed)
      A_ = value_fetch(kAbsoluteX);
      PC_ += byte_count(kLDA_ABSX);
      break;
    }
    case kLDA_ABSY: {
      // LDA, Absolute,Y, 3 bytes, 4 (+1 if page crossed)
      A_ = value_fetch(kAbsoluteY);
      PC_ += byte_count(kLDA_ABSY);
      break;
    }
    case kLDA_INDX: {
      // LDA, (Indirect,X), 2 bytes, 6 cycles
      A_ = value_fetch(kIndexedIndirect);
      PC_ += byte_count(kLDA_INDX);
      break;
    }
    case kLDA_INDY: {
      // LDA, (Indirect),Y, 2 bytes, 5 (+1 if page crossed)
      A_ = value_fetch(kIndirectIndexed);
      PC_ += byte_count(kLDA_INDY);
      break;
    }
    case kLDX_IMM: {
      // LDX, Immediate, 2 bytes, 2 cycles
      X_ = value_fetch(kImmediate);
      PC_ += byte_count(kLDX_IMM);
      break;
    }
    case kLDX_ZP: {
      // LDX, Zero Page, 2 bytes, 3 cycles
      X_ = value_fetch(kZeroPage);
      PC_ += byte_count(kLDX_ZP);
      break;
    }
    case kLDX_ZPY: {
      // LDX, Zero Page,Y, 2 bytes, 4 cycles
      X_ = value_fetch(kZeroPageY);
      PC_ += byte_count(kLDX_ZPY);
      break;
    }
    case kLDX_ABS: {
      // LDX, Absolute, 3 bytes, 4 cycles
      X_ = value_fetch(kAbsolute);
      PC_ += byte_count(kLDX_ABS);
      break;
    }
    case kLDX_ABSY: {
      // LDX, Absolute,Y, 3 bytes, 4 (+1 if page crossed)
      X_ = value_fetch(kAbsoluteY);
      PC_ += byte_count(kLDX_ABSY);
      break;
    }
    case kLDY_IMM: {
      // LDY, Immediate, 2 bytes, 2 cycles
      Y_ = value_fetch(kImmediate);
      PC_ += byte_count(kLDY_IMM);
      break;
    }
    case kLDY_ZP: {
      // LDY, Zero Page, 2 bytes, 3 cycles
      Y_ = value_fetch(kZeroPage);
      PC_ += byte_count(kLDY_ZP);
      break;
    }
    case kLDY_ZPX: {
      // LDY, Zero Page,X, 2 bytes, 4 cycles
      Y_ = value_fetch(kZeroPageX);
      PC_ += byte_count(kLDY_ZPX);
      break;
    }
    case kLDY_ABS: {
      // LDY, Absolute, 3 bytes, 4 cycles
      Y_ = value_fetch(kAbsolute);
      PC_ += byte_count(kLDY_ABS);
      break;
    }
    case kLDY_ABSX: {
      // LDY, Absolute,X, 3 bytes, 4 (+1 if page crossed)
      Y_ = value_fetch(kAbsoluteX);
      PC_ += byte_count(kLDY_ABSX);
      break;
    }
    case kLSR_A: {
      // LSR, Accumulator, 1 bytes, 2 cycles
      LSR_a();
      PC_ += byte_count(kLSR_A);
      break;
    }
    case kLSR_ZP: {
      // LSR, Zero Page, 2 bytes, 5 cycles
      LSR_m(kZeroPage);
      PC_ += byte_count(kLSR_ZP);
      break;
    }
    case kLSR_ZPX: {
      // LSR, Zero Page,X, 2 bytes, 6 cycles
      LSR_m(kZeroPageX);
      PC_ += byte_count(kLSR_ZPX);
      break;
    }
    case kLSR_ABS: {
      // LSR, Absolute, 3 bytes, 6 cycles
      LSR_m(kAbsolute);
      PC_ += byte_count(kLSR_ABS);
      break;
    }
    case kLSR_ABSX: {
      // LSR, Absolute,X, 3 bytes, 7 cycles
      LSR_m(kAbsoluteX);
      PC_ += byte_count(kLSR_ABSX);
      break;
    }
    case kNOP: {
      // NOP, Implied, 1 bytes, 2 cycles
      PC_ += byte_count(kNOP);
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
    case kSEC: {
      // SEC, Implied, 1 bytes, 2 cycles
      set_carry(true);
      PC_ += byte_count(kSEC);
      break;
    }
    case kSED: {
      // SED, Implied, 1 bytes, 2 cycles
      set_decimal(true);
      PC_ += byte_count(kSED);
      break;
    }
    case kSEI: {
      // SEI, Implied, 1 bytes, 2 cycles
      set_interrupt_disable(true);
      PC_ += byte_count(kSEI);
      break;
    }
    case kSTA_ZP: {
      // STA, Zero Page, 2 bytes, 3 cycles
      STA(kZeroPage);
      PC_ += byte_count(kSTA_ZP);
      break;
    }
    case kSTA_ZPX: {
      // STA, Zero Page,X, 2 bytes, 4 cycles
      STA(kZeroPageX);
      PC_ += byte_count(kSTA_ZPX);
      break;
    }
    case kSTA_ABS: {
      // STA, Absolute, 3 bytes, 4 cycles
      STA(kAbsolute);
      PC_ += byte_count(kSTA_ABS);
      break;
    }
    case kSTA_ABSX: {
      // STA, Absolute,X, 3 bytes, 5 cycles
      STA(kAbsoluteX);
      PC_ += byte_count(kSTA_ABSX);
      break;
    }
    case kSTA_ABSY: {
      // STA, Absolute,Y, 3 bytes, 5 cycles
      STA(kAbsoluteY);
      PC_ += byte_count(kSTA_ABSY);
      break;
    }
    case kSTA_INDX: {
      // STA, (Indirect,X), 2 bytes, 6 cycles
      STA(kIndexedIndirect);
      PC_ += byte_count(kSTA_INDX);
      break;
    }
    case kSTA_INDY: {
      // STA, (Indirect),Y, 2 bytes, 6 cycles
      STA(kIndirectIndexed);
      PC_ += byte_count(kSTA_INDY);
      break;
    }
    case kSTX_ZP: {
      // STX, Zero Page, 2 bytes, 3 cycles
      STX(kZeroPage);
      PC_ += byte_count(kSTX_ZP);
      break;
    }
    case kSTX_ZPY: {
      // STX, Zero Page,Y, 2 bytes, 4 cycles
      STX(kZeroPageY);
      PC_ += byte_count(kSTX_ZPY);
      break;
    }
    case kSTX_ABS: {
      // STX, Absolute, 3 bytes, 4 cycles
      STX(kAbsolute);
      PC_ += byte_count(kSTX_ABS);
      break;
    }
    case kSTY_ZP: {
      // STY, Zero Page, 2 bytes, 3 cycles
      STY(kZeroPage);
      PC_ += byte_count(kSTX_ZP);
      break;
    }
    case kSTY_ZPX: {
      // STY, Zero Page,X, 2 bytes, 4 cycles
      STY(kZeroPageY);
      PC_ += byte_count(kSTY_ZPX);
      break;
    }
    case kSTY_ABS: {
      // STY, Absolute, 3 bytes, 4 cycles
      STY(kAbsolute);
      PC_ += byte_count(kSTY_ABS);
      break;
    }
    case kTAX: {
      // TAX, Implied, 1 bytes, 2 cycles
      X_ = A_;
      set_zero(X_ == 0);
      set_negative((X_ & 0b10000000) > 0);
      break;
    }
    case kTAY: {
      // TAY, Implied, 1 bytes, 2 cycles
      Y_ = A_;
      set_zero(Y_ == 0);
      set_negative((Y_ & 0b10000000) > 0);
      break;
    }
    case kTSX: {
      // TSX, Implied, 1 bytes, 2 cycles
      X_ = SP_;
      set_zero(X_ == 0);
      set_negative((X_ & 0b10000000) > 0);
      break;
    }
    case kTXA: {
      // TXA, Implied, 1 bytes, 2 cycles
      A_ = X_;
      set_zero(A_ == 0);
      set_negative((A_ & 0b10000000) > 0);
      break;
    }
    case kTXS: {
      // TXS, Implied, 1 bytes, 2 cycles
      SP_ = X_;
      break;
    }
    case kTYA: {
      // TYA, Implied, 1 bytes, 2 cycles
      A_ = Y_;
      set_zero(A_ == 0);
      set_negative((A_ & 0b10000000) > 0);
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
  if (get_carry()) {
    A_ |= 0b10000000;
    set_negative(true);
  }
}

void CPU::LSR_m(AddrMode addressingMode) {
  uint16_t addr = addr_fetch(addressingMode);
  uint8_t val = read(addr);  // could use value_fetch here too but not necessary
  set_carry((val & 0b1) == 1);
  val = val >> 1;
  set_zero(val == 0);
  if (get_carry()) {
    val |= 0b10000000;
    set_negative(true);
  }
  write(addr, val);
}

void CPU::ORA() {}

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