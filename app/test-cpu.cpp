#include <catch2/catch.hpp>
#include <cstdint>
#include <vector>

#include "cartridge.hpp"
#include "cpu.hpp"
#include "ppu.hpp"

using namespace cpu;

class VectorMapper : public cartridge::Mapper {
 public:
  VectorMapper(std::vector<uint8_t> bytecode, uint16_t entrypoint = 0x8000u)
      : bytes_(bytecode), entrypoint_(entrypoint){};
  uint8_t prg_read(uint16_t addr) override {
    if (addr < 0x8000) {
      return 0xAA;
    } else if (addr == 0xFFFC) {
      return entrypoint_ & 0xFF;
    } else if (addr == 0xFFFD) {
      return entrypoint_ >> 8;
    }
    addr -= 0x8000;
    if (addr >= bytes_.size()) {
      return 0xAA;
    }
    return bytes_.at(addr);
  };
  void prg_write(uint16_t addr, uint8_t data) override {
    (void)addr;  // explicitly unused
    (void)data;
  };

 private:
  std::vector<uint8_t> bytes_;
  uint16_t entrypoint_;
};

#define U16(x) ((x)&0xFF), ((x) >> 8)

#define MAKE_CPU(bytecode)                                            \
  std::unique_ptr<VectorMapper> __mapper(new VectorMapper(bytecode)); \
  cartridge::Cartridge __cart(std::move(__mapper));                   \
  ppu::PPU __ppu;                                                     \
  CPU cpu(__ppu, __cart);

TEST_CASE("trivial load and store") {
  // clang-format off
  std::vector<uint8_t> bytecode = {
      // LDA #$01
      kLDA_IMM, 0x01,
      // STA $0000
      kSTA_ZP, 0x00,
      // LDA #$02
      kLDA_IMM, 0x02,
      // LDA $0000
      kLDA_ZP, 0x00, 
  };  // clang-format on

  MAKE_CPU(bytecode);

  REQUIRE(cpu.PC() == 0x8000);
  REQUIRE(cpu.A() == 0);

  // LDA #$01
  cpu.cycle();
  REQUIRE(cpu.PC() == 0x8000);
  REQUIRE(cpu.A() == 0);

  cpu.cycle();
  REQUIRE(cpu.PC() == 0x8002);
  REQUIRE(cpu.A() == 1);

  REQUIRE(cpu.read(0x0000) == 0x00);

  // STA $0000
  cpu.cycle();
  REQUIRE(cpu.PC() == 0x8002);
  REQUIRE(cpu.A() == 1);

  cpu.cycle();
  REQUIRE(cpu.PC() == 0x8002);
  REQUIRE(cpu.A() == 1);

  cpu.cycle();
  REQUIRE(cpu.PC() == 0x8004);
  REQUIRE(cpu.A() == 1);

  REQUIRE(cpu.read(0x0000) == 0x01);

  // LDA #$02
  cpu.cycle();
  REQUIRE(cpu.PC() == 0x8004);
  REQUIRE(cpu.A() == 1);

  cpu.cycle();
  REQUIRE(cpu.PC() == 0x8006);
  REQUIRE(cpu.A() == 2);

  // LDA $0000
  cpu.cycle();
  REQUIRE(cpu.PC() == 0x8006);
  REQUIRE(cpu.A() == 2);

  cpu.cycle();
  REQUIRE(cpu.PC() == 0x8006);
  REQUIRE(cpu.A() == 2);

  cpu.cycle();
  REQUIRE(cpu.PC() == 0x8008);
  REQUIRE(cpu.A() == 1);
};

TEST_CASE("trivial memory") {
  std::vector<uint8_t> bytecode = {};

  uint8_t val8 = 123;
  uint16_t val16 = 12345;

  MAKE_CPU(bytecode);

  // RAM
  REQUIRE(cpu.read(0x0000) == 0x00);
  REQUIRE(cpu.write(0x0800, val8));
  REQUIRE(cpu.read(0x0000) == val8);
  REQUIRE(cpu.write16(0x0900, val16));
  REQUIRE(cpu.read16(0x0100) == val16);
};

