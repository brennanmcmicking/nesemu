#ifndef CPU_HPP
#define CPU_HPP
#include <array>
#include <chrono>
#include <cstdint>

#include "cartridge.hpp"
#include "ppu.hpp"

// Forward declaration to deal with circular dependency
namespace debugger {
class Debugger;
}

namespace cpu {

// syntax:
// #x = immediate
// x, y = addition
// (x) = indirection/dereference
//
// for everything except immediate or accumulator, the value at the address
enum AddrMode {
  // 8-bit (value)
  kAccumulator,  // A
  kImmediate,    // #$00
  // 16-bit (address)
  kZeroPage,         // $00
  kZeroPageX,        // $00, X (zero page wraparound)
  kZeroPageY,        // $00, Y (zero page wraparound)
  kRelative,         // $0000 (signed)
  kAbsolute,         // $0000
  kAbsoluteX,        // $0000, X
  kAbsoluteY,        // $0000, Y
  kIndirect,         // ($0000)
  kIndexedIndirect,  // ($00, X) (zero page wraparound)
  kIndirectIndexed,  // ($00), Y
};

enum OpCode : uint8_t;

class CPU {
  // Debugger needs access to internal data to read/write memory and registers
  friend class debugger::Debugger;

 public:
  using Cartridge = cartridge::Cartridge;
  using PPU = ppu::PPU;

  const double kFramerate = 60.0988;              // Hz
  const double kTimePerFrame = 1.0 / kFramerate;  // seconds
  const std::chrono::duration<double, std::milli> kTimePerFrameMillis{
      kTimePerFrame * 1000};               // Milliseconds
  const double kCyclesPerFrame = 29780.5;  // taken from wiki

  explicit CPU(PPU& ppu, Cartridge& cartridge);

  CPU() = delete;
  CPU(CPU&) = delete;
  CPU(CPU&&) = delete;
  CPU& operator=(CPU&) = delete;
  CPU operator=(CPU&&) = delete;

  uint16_t PC() const;
  uint8_t SP() const;
  uint8_t A() const;
  uint8_t X() const;
  uint8_t Y() const;
  uint8_t P() const;

  /**
   * @brief Runs a blocking infinite loop
   *
   * Once this function is run, it will loop until the program terminates. It
   * handles running the clock at a consistent speed, invoking various
   * components of the system at appropriate times, and sleeping in between
   * cycles.
   */
  void begin_cpu_loop();

  /**
   * @brief Executes the next cycle of the ALU.
   *
   * Most (if not all) instructions take more than 1 cycle to execute. To
   * accurately emulate CPU behaviour, the ALU will execute the entire
   * functionality of the instruction in the first cycle and then NOP for
   * the remaining cycles of that instruction.
   */
  void cycle();

  /**
   * @brief Executes multiple cycles sequentially.
   */
  void advance_cycles(std::size_t cycles);

  /**
   * @brief Executes the next instruction.
   */
  void advance_instruction();

  /**
   * @brief Renders a frame, then advances a frame's worth of CPU cycles.
   */
  void advance_frame();

  /**
   * @brief Reads the value in the absolute memory address
   *
   * The given address is forwarded to the relevant component
   * based on the table in the [NesDEV Memory Map Wiki Page]
   * (https://www.nesdev.org/wiki/CPU_memory_map)
   *
   * @param addr The absolute address to read
   * @return uint8_t The value at that location
   */
  uint8_t read(uint16_t addr);
  // convenience function for reading 16-bit values
  uint16_t read16(uint16_t addr);

  /**
   * @brief Writes the given value to the absolute memory address
   *
   * The given address and value are forwarded to the relevant component
   * based on the table in the [NesDEV Memory Map Wiki Page]
   * (https://www.nesdev.org/wiki/CPU_memory_map)
   *
   * Instead of throwing an exception, a boolean value of false will be returned
   * if an attempt is made to write to a read-only section of memory (i.e.
   * controller state).
   *
   * @param addr The address to write to
   * @param value The value to write to it
   * @return true Success
   * @return false Failure
   */
  bool write(uint16_t addr, uint8_t value);
  // convenience function for writing 16-bit values
  bool write16(uint16_t addr, uint16_t value);

 private:
  PPU& ppu_;
  Cartridge& cart_;

  // True iff the program was started in debug mode. If true, the program will
  // stop execution on breakpoints //TODO: make that happen
  bool debug_mode_;

