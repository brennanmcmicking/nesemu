#ifndef CPU_HPP
#define CPU_HPP
#include <cstdint>

#include "cartridge.hpp"
#include "ppu.hpp"

namespace cpu {

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

  void cycle();
  void advance(std::size_t cycles);

 private:
  PPU& ppu_;
  Cartridge& cart_;

  uint16_t PC_;  // program counter. points to next instruction
  uint8_t SP_;   // stack pointer
  uint8_t A_;    // accumulator
  uint8_t X_;    // index register Y
  uint8_t Y_;    // index register Y
  uint8_t P_;    // processor status

  std::size_t cycles_todo_;  // cycles until the instruction is done
};

}  // namespace cpu
#endif  // CPU_HPP