TEST_CASE("Unit: ADC_IMM") {}
TEST_CASE("Unit: ADC_ZP") {}
TEST_CASE("Unit: ADC_ZPX") {}
TEST_CASE("Unit: ADC_ABS") {}
TEST_CASE("Unit: ADC_ABSX") {}
TEST_CASE("Unit: ADC_ABSY") {}
TEST_CASE("Unit: ADC_INDX") {}
TEST_CASE("Unit: ADC_INDY") {}
TEST_CASE("Unit: AND_IMM") {}
TEST_CASE("Unit: AND_ZP") {}
TEST_CASE("Unit: AND_ZPX") {}
TEST_CASE("Unit: AND_ABS") {}
TEST_CASE("Unit: AND_ABSX") {}
TEST_CASE("Unit: AND_ABSY") {}
TEST_CASE("Unit: AND_INDX") {}
TEST_CASE("Unit: AND_INDY") {}
TEST_CASE("Unit: ASL_A") {}
TEST_CASE("Unit: ASL_ZP") {}
TEST_CASE("Unit: ASL_ZPX") {}
TEST_CASE("Unit: ASL_ABS") {}
TEST_CASE("Unit: ASL_ABSX") {}
TEST_CASE("Unit: BCC_REL") {}
TEST_CASE("Unit: BCS_REL") {}
TEST_CASE("Unit: BEQ_REL") {}
TEST_CASE("Unit: BIT_ZP") {}
TEST_CASE("Unit: BIT_ABS") {}
TEST_CASE("Unit: BMI_REL") {}
TEST_CASE("Unit: BNE_REL") {}
TEST_CASE("Unit: BPL_REL") {}
TEST_CASE("Unit: BRK") {}
TEST_CASE("Unit: BVC_REL") {}
TEST_CASE("Unit: BVS_REL") {}
TEST_CASE("Unit: CLC") {}
TEST_CASE("Unit: CLD") {}
TEST_CASE("Unit: CLI") {}
TEST_CASE("Unit: CLV") {}
TEST_CASE("Unit: CMP_IMM") {}
TEST_CASE("Unit: CMP_ZP") {}
TEST_CASE("Unit: CMP_ZPX") {}
TEST_CASE("Unit: CMP_ABS") {}
TEST_CASE("Unit: CMP_ABSX") {}
TEST_CASE("Unit: CMP_ABSY") {}
TEST_CASE("Unit: CMP_INDX") {}
TEST_CASE("Unit: CMP_INDY") {}
TEST_CASE("Unit: CPX_IMM") {}
TEST_CASE("Unit: CPX_ZP") {}
TEST_CASE("Unit: CPX_ABS") {}
TEST_CASE("Unit: CPY_IMM") {}
TEST_CASE("Unit: CPY_ZP") {}
TEST_CASE("Unit: CPY_ABS") {}
TEST_CASE("Unit: DEC_ZP") {}
TEST_CASE("Unit: DEC_ZPX") {}
TEST_CASE("Unit: DEC_ABS") {}
TEST_CASE("Unit: DEC_ABSX") {}
TEST_CASE("Unit: DEX") {}
TEST_CASE("Unit: DEY") {}
TEST_CASE("Unit: EOR_IMM") {}
TEST_CASE("Unit: EOR_ZP") {}
TEST_CASE("Unit: EOR_ZPX") {}
TEST_CASE("Unit: EOR_ABS") {}
TEST_CASE("Unit: EOR_ABSX") {}
TEST_CASE("Unit: EOR_ABSY") {}
TEST_CASE("Unit: EOR_INDX") {}
TEST_CASE("Unit: EOR_INDY") {}
TEST_CASE("Unit: INC_ZP") {}
TEST_CASE("Unit: INC_ZPX") {}
TEST_CASE("Unit: INC_ABS") {}
TEST_CASE("Unit: INC_ABSX") {}
TEST_CASE("Unit: INX") {}
TEST_CASE("Unit: INY") {}
TEST_CASE("Unit: JMP_ABS") {}
TEST_CASE("Unit: JMP_IND") {}
TEST_CASE("Unit: JSR_ABS") {}