  /**
   * @brief Execute the instruction with given opcode
   *
   * @param opcode
   */
  void execute(uint8_t opcode);

  uint16_t PC_;  // program counter. points to next instruction
  uint8_t SP_;   // stack pointer
  uint8_t A_;    // accumulator
  uint8_t X_;    // index register Y
  uint8_t Y_;    // index register Y
  uint8_t P_;    // processor status

  std::size_t cycles_todo_;  // cycles until the instruction is done
  uint8_t next_instr_;

  std::array<uint8_t, 0x800> ram_;  // 2kb of RAM

  std::size_t cycle_count(uint8_t opcode);
  std::size_t byte_count(uint8_t opcode);

  uint16_t addr_fetch(AddrMode mode);

  uint8_t value_fetch(AddrMode mode);

  /**
   * @brief Pushes a byte onto the stack and updates the stack pointer
   *
   * Complains if SP is invalid
   *
   * @param value the byte to put
   */
  void push_stack(uint8_t value);
  void push_stack16(uint16_t value);

  /**
   * @brief Pops the top byte off of the stack and decrements the stack pointer
   *
   * Complains if SP is invalid
   *
   * @return uint8_t
   */
  uint8_t pop_stack();
  uint16_t pop_stack16();

  /**
   * @brief Peeks the top of the stack without decrementing
   *
   * Does not complain if SP is invalid
   *
   * @return uint8_t
   */
  uint8_t peek_stack();
  uint16_t peek_stack16();

  bool get_carry();
  void set_carry(bool value);
  bool get_zero();
  void set_zero(bool value);
  bool get_interrupt_disable();
  void set_interrupt_disable(bool value);
  bool get_decimal();
  void set_decimal(bool value);
  bool get_overflow();
  void set_overflow(bool value);
  bool get_negative();
  void set_negative(bool value);

