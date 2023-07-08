#include "cpu.hpp"

namespace cpu {
uint8_t CPU::read(uint16_t addr) {
  if (addr < 0x2000) {
    return ram_[addr % 0x800];
  } else if (addr < 0x4000) {
    switch (addr % 8) {};
    return 0xAA;  // TODO ppu
  } else if (addr < 0x4020) {
    return 0xAA;  // TODO io
  } else {
    return cart_.cpu_read(addr);
  }
};

bool CPU::write(uint16_t addr, uint8_t data) {
  if (addr < 0x2000) {
    ram_[addr % 0x800] = data;
    return true;
  } else if (addr < 0x4000) {
    switch (addr % 8) {};
    return false;  // TODO ppu
  } else if (addr < 0x4020) {
    return false;  // TODO io
  } else {
    // cart_.cpu_write(addr, data);
    return false;  // for now, writing is not allowed
  }
  return true;
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

}  // namespace cpu