TEST_CASE("Unit: LDA_IMM") {
  SECTION("Postitive") {
    std::vector<uint8_t> bytecode = {
        kLDA_IMM,
        0x09,
    };

    MAKE_CPU(bytecode);

    cpu.cycle();

    REQUIRE(cpu.A() == 0);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());

    cpu.cycle();

    REQUIRE(cpu.A() == 9);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Negative") {
    std::vector<uint8_t> bytecode = {
        kLDA_IMM,
        0x91,
    };

    MAKE_CPU(bytecode);

    cpu.cycle();

    REQUIRE(cpu.A() == 0);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());

    cpu.cycle();

    REQUIRE(cpu.A() == 0x91);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE(cpu.get_negative());
  };

  SECTION("Zero flag") {
    std::vector<uint8_t> bytecode = {
        kLDA_IMM,
        0x00,
    };

    MAKE_CPU(bytecode);

    cpu.cycle();

    REQUIRE(cpu.A() == 0);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
    cpu.cycle();

    REQUIRE(cpu.A() == 0);
    REQUIRE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };
}

TEST_CASE("Unit: LDA_ZP") {
  std::vector<uint8_t> bytecode = {
      kLDA_ZP,
      0x05,
  };

  MAKE_CPU(bytecode);

  cpu.cycle();
  cpu.cycle();

  REQUIRE(cpu.A() == 0);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());

  SECTION("Positive") {
    cpu.write(0x05, 0x09);

    cpu.cycle();

    REQUIRE(cpu.A() == 9);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Negative") {
    cpu.write(0x05, 0x91);

    cpu.cycle();

    REQUIRE(cpu.A() == 0x91);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE(cpu.get_negative());
  };

  SECTION("Zero flag") {
    cpu.write(0x05, 0x00);

    cpu.cycle();

    REQUIRE(cpu.A() == 0x00);
    REQUIRE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };
}

TEST_CASE("Unit: LDA_ZPX") {
  std::vector<uint8_t> bytecode = {
      kLDA_ZPX,
      0x05,
  };

  MAKE_CPU(bytecode);

  cpu.cycle();
  cpu.cycle();
  cpu.cycle();

  REQUIRE(cpu.A() == 0);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());

  SECTION("Positive") {
    cpu.write(0x05, 0x09);

    cpu.cycle();

    REQUIRE(cpu.A() == 9);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Negative") {
    cpu.write(0x05, 0x91);

    cpu.cycle();

    REQUIRE(cpu.A() == 0x91);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE(cpu.get_negative());
  };

  SECTION("Zero flag") {
    cpu.write(0x05, 0x00);

    cpu.cycle();

    REQUIRE(cpu.A() == 0x00);
    REQUIRE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };
}

TEST_CASE("Unit: LDA_ABS") {
  std::vector<uint8_t> bytecode = {
      kLDA_ABS, 0x02, 0x00,  // Address 0x0002
  };

  MAKE_CPU(bytecode);

  cpu.cycle();
  cpu.cycle();
  cpu.cycle();

  REQUIRE(cpu.A() == 0);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());

  SECTION("Positive") {
    cpu.write(0x0002, 0x09);

    cpu.cycle();

    REQUIRE(cpu.A() == 9);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Negative") {
    cpu.write(0x0002, 0x91);

    cpu.cycle();

    REQUIRE(cpu.A() == 0x91);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE(cpu.get_negative());
  };

  SECTION("Zero flag") {
    cpu.write(0x0002, 0x00);

    cpu.cycle();

    REQUIRE(cpu.A() == 0x00);
    REQUIRE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };
}