  void ADC(uint8_t value);
  void AND(uint8_t other);
  void ASL_a();
  void ASL_m(uint16_t addr);
  void BIT(AddrMode addressingMode);
  void BRANCH(enum OpCode opcode, bool doBranch);
  void CMP(uint8_t reg, uint8_t other);
  void DEC(AddrMode addressingMode);
  void DEX();
  void EOR(AddrMode addressingMode);
  void INC(AddrMode addressingMode);
  void INX();
  void LDA(uint8_t other);
  void LSR_a();
  void LSR_m(AddrMode addressingMode);
  void ORA();
  void STA(AddrMode addressingMode);
  void STX(AddrMode addressingMode);
  void STY(AddrMode addressingMode);
};

// generated using scripts/scrape-opcodes.py
// name is of the form <instruction>_<addressing mode>
// e.g. ADC_IMM is the ADC instruction with immediate addressing mode
enum OpCode : uint8_t {
  kADC_IMM = 0x69,
  kADC_ZP = 0x65,
  kADC_ZPX = 0x75,
  kADC_ABS = 0x6D,
  kADC_ABSX = 0x7D,
  kADC_ABSY = 0x79,
  kADC_INDX = 0x61,
  kADC_INDY = 0x71,
  kAND_IMM = 0x29,
  kAND_ZP = 0x25,
  kAND_ZPX = 0x35,
  kAND_ABS = 0x2D,
  kAND_ABSX = 0x3D,
  kAND_ABSY = 0x39,
  kAND_INDX = 0x21,
  kAND_INDY = 0x31,
  kASL_A = 0x0A,
  kASL_ZP = 0x06,
  kASL_ZPX = 0x16,
  kASL_ABS = 0x0E,
  kASL_ABSX = 0x1E,
  kBCC_REL = 0x90,
  kBCS_REL = 0xB0,
  kBEQ_REL = 0xF0,
  kBIT_ZP = 0x24,
  kBIT_ABS = 0x2C,
  kBMI_REL = 0x30,
  kBNE_REL = 0xD0,
  kBPL_REL = 0x10,
  kBRK = 0x00,
  kBVC_REL = 0x50,
  kBVS_REL = 0x70,
  kCLC = 0x18,
  kCLD = 0xD8,
  kCLI = 0x58,
  kCLV = 0xB8,
  kCMP_IMM = 0xC9,
  kCMP_ZP = 0xC5,
  kCMP_ZPX = 0xD5,
  kCMP_ABS = 0xCD,
  kCMP_ABSX = 0xDD,
  kCMP_ABSY = 0xD9,
  kCMP_INDX = 0xC1,
  kCMP_INDY = 0xD1,
  kCPX_IMM = 0xE0,
  kCPX_ZP = 0xE4,
  kCPX_ABS = 0xEC,
  kCPY_IMM = 0xC0,
  kCPY_ZP = 0xC4,
  kCPY_ABS = 0xCC,
  kDEC_ZP = 0xC6,
  kDEC_ZPX = 0xD6,
  kDEC_ABS = 0xCE,
  kDEC_ABSX = 0xDE,
  kDEX = 0xCA,
  kDEY = 0x88,
  kEOR_IMM = 0x49,
  kEOR_ZP = 0x45,
  kEOR_ZPX = 0x55,
  kEOR_ABS = 0x4D,
  kEOR_ABSX = 0x5D,
  kEOR_ABSY = 0x59,
  kEOR_INDX = 0x41,
  kEOR_INDY = 0x51,
  kINC_ZP = 0xE6,
  kINC_ZPX = 0xF6,
  kINC_ABS = 0xEE,
  kINC_ABSX = 0xFE,
  kINX = 0xE8,
  kINY = 0xC8,
  kJMP_ABS = 0x4C,
  kJMP_IND = 0x6C,
  kJSR_ABS = 0x20,
  kLDA_IMM = 0xA9,
  kLDA_ZP = 0xA5,
  kLDA_ZPX = 0xB5,
  kLDA_ABS = 0xAD,
  kLDA_ABSX = 0xBD,
  kLDA_ABSY = 0xB9,
  kLDA_INDX = 0xA1,
  kLDA_INDY = 0xB1,
  kLDX_IMM = 0xA2,
  kLDX_ZP = 0xA6,
  kLDX_ZPY = 0xB6,
  kLDX_ABS = 0xAE,
  kLDX_ABSY = 0xBE,
  kLDY_IMM = 0xA0,
  kLDY_ZP = 0xA4,
  kLDY_ZPX = 0xB4,
  kLDY_ABS = 0xAC,
  kLDY_ABSX = 0xBC,
  kLSR_A = 0x4A,
  kLSR_ZP = 0x46,
  kLSR_ZPX = 0x56,
  kLSR_ABS = 0x4E,
  kLSR_ABSX = 0x5E,
  kNOP = 0xEA,
  kORA_IMM = 0x09,
  kORA_ZP = 0x05,
  kORA_ZPX = 0x15,
  kORA_ABS = 0x0D,
  kORA_ABSX = 0x1D,
  kORA_ABSY = 0x19,
  kORA_INDX = 0x01,
  kORA_INDY = 0x11,
  kPHA = 0x48,
  kPHP = 0x08,
  kPLA = 0x68,
  kPLP = 0x28,
  kROL_A = 0x2A,
  kROL_ZP = 0x26,
  kROL_ZPX = 0x36,
  kROL_ABS = 0x2E,
  kROL_ABSX = 0x3E,
  kROR_A = 0x6A,
  kROR_ZP = 0x66,
  kROR_ZPX = 0x76,
  kROR_ABS = 0x6E,
  kROR_ABSX = 0x7E,
  kRTI = 0x40,
  kRTS = 0x60,
  kSBC_IMM = 0xE9,
  kSBC_ZP = 0xE5,
  kSBC_ZPX = 0xF5,
  kSBC_ABS = 0xED,
  kSBC_ABSX = 0xFD,
  kSBC_ABSY = 0xF9,
  kSBC_INDX = 0xE1,
  kSBC_INDY = 0xF1,
  kSEC = 0x38,
  kSED = 0xF8,
  kSEI = 0x78,
  kSTA_ZP = 0x85,
  kSTA_ZPX = 0x95,
  kSTA_ABS = 0x8D,
  kSTA_ABSX = 0x9D,
  kSTA_ABSY = 0x99,
  kSTA_INDX = 0x81,
  kSTA_INDY = 0x91,
  kSTX_ZP = 0x86,
  kSTX_ZPY = 0x96,
  kSTX_ABS = 0x8E,
  kSTY_ZP = 0x84,
  kSTY_ZPX = 0x94,
  kSTY_ABS = 0x8C,
  kTAX = 0xAA,
  kTAY = 0xA8,
  kTSX = 0xBA,
  kTXA = 0x8A,
  kTXS = 0x9A,
  kTYA = 0x98,
};

}  // namespace cpu
#endif  // CPU_HPP