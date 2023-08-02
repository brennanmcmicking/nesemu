#include <boost/log/trivial.hpp>
#include <cstdint>

#include "cpu.hpp"

namespace cpu {

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
    case kBRK: {
      // BRK, Implied, 1 bytes, 7 cycles
      push_stack16(PC_);
      push_stack16(P_);
      PC_ = read16(0xFFFE);
      set_break(true);
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
      set_zero(A_ == 0);
      set_negative((A_ & 0b10000000) > 0);
      break;
    }
    case kLDA_ZP: {
      // LDA, Zero Page, 2 bytes, 3 cycles
      A_ = value_fetch(kZeroPage);
      PC_ += byte_count(kLDA_ZP);
      set_zero(A_ == 0);
      set_negative((A_ & 0b10000000) > 0);
      break;
    }
    case kLDA_ZPX: {
      // LDA, Zero Page,X, 2 bytes, 4 cycles
      A_ = value_fetch(kZeroPageX);
      PC_ += byte_count(kLDA_ZPX);
      set_zero(A_ == 0);
      set_negative((A_ & 0b10000000) > 0);
      break;
    }
    case kLDA_ABS: {
      // LDA, Absolute, 3 bytes, 4 cycles
      A_ = value_fetch(kAbsolute);
      PC_ += byte_count(kLDA_ABS);
      set_zero(A_ == 0);
      set_negative((A_ & 0b10000000) > 0);
      break;
    }
    case kLDA_ABSX: {
      // LDA, Absolute,X, 3 bytes, 4 (+1 if page crossed)
      A_ = value_fetch(kAbsoluteX);
      PC_ += byte_count(kLDA_ABSX);
      set_zero(A_ == 0);
      set_negative((A_ & 0b10000000) > 0);
      break;
    }
    case kLDA_ABSY: {
      // LDA, Absolute,Y, 3 bytes, 4 (+1 if page crossed)
      A_ = value_fetch(kAbsoluteY);
      PC_ += byte_count(kLDA_ABSY);
      set_zero(A_ == 0);
      set_negative((A_ & 0b10000000) > 0);
      break;
    }
    case kLDA_INDX: {
      // LDA, (Indirect,X), 2 bytes, 6 cycles
      A_ = value_fetch(kIndexedIndirect);
      PC_ += byte_count(kLDA_INDX);
      set_zero(A_ == 0);
      set_negative((A_ & 0b10000000) > 0);
      break;
    }
    case kLDA_INDY: {
      // LDA, (Indirect),Y, 2 bytes, 5 (+1 if page crossed)
      A_ = value_fetch(kIndirectIndexed);
      PC_ += byte_count(kLDA_INDY);
      set_zero(A_ == 0);
      set_negative((A_ & 0b10000000) > 0);
      break;
    }
    case kLDX_IMM: {
      // LDX, Immediate, 2 bytes, 2 cycles
      X_ = value_fetch(kImmediate);
      PC_ += byte_count(kLDX_IMM);
      set_zero(X_ == 0);
      set_negative((X_ & 0b10000000) > 0);
      break;
    }
    case kLDX_ZP: {
      // LDX, Zero Page, 2 bytes, 3 cycles
      X_ = value_fetch(kZeroPage);
      PC_ += byte_count(kLDX_ZP);
      set_zero(X_ == 0);
      set_negative((X_ & 0b10000000) > 0);
      break;
    }
    case kLDX_ZPY: {
      // LDX, Zero Page,Y, 2 bytes, 4 cycles
      X_ = value_fetch(kZeroPageY);
      PC_ += byte_count(kLDX_ZPY);
      set_zero(X_ == 0);
      set_negative((X_ & 0b10000000) > 0);
      break;
    }
    case kLDX_ABS: {
      // LDX, Absolute, 3 bytes, 4 cycles
      X_ = value_fetch(kAbsolute);
      PC_ += byte_count(kLDX_ABS);
      set_zero(X_ == 0);
      set_negative((X_ & 0b10000000) > 0);
      break;
    }
    case kLDX_ABSY: {
      // LDX, Absolute,Y, 3 bytes, 4 (+1 if page crossed)
      X_ = value_fetch(kAbsoluteY);
      PC_ += byte_count(kLDX_ABSY);
      set_zero(X_ == 0);
      set_negative((X_ & 0b10000000) > 0);
      break;
    }
    case kLDY_IMM: {
      // LDY, Immediate, 2 bytes, 2 cycles
      Y_ = value_fetch(kImmediate);
      PC_ += byte_count(kLDY_IMM);
      set_zero(Y_ == 0);
      set_negative((Y_ & 0b10000000) > 0);
      break;
    }
    case kLDY_ZP: {
      // LDY, Zero Page, 2 bytes, 3 cycles
      Y_ = value_fetch(kZeroPage);
      PC_ += byte_count(kLDY_ZP);
      set_zero(Y_ == 0);
      set_negative((Y_ & 0b10000000) > 0);
      break;
    }
    case kLDY_ZPX: {
      // LDY, Zero Page,X, 2 bytes, 4 cycles
      Y_ = value_fetch(kZeroPageX);
      PC_ += byte_count(kLDY_ZPX);
      set_zero(Y_ == 0);
      set_negative((Y_ & 0b10000000) > 0);
      break;
    }
    case kLDY_ABS: {
      // LDY, Absolute, 3 bytes, 4 cycles
      Y_ = value_fetch(kAbsolute);
      PC_ += byte_count(kLDY_ABS);
      set_zero(Y_ == 0);
      set_negative((Y_ & 0b10000000) > 0);
      break;
    }
    case kLDY_ABSX: {
      // LDY, Absolute,X, 3 bytes, 4 (+1 if page crossed)
      Y_ = value_fetch(kAbsoluteX);
      PC_ += byte_count(kLDY_ABSX);
      set_zero(Y_ == 0);
      set_negative((Y_ & 0b10000000) > 0);
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
    case kORA_IMM: {
      // ORA, Immediate, 2 bytes, 2 cycles
      ORA(kImmediate);
      PC_ += byte_count(kORA_IMM);
      break;
    }
    case kORA_ZP: {
      // ORA, Zero Page, 2 bytes, 3 cycles
      ORA(kZeroPage);
      PC_ += byte_count(kORA_ZP);
      break;
    }
    case kORA_ZPX: {
      // ORA, Zero Page,X, 2 bytes, 4 cycles
      ORA(kZeroPageX);
      PC_ += byte_count(kORA_ZPX);
      break;
    }
    case kORA_ABS: {
      // ORA, Absolute, 3 bytes, 4 cycles
      ORA(kAbsolute);
      PC_ += byte_count(kORA_ABS);
      break;
    }
    case kORA_ABSX: {
      // ORA, Absolute,X, 3 bytes, 4 (+1 if page crossed)
      ORA(kAbsoluteX);
      PC_ += byte_count(kORA_ABSX);
      break;
    }
    case kORA_ABSY: {
      // ORA, Absolute,Y, 3 bytes, 4 (+1 if page crossed)
      ORA(kAbsoluteY);
      PC_ += byte_count(kORA_ABSY);
      break;
    }
    case kORA_INDX: {
      // ORA, (Indirect,X), 2 bytes, 6 cycles
      ORA(kIndexedIndirect);
      PC_ += byte_count(kORA_INDX);
      break;
    }
    case kORA_INDY: {
      // ORA, (Indirect),Y, 2 bytes, 5 (+1 if page crossed)
      ORA(kIndirectIndexed);
      PC_ += byte_count(kORA_INDY);
      break;
    }
    case kPHA: {
      // PHA, Implied, 1 bytes, 3 cycles
      push_stack(A_);
      PC_ += byte_count(kPHA);
      break;
    }
    case kPHP: {
      // PHP, Implied, 1 bytes, 3 cycles
      push_stack16(P_);
      PC_ += byte_count(kPHP);
      break;
    }
    case kPLA: {
      // PLA, Implied, 1 bytes, 4 cycles
      A_ = pop_stack();
      set_zero(A_ == 0);
      set_negative((A_ & 0b10000000) > 0);
      PC_ += byte_count(kPLA);
      break;
    }
    case kPLP: {
      // PLP, Implied, 1 bytes, 4 cycles
      P_ = pop_stack();
      set_zero(P_ == 0);
      set_negative((P_ & 0b10000000) > 0);
      PC_ += byte_count(kPLP);
      break;
    }
    case kROL_A: {
      // ROL, Accumulator, 1 bytes, 2 cycles
      bool old_carry = get_carry();
      set_carry((A_ & 0b10000000) > 0);
      A_ = A_ << 1;
      A_ |= old_carry ? 1 : 0;
      PC_ += byte_count(kROL_A);
      break;
    }
    case kROL_ZP: {
      // ROL, Zero Page, 2 bytes, 5 cycles
      ROL_m(kZeroPage);
      PC_ += byte_count(kROL_ZP);
      break;
    }
    case kROL_ZPX: {
      // ROL, Zero Page,X, 2 bytes, 6 cycles
      ROL_m(kZeroPageX);
      PC_ += byte_count(kROL_ZPX);
      break;
    }
    case kROL_ABS: {
      // ROL, Absolute, 3 bytes, 6 cycles
      ROL_m(kAbsolute);
      PC_ += byte_count(kROL_ABS);
      break;
    }
    case kROL_ABSX: {
      // ROL, Absolute,X, 3 bytes, 7 cycles
      ROL_m(kAbsoluteX);
      PC_ += byte_count(kROL_ABSX);
      break;
    }
    case kROR_A: {
      // ROR, Accumulator, 1 bytes, 2 cycles
      bool old_carry = get_carry();
      set_carry((A_ & 0b00000001) > 0);
      A_ = A_ >> 1;
      A_ |= old_carry ? 0b10000000 : 0;
      PC_ += byte_count(kROR_A);
      break;
    }
    case kROR_ZP: {
      // ROR, Zero Page, 2 bytes, 5 cycles
      ROR_m(kZeroPage);
      PC_ += byte_count(kROR_ZP);
      break;
    }
    case kROR_ZPX: {
      // ROR, Zero Page,X, 2 bytes, 6 cycles
      ROR_m(kZeroPageX);
      PC_ += byte_count(kROR_ZPX);
      break;
    }
    case kROR_ABS: {
      // ROR, Absolute, 3 bytes, 6 cycles
      ROR_m(kAbsolute);
      PC_ += byte_count(kROR_ABS);
      break;
    }
    case kROR_ABSX: {
      // ROR, Absolute,X, 3 bytes, 7 cycles
      ROR_m(kAbsoluteX);
      PC_ += byte_count(kROR_ABSX);
      break;
    }
    case kRTI: {
      // RTI, Implied, 1 bytes, 6 cycles
      P_ = pop_stack16();
      PC_ = pop_stack16();
      // when returning from interrupt we do not increment the PC
      break;
    }
    case kRTS: {
      // RTS, Implied, 1 bytes, 6 cycles
      PC_ = pop_stack();
      // JSR stores the byte before the next memory location before jumping
      // so we need to add 1 to the value from the stack to get the true
      // next instruction location
      PC_ += byte_count(kRTS);
      break;
    }
    case kSBC_IMM: {
      // SBC, Immediate, 2 bytes, 2 cycles
      SBC(kImmediate);
      PC_ += byte_count(kSBC_IMM);
      break;
    }
    case kSBC_ZP: {
      // SBC, Zero Page, 2 bytes, 3 cycles
      SBC(kZeroPage);
      PC_ += byte_count(kSBC_ZP);
      break;
    }
    case kSBC_ZPX: {
      // SBC, Zero Page,X, 2 bytes, 4 cycles
      SBC(kZeroPageX);
      PC_ += byte_count(kSBC_ZPX);
      break;
    }
    case kSBC_ABS: {
      // SBC, Absolute, 3 bytes, 4 cycles
      SBC(kAbsolute);
      PC_ += byte_count(kSBC_ABS);
      break;
    }
    case kSBC_ABSX: {
      // SBC, Absolute,X, 3 bytes, 4 (+1 if page crossed)
      SBC(kAbsoluteX);
      PC_ += byte_count(kSBC_ABSX);
      break;
    }
    case kSBC_ABSY: {
      // SBC, Absolute,Y, 3 bytes, 4 (+1 if page crossed)
      SBC(kAbsoluteY);
      PC_ += byte_count(kSBC_ABSY);
      break;
    }
    case kSBC_INDX: {
      // SBC, (Indirect,X), 2 bytes, 6 cycles
      SBC(kIndexedIndirect);
      PC_ += byte_count(kSBC_INDX);
      break;
    }
    case kSBC_INDY: {
      // SBC, (Indirect),Y, 2 bytes, 5 (+1 if page crossed)
      SBC(kIndirectIndexed);
      PC_ += byte_count(kSBC_INDY);
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
      STY(kZeroPageX);
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
      PC_ += byte_count(kTAX);
      break;
    }
    case kTAY: {
      // TAY, Implied, 1 bytes, 2 cycles
      Y_ = A_;
      set_zero(Y_ == 0);
      set_negative((Y_ & 0b10000000) > 0);
      PC_ += byte_count(kTAY);
      break;
    }
    case kTSX: {
      // TSX, Implied, 1 bytes, 2 cycles
      X_ = SP_;
      set_zero(X_ == 0);
      set_negative((X_ & 0b10000000) > 0);
      PC_ += byte_count(kTSX);
      break;
    }
    case kTXA: {
      // TXA, Implied, 1 bytes, 2 cycles
      A_ = X_;
      set_zero(A_ == 0);
      set_negative((A_ & 0b10000000) > 0);
      PC_ += byte_count(kTXA);
      break;
    }
    case kTXS: {
      // TXS, Implied, 1 bytes, 2 cycles
      SP_ = X_;
      PC_ += byte_count(kTXS);
      break;
    }
    case kTYA: {
      // TYA, Implied, 1 bytes, 2 cycles
      A_ = Y_;
      set_zero(A_ == 0);
      set_negative((A_ & 0b10000000) > 0);
      PC_ += byte_count(kTYA);
      break;
    }

    default:
      // BOOST_LOG_TRIVIAL(warning) << std::format("Unexpected opcode {}",
      // opcode);
      break;
  }
}

}  // namespace cpu