TEST_CASE("Unit: LDA_ABSX") {}
TEST_CASE("Unit: LDA_ABSY") {}
TEST_CASE("Unit: LDA_INDX") {}
TEST_CASE("Unit: LDA_INDY") {}
TEST_CASE("Unit: LDX_IMM") {}
TEST_CASE("Unit: LDX_ZP") {}
TEST_CASE("Unit: LDX_ZPY") {}
TEST_CASE("Unit: LDX_ABS") {}
TEST_CASE("Unit: LDX_ABSY") {}
TEST_CASE("Unit: LDY_IMM") {}
TEST_CASE("Unit: LDY_ZP") {}
TEST_CASE("Unit: LDY_ZPX") {}
TEST_CASE("Unit: LDY_ABS") {}
TEST_CASE("Unit: LDY_ABSX") {}
TEST_CASE("Unit: LSR_A") {}
TEST_CASE("Unit: LSR_ZP") {}
TEST_CASE("Unit: LSR_ZPX") {}
TEST_CASE("Unit: LSR_ABS") {}
TEST_CASE("Unit: LSR_ABSX") {}
TEST_CASE("Unit: NOP") {}
TEST_CASE("Unit: ORA_IMM") {}
TEST_CASE("Unit: ORA_ZP") {}
TEST_CASE("Unit: ORA_ZPX") {}
TEST_CASE("Unit: ORA_ABS") {}
TEST_CASE("Unit: ORA_ABSX") {}
TEST_CASE("Unit: ORA_ABSY") {}
TEST_CASE("Unit: ORA_INDX") {}
TEST_CASE("Unit: ORA_INDY") {}
TEST_CASE("Unit: PHA") {}
TEST_CASE("Unit: PHP") {}
TEST_CASE("Unit: PLA") {}
TEST_CASE("Unit: PLP") {}
TEST_CASE("Unit: ROL_A") {}
TEST_CASE("Unit: ROL_ZP") {}
TEST_CASE("Unit: ROL_ZPX") {}
TEST_CASE("Unit: ROL_ABS") {}
TEST_CASE("Unit: ROL_ABSX") {}
TEST_CASE("Unit: ROR_A") {}
TEST_CASE("Unit: ROR_ZP") {}
TEST_CASE("Unit: ROR_ZPX") {}
TEST_CASE("Unit: ROR_ABS") {}
TEST_CASE("Unit: ROR_ABSX") {}
TEST_CASE("Unit: RTI") {}
TEST_CASE("Unit: RTS") {}
TEST_CASE("Unit: SBC_IMM") {}
TEST_CASE("Unit: SBC_ZP") {}
TEST_CASE("Unit: SBC_ZPX") {}
TEST_CASE("Unit: SBC_ABS") {}
TEST_CASE("Unit: SBC_ABSX") {}
TEST_CASE("Unit: SBC_ABSY") {}
TEST_CASE("Unit: SBC_INDX") {}
TEST_CASE("Unit: SBC_INDY") {}
TEST_CASE("Unit: SEC") {}
TEST_CASE("Unit: SED") {}
TEST_CASE("Unit: SEI") {}
TEST_CASE("Unit: STA_ZP") {}
TEST_CASE("Unit: STA_ZPX") {}
TEST_CASE("Unit: STA_ABS") {}
TEST_CASE("Unit: STA_ABSX") {}
TEST_CASE("Unit: STA_ABSY") {}
TEST_CASE("Unit: STA_INDX") {}
TEST_CASE("Unit: STA_INDY") {}
TEST_CASE("Unit: STX_ZP") {}
TEST_CASE("Unit: STX_ZPY") {}
TEST_CASE("Unit: STX_ABS") {}
TEST_CASE("Unit: STY_ZP") {}
TEST_CASE("Unit: STY_ZPX") {}
TEST_CASE("Unit: STY_ABS") {}
TEST_CASE("Unit: TAX") {}
TEST_CASE("Unit: TAY") {}
TEST_CASE("Unit: TSX") {}
TEST_CASE("Unit: TXA") {}
TEST_CASE("Unit: TXS") {}
TEST_CASE("Unit: TYA") {}
