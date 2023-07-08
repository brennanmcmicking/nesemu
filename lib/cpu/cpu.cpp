#include "cpu.hpp"

namespace cpu {

CPU::CPU(CPU::PPU& ppu, CPU::Cartridge& cartridge)
    : ppu_(ppu),
      cart_(cartridge),
      PC_(0),
      SP_(0),
      A_(0),
      X_(0),
      Y_(0),
      P_(0),
      cycles_todo_(0),
      ram_({}) {
  // read reset vector to get entry point
  PC_ = read(0xFFFC) | (read(0xFFFD) << 8);
};

uint16_t CPU::PC() const { return PC_; }
uint8_t CPU::SP() const { return SP_; }
uint8_t CPU::A() const { return A_; }
uint8_t CPU::X() const { return X_; }
uint8_t CPU::Y() const { return Y_; }
uint8_t CPU::P() const { return P_; }

}  // namespace cpu