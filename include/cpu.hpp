#ifndef CPU_HPP
#define CPU_HPP
#include <array>
#include <cstdint>

#include "cartridge.hpp"
#include "ppu.hpp"

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

class CPU {
 public:
  using Cartridge = cartridge::Cartridge;
  using PPU = ppu::PPU;

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
   * @brief Executes the next cycle of the ALU.
   *
   * Most (if not all) instructions take more than 1 cycle to execute. To
   * accurately emulate CPU behaviour, the ALU will execute the entire
   * functionality of the instruction in the first cycle and then NOP for
   * the remaining cycles of that instruction.
   */
  void cycle();
  void advance(std::size_t cycles);

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
  void write16(uint16_t addr, uint16_t value);

 private:
  PPU& ppu_;
  Cartridge& cart_;

  /**
   * @brief TODO: write docstring
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

  template <AddrMode M>
  uint16_t addr_fetch();

  template <AddrMode M>
  uint8_t value_fetch() {
    return read(addr_fetch<M>());
  }

  bool get_carry();
  void set_carry(bool value);
  bool get_zero();
  void set_zero(bool value);
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
  void LDA(uint8_t other);
  void STA(uint16_t addr);
};

}  // namespace cpu
#endif  // CPU_HPP