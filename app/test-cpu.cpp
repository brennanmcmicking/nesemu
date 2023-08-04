#include <catch2/catch.hpp>
#include <cstdint>
#include <vector>

#include "cartridge.hpp"
#include "cpu.hpp"
#include "util.hpp"

using namespace util;
using namespace cpu;

class VectorMapper : public cartridge::Mapper {
 public:
  VectorMapper(std::vector<uint8_t> bytecode, uint16_t brk_isr = 0xFFFFu,
               uint16_t entrypoint = 0x8000u)
      : bytes_(bytecode), entrypoint_(entrypoint), brk_isr_(brk_isr){};
  uint8_t prg_read(uint16_t addr) override {
    if (addr < 0x8000) {
      return 0xAA;
    } else if (addr == 0xFFFC) {
      return entrypoint_ & 0xFF;
    } else if (addr == 0xFFFD) {
      return entrypoint_ >> 8;
    } else if (addr == 0xFFFE) {
      return brk_isr_ & 0xFF;
    } else if (addr == 0xFFFF) {
      return brk_isr_ >> 8;
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
  uint16_t brk_isr_;
};

#define U16(x) static_cast<uint8_t>((x)&0xFFu), static_cast<uint8_t>((x) >> 8u)

#define MAKE_CPU(bytecode)                                            \
  std::unique_ptr<VectorMapper> __mapper(new VectorMapper(bytecode)); \
  cartridge::Cartridge __cart(std::move(__mapper));                   \
  CPU cpu(__cart);

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

TEST_CASE("Unit: ADC_IMM") {
  SECTION("no flags") {
    std::vector<uint8_t> bytecode = {
        kLDA_IMM, 0x01,  //
        kADC_IMM, 0x01,  //
    };

    MAKE_CPU(bytecode);
    REQUIRE(cpu.A() == 0);
    REQUIRE(cpu.get_carry() == false);
    REQUIRE(cpu.get_zero() == false);
    REQUIRE(cpu.get_negative() == false);
    REQUIRE(cpu.get_overflow() == false);

    cpu.advance_cycles(2);
    REQUIRE(cpu.A() == 1);

    cpu.advance_cycles(2);
    REQUIRE(cpu.A() == 2);
    REQUIRE(cpu.get_carry() == false);
    REQUIRE(cpu.get_zero() == false);
    REQUIRE(cpu.get_negative() == false);
    REQUIRE(cpu.get_overflow() == false);
  };
  SECTION("carry and zero flag") {
    std::vector<uint8_t> bytecode = {
        kLDA_IMM, 0xFF,  //
        kADC_IMM, 0x01,  //
    };

    MAKE_CPU(bytecode);

    REQUIRE(cpu.PC() == 0x8000);
    REQUIRE(cpu.A() == 0);
    REQUIRE(cpu.get_carry() == false);
    REQUIRE(cpu.get_zero() == false);
    REQUIRE(cpu.get_negative() == false);
    REQUIRE(cpu.get_overflow() == false);

    cpu.advance_cycles(2);
    REQUIRE(cpu.A() == 0xFF);

    cpu.advance_cycles(2);
    REQUIRE(cpu.A() == 0);
    REQUIRE(cpu.get_carry() == true);
    REQUIRE(cpu.get_zero() == true);
    REQUIRE(cpu.get_negative() == false);
    REQUIRE(cpu.get_overflow() == false);
  };
  SECTION("negative and carry flag") {
    std::vector<uint8_t> bytecode = {
        kLDA_IMM, static_cast<uint8_t>(-5),   // 0xFB
        kADC_IMM, static_cast<uint8_t>(-10),  // 0xF6
    };

    MAKE_CPU(bytecode);

    REQUIRE(cpu.PC() == 0x8000);
    REQUIRE(cpu.A() == 0);
    REQUIRE(cpu.get_carry() == false);
    REQUIRE(cpu.get_zero() == false);
    REQUIRE(cpu.get_negative() == false);

    cpu.advance_cycles(2);
    REQUIRE(cpu.A() == static_cast<uint8_t>(-5));  // 0xFB

    cpu.advance_cycles(2);
    REQUIRE(cpu.A() == static_cast<uint8_t>(-15));  // 0xF1
    REQUIRE(cpu.get_carry() == true);
    REQUIRE(cpu.get_zero() == false);
    REQUIRE(cpu.get_negative() == true);
  };
  SECTION("overflow and negative flag") {
    std::vector<uint8_t> bytecode = {
        kLDA_IMM, 0x7F,  //
        kADC_IMM, 0x01,  //
    };

    MAKE_CPU(bytecode);

    REQUIRE(cpu.PC() == 0x8000);
    REQUIRE(cpu.A() == 0);
    REQUIRE(cpu.get_carry() == false);
    REQUIRE(cpu.get_zero() == false);
    REQUIRE(cpu.get_negative() == false);
    REQUIRE(cpu.get_overflow() == false);

    cpu.advance_cycles(2);
    REQUIRE(cpu.A() == 0x7F);  // 127

    cpu.advance_cycles(2);
    REQUIRE(cpu.A() == 0x80);  // -128
    REQUIRE(cpu.get_carry() == false);
    REQUIRE(cpu.get_zero() == false);
    REQUIRE(cpu.get_negative() == true);
    REQUIRE(cpu.get_overflow() == true);
  };
}

TEST_CASE("Unit: ADC_ZP") {
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, 0x01,  //
      kSTA_ZP,  0x00,  //
      kADC_ZP,  0x00,  //
  };

  MAKE_CPU(bytecode);

  REQUIRE(cpu.A() == 0);

  cpu.advance_cycles(2);
  REQUIRE(cpu.A() == 1);
  REQUIRE(cpu.read(0x0000) == 0x00);

  cpu.advance_cycles(3);
  REQUIRE(cpu.A() == 1);
  REQUIRE(cpu.read(0x0000) == 0x01);

  cpu.advance_cycles(3);
  REQUIRE(cpu.A() == 2);
}

TEST_CASE("Unit: ADC_ZPX") {
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, 0x05,  //
      kSTA_ZP,  0x03,  //
      kLDX_IMM, 0x02,  //
      kADC_ZPX, 0x01,  //
  };

  MAKE_CPU(bytecode);

  REQUIRE(cpu.A() == 0);

  cpu.advance_cycles(2);
  REQUIRE(cpu.A() == 5);
  REQUIRE(cpu.read(0x0003) == 0x00);

  cpu.advance_cycles(3);
  REQUIRE(cpu.A() == 5);
  REQUIRE(cpu.read(0x0003) == 0x05);

  cpu.advance_cycles(2);
  REQUIRE(cpu.X() == 2);

  cpu.advance_cycles(4);
  REQUIRE(cpu.A() == 10);
}

TEST_CASE("Unit: ADC_ABS") {
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, 0x01,         //
      kSTA_ABS, U16(0x1000),  //
      kADC_ABS, U16(0x1000),  //
  };

  MAKE_CPU(bytecode);

  REQUIRE(cpu.A() == 0);

  cpu.advance_cycles(2);
  REQUIRE(cpu.A() == 1);
  REQUIRE(cpu.read(0x1000) == 0x00);

  cpu.advance_cycles(4);
  REQUIRE(cpu.A() == 1);
  REQUIRE(cpu.read(0x1000) == 0x01);

  cpu.advance_cycles(4);
  REQUIRE(cpu.A() == 2);
}

TEST_CASE("Unit: ADC_ABSX") {
  SECTION("no page cross") {
    std::vector<uint8_t> bytecode = {
        kLDA_IMM,  0x05,             //
        kSTA_ABS,  U16(0x1000 + 2),  //
        kLDX_IMM,  0x02,             //
        kADC_ABSX, U16(0x1000),      //
    };

    MAKE_CPU(bytecode);

    REQUIRE(cpu.A() == 0);

    cpu.advance_cycles(2);
    REQUIRE(cpu.A() == 5);
    REQUIRE(cpu.read(0x1002) == 0x00);

    cpu.advance_cycles(4);
    REQUIRE(cpu.A() == 5);
    REQUIRE(cpu.read(0x1002) == 0x05);

    cpu.advance_cycles(2);
    REQUIRE(cpu.X() == 2);
    REQUIRE(cpu.A() == 5);
    REQUIRE(cpu.read(0x1002) == 0x05);

    cpu.advance_cycles(4);
    REQUIRE(cpu.A() == 10);
    REQUIRE(cpu.read(0x1002) == 0x05);
  }
  SECTION("page cross") {
    std::vector<uint8_t> bytecode = {
        kLDA_IMM,  0x05,             //
        kSTA_ABS,  U16(0x1000),      //
        kLDX_IMM,  0x02,             //
        kADC_ABSX, U16(0x1000 - 2),  //
    };

    MAKE_CPU(bytecode);

    REQUIRE(cpu.A() == 0);

    cpu.advance_cycles(2);
    REQUIRE(cpu.A() == 5);
    REQUIRE(cpu.read(0x1000) == 0x00);

    cpu.advance_cycles(4);
    REQUIRE(cpu.A() == 5);
    REQUIRE(cpu.read(0x1000) == 0x05);

    cpu.advance_cycles(2);
    REQUIRE(cpu.X() == 2);
    REQUIRE(cpu.A() == 5);
    REQUIRE(cpu.read(0x1000) == 0x05);

    cpu.advance_cycles(4);
    REQUIRE(cpu.A() == 5);
    REQUIRE(cpu.read(0x1000) == 0x05);

    cpu.cycle();
    REQUIRE(cpu.A() == 10);
    REQUIRE(cpu.read(0x1000) == 0x05);
  }
}

TEST_CASE("Unit: ADC_ABSY") {
  SECTION("no page cross") {
    std::vector<uint8_t> bytecode = {
        kLDA_IMM,  0x05,             //
        kSTA_ABS,  U16(0x1000 + 2),  //
        kLDY_IMM,  0x02,             //
        kADC_ABSY, U16(0x1000),      //
    };

    MAKE_CPU(bytecode);

    REQUIRE(cpu.A() == 0);

    cpu.advance_cycles(2);
    REQUIRE(cpu.A() == 5);
    REQUIRE(cpu.read(0x1002) == 0x00);

    cpu.advance_cycles(4);
    REQUIRE(cpu.A() == 5);
    REQUIRE(cpu.read(0x1002) == 0x05);

    cpu.advance_cycles(2);
    REQUIRE(cpu.Y() == 2);
    REQUIRE(cpu.A() == 5);
    REQUIRE(cpu.read(0x1002) == 0x05);

    cpu.advance_cycles(4);
    REQUIRE(cpu.A() == 10);
    REQUIRE(cpu.read(0x1002) == 0x05);
  }
  SECTION("page cross") {
    std::vector<uint8_t> bytecode = {
        kLDA_IMM,  0x05,             //
        kSTA_ABS,  U16(0x1000),      //
        kLDY_IMM,  0x02,             //
        kADC_ABSY, U16(0x1000 - 2),  //
    };

    MAKE_CPU(bytecode);

    REQUIRE(cpu.A() == 0);

    cpu.advance_cycles(2);
    REQUIRE(cpu.A() == 5);
    REQUIRE(cpu.read(0x1000) == 0x00);

    cpu.advance_cycles(4);
    REQUIRE(cpu.A() == 5);
    REQUIRE(cpu.read(0x1000) == 0x05);

    cpu.advance_cycles(2);
    REQUIRE(cpu.Y() == 2);
    REQUIRE(cpu.A() == 5);
    REQUIRE(cpu.read(0x1000) == 0x05);

    cpu.advance_cycles(4);
    REQUIRE(cpu.A() == 5);
    REQUIRE(cpu.read(0x1000) == 0x05);

    cpu.cycle();
    REQUIRE(cpu.A() == 10);
    REQUIRE(cpu.read(0x1000) == 0x05);
  }
}

TEST_CASE("Unit: ADC_INDX") {
  std::vector<uint8_t> bytecode = {
      kLDA_IMM,  0x05,         //
      kSTA_ABS,  U16(0x1000),  //
      kLDX_IMM,  0x02,         //
      kADC_INDX, 0x03,         //
  };

  MAKE_CPU(bytecode);
  REQUIRE(cpu.A() == 0);
  cpu.write16(0x0005, 0x1000);

  cpu.advance_cycles(2);
  REQUIRE(cpu.A() == 5);
  REQUIRE(cpu.read(0x1000) == 0x00);

  cpu.advance_cycles(4);
  REQUIRE(cpu.A() == 5);
  REQUIRE(cpu.read(0x1000) == 0x05);

  cpu.advance_cycles(2);
  REQUIRE(cpu.X() == 2);
  REQUIRE(cpu.A() == 5);
  REQUIRE(cpu.read(0x1000) == 0x05);

  cpu.advance_cycles(6);
  REQUIRE(cpu.A() == 10);
  REQUIRE(cpu.read(0x1000) == 0x05);
}

TEST_CASE("Unit: ADC_INDY") {
  std::vector<uint8_t> bytecode = {
      kLDA_IMM,  0x05,         //
      kSTA_ABS,  U16(0x1002),  //
      kLDY_IMM,  0x02,         //
      kADC_INDY, 0x03,         //
  };

  MAKE_CPU(bytecode);
  REQUIRE(cpu.A() == 0);
  cpu.write16(0x0003, 0x1000);

  cpu.advance_cycles(2);
  REQUIRE(cpu.A() == 5);
  REQUIRE(cpu.read(0x1000) == 0x00);

  cpu.advance_cycles(4);
  REQUIRE(cpu.A() == 5);
  REQUIRE(cpu.read(0x1002) == 0x05);

  cpu.advance_cycles(2);
  REQUIRE(cpu.Y() == 2);
  REQUIRE(cpu.A() == 5);
  REQUIRE(cpu.read(0x1002) == 0x05);

  cpu.advance_cycles(6);

  REQUIRE(cpu.A() == 10);
  REQUIRE(cpu.read(0x1002) == 0x05);
}

TEST_CASE("Unit: AND_IMM") {
  SECTION("Postitive") {
    std::vector<uint8_t> bytecode = {
        kLDA_IMM, 0b00000101,  //
        kAND_IMM, 0b00000110   //
    };

    MAKE_CPU(bytecode);

    cpu.advance_instruction();

    REQUIRE(cpu.A() == 0b00000101);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());

    cpu.advance_cycles(2);

    REQUIRE(cpu.A() == (0b00000101 & 0b00000110));
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Negative") {
    std::vector<uint8_t> bytecode = {
        kLDA_IMM, 0b10000101,  //
        kAND_IMM, 0b10000110   //
    };

    MAKE_CPU(bytecode);

    cpu.advance_instruction();

    REQUIRE(cpu.A() == 0b10000101);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE(cpu.get_negative());

    cpu.advance_cycles(2);

    REQUIRE(cpu.A() == (0b10000101 & 0b10000110));
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE(cpu.get_negative());
  };

  SECTION("Zero flag") {
    std::vector<uint8_t> bytecode = {
        kLDA_IMM, 0b00000001,  //
        kAND_IMM, 0b00000010   //
    };

    MAKE_CPU(bytecode);

    cpu.advance_instruction();

    REQUIRE(cpu.A() == 0b00000001);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());

    cpu.advance_cycles(2);

    REQUIRE(cpu.A() == 0);
    REQUIRE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };
}

TEST_CASE("Unit: AND_ZP") {
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, 0b00010101,  //
      kAND_ZP, 0x07          //
  };

  MAKE_CPU(bytecode);
  cpu.write(0x07, 0b00000100);

  cpu.advance_instruction();

  REQUIRE(cpu.A() == 0b00010101);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());

  cpu.advance_instruction();

  REQUIRE(cpu.A() == 0b00000100);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());
}

TEST_CASE("Unit: AND_ZPX") {
  std::vector<uint8_t> bytecode = {
      kLDX_IMM, 0x02,        //
      kLDA_IMM, 0b00000101,  //
      kAND_ZPX, 0x05         //
  };

  MAKE_CPU(bytecode);
  cpu.write(0x07, 0b00000100);

  cpu.advance_instruction();
  cpu.advance_instruction();

  REQUIRE(cpu.A() == 0b00000101);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());

  cpu.advance_instruction();

  REQUIRE(cpu.A() == 0b00000100);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());
}

TEST_CASE("Unit: AND_ABS") {
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, 0b00000101,  //
      kAND_ABS, U16(0x05)    //
  };

  MAKE_CPU(bytecode);
  cpu.write(0x05, 0b00000100);

  cpu.advance_instruction();

  REQUIRE(cpu.A() == 0b00000101);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());

  cpu.advance_instruction();

  REQUIRE(cpu.A() == 0b00000100);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());
}

TEST_CASE("Unit: AND_ABSX") {
  std::vector<uint8_t> bytecode = {
      kLDX_IMM,  0x02,        //
      kLDA_IMM,  0b00000101,  //
      kAND_ABSX, U16(0x05)    //
  };

  MAKE_CPU(bytecode);
  cpu.write(0x07, 0b00000100);

  cpu.advance_instruction();
  cpu.advance_instruction();

  REQUIRE(cpu.A() == 0b00000101);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());

  cpu.advance_instruction();

  REQUIRE(cpu.A() == 0b00000100);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());
}

TEST_CASE("Unit: AND_ABSY") {
  std::vector<uint8_t> bytecode = {
      kLDY_IMM,  0x02,        //
      kLDA_IMM,  0b00000101,  //
      kAND_ABSY, U16(0x05)    //
  };

  MAKE_CPU(bytecode);
  cpu.write(0x07, 0b00000100);

  cpu.advance_instruction();
  cpu.advance_instruction();

  REQUIRE(cpu.A() == 0b00000101);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());

  cpu.advance_cycles(4);

  REQUIRE(cpu.A() == 0b00000100);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());
}

TEST_CASE("Unit: AND_INDX") {
  std::vector<uint8_t> bytecode = {
      kLDX_IMM,  0x02,        //
      kLDA_IMM,  0b00000101,  //
      kAND_INDX, U16(0x05)    //
  };

  MAKE_CPU(bytecode);
  cpu.write(0x07, 0x09);
  cpu.write(0x09, 0b00000100);

  cpu.advance_instruction();
  cpu.advance_instruction();

  REQUIRE(cpu.A() == 0b00000101);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());

  cpu.advance_instruction();

  REQUIRE(cpu.A() == 0b00000100);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());
}

TEST_CASE("Unit: AND_INDY") {
  std::vector<uint8_t> bytecode = {
      kLDY_IMM,  0x02,        //
      kLDA_IMM,  0b00000101,  //
      kAND_INDY, U16(0x05)    //
  };

  MAKE_CPU(bytecode);
  cpu.write(0x05, 0x07);
  cpu.write(0x09, 0b00000100);

  cpu.advance_instruction();
  cpu.advance_instruction();

  REQUIRE(cpu.A() == 0b00000101);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());

  cpu.advance_instruction();

  REQUIRE(cpu.A() == 0b00000100);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());
}

TEST_CASE("Unit: ASL_A") {}

TEST_CASE("Unit: ASL_ZP") {}

TEST_CASE("Unit: ASL_ZPX") {}

TEST_CASE("Unit: ASL_ABS") {}

TEST_CASE("Unit: ASL_ABSX") {}

TEST_CASE("Unit: BCC_REL") {
  // branch if carry clear
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, 0x02,  //
      kCMP_IMM, 0x01,  //
      kBCC_REL, 0x06,  // jump 6 (+2)
      kCMP_IMM, 0x03,  //
      kNOP,     kNOP,  //
      kNOP,     kNOP,  //
      kBCC_REL, 0xF8,  // jump -8 (+2)
      kNOP,     kNOP   //
  };

  MAKE_CPU(bytecode);

  cpu.advance_instruction();  // LDA #$02
  cpu.advance_instruction();  // CMP #$01

  REQUIRE_FALSE(cpu.get_negative());
  REQUIRE_FALSE(cpu.get_carry());

  uint16_t old_pc = cpu.PC();

  cpu.advance_instruction();  // BCC #$06

  REQUIRE(cpu.PC() == old_pc + 0x08);

  old_pc = cpu.PC();

  cpu.advance_instruction();  // BCC #$-08

  REQUIRE(cpu.PC() == old_pc - 0x06);

  cpu.advance_instruction();  // CMP #$03
  REQUIRE(cpu.get_negative());

  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP

  old_pc = cpu.PC();

  cpu.advance_instruction();

  REQUIRE(cpu.PC() == old_pc + 0x02);
}

TEST_CASE("Unit: BCS_REL") {
  // branch if carry set
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, 0x02,  //
      kCMP_IMM, 0x03,  //
      kBCS_REL, 0x06,  // jump 6 (+2)
      kCMP_IMM, 0x01,  //
      kNOP,     kNOP,  //
      kNOP,     kNOP,  //
      kBCS_REL, 0xF8,  // jump -8 (+2)
      kNOP,     kNOP   //
  };

  MAKE_CPU(bytecode);

  cpu.advance_instruction();  // LDA #$02
  cpu.advance_instruction();  // CMP #$03

  REQUIRE(cpu.get_negative());
  REQUIRE(cpu.get_carry());

  uint16_t old_pc = cpu.PC();

  cpu.advance_instruction();  // BCS #$06

  REQUIRE(cpu.PC() == old_pc + 0x08);

  old_pc = cpu.PC();

  cpu.advance_instruction();  // BCS #$-08

  REQUIRE(cpu.PC() == old_pc - 0x06);

  cpu.advance_instruction();  // CMP #$01
  REQUIRE_FALSE(cpu.get_negative());

  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP

  old_pc = cpu.PC();

  cpu.advance_instruction();

  REQUIRE(cpu.PC() == old_pc + 0x02);
}

TEST_CASE("Unit: BEQ_REL") {
  // branch if equal (i.e. zero set)
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, 0x02,  //
      kCMP_IMM, 0x02,  //
      kBEQ_REL, 0x06,  // jump 6 (+2)
      kCMP_IMM, 0x03,  //
      kNOP,     kNOP,  //
      kNOP,     kNOP,  //
      kBEQ_REL, 0xF8,  // jump -8 (+2)
      kNOP,     kNOP   //
  };

  MAKE_CPU(bytecode);

  cpu.advance_instruction();  // LDA #$02
  cpu.advance_instruction();  // CMP #$02

  REQUIRE_FALSE(cpu.get_negative());
  REQUIRE_FALSE(cpu.get_carry());
  REQUIRE(cpu.get_zero());

  uint16_t old_pc = cpu.PC();

  cpu.advance_instruction();  // BEQ #$06

  REQUIRE(cpu.PC() == old_pc + 0x08);

  old_pc = cpu.PC();

  cpu.advance_instruction();  // BEQ #$-08

  REQUIRE(cpu.PC() == old_pc - 0x06);

  cpu.advance_instruction();  // CMP #$03
  REQUIRE(cpu.get_negative());

  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP

  old_pc = cpu.PC();

  cpu.advance_instruction();

  REQUIRE(cpu.PC() == old_pc + 0x02);
}

TEST_CASE("Unit: BIT_ZP") {}

TEST_CASE("Unit: BIT_ABS") {}

TEST_CASE("Unit: BMI_REL") {
  // branch if minus
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, 0x02,  //
      kCMP_IMM, 0x03,  //
      kBMI_REL, 0x06,  // jump 6 (+2)
      kCMP_IMM, 0x01,  //
      kNOP,     kNOP,  //
      kNOP,     kNOP,  //
      kBMI_REL, 0xF8,  // jump -8 (+2)
      kNOP,     kNOP   //
  };

  MAKE_CPU(bytecode);

  cpu.advance_instruction();  // LDA #$02
  cpu.advance_instruction();  // CMP #$03

  REQUIRE(cpu.get_negative());
  REQUIRE(cpu.get_carry());

  uint16_t old_pc = cpu.PC();

  cpu.advance_instruction();  // BMI #$06

  REQUIRE(cpu.PC() == old_pc + 0x08);

  old_pc = cpu.PC();

  cpu.advance_instruction();  // BMI #$-08

  REQUIRE(cpu.PC() == old_pc - 0x06);

  cpu.advance_instruction();  // CMP #$01
  REQUIRE_FALSE(cpu.get_negative());

  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP

  old_pc = cpu.PC();

  cpu.advance_instruction();

  REQUIRE(cpu.PC() == old_pc + 0x02);
}

TEST_CASE("Unit: BNE_REL") {
  // branch if not equal
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, 0x02,  //
      kCMP_IMM, 0x01,  //
      kBNE_REL, 0x06,  // jump 6 (+2)
      kCMP_IMM, 0x02,  //
      kNOP,     kNOP,  //
      kNOP,     kNOP,  //
      kBNE_REL, 0xF8,  // jump -8 (+2)
      kNOP,     kNOP   //
  };

  MAKE_CPU(bytecode);

  cpu.advance_instruction();  // LDA #$02
  cpu.advance_instruction();  // CMP #$01

  REQUIRE_FALSE(cpu.get_negative());
  REQUIRE_FALSE(cpu.get_carry());

  uint16_t old_pc = cpu.PC();

  cpu.advance_instruction();  // BNE #$06

  REQUIRE(cpu.PC() == old_pc + 0x08);

  old_pc = cpu.PC();

  cpu.advance_instruction();  // BNE #$-08

  REQUIRE(cpu.PC() == old_pc - 0x06);

  cpu.advance_instruction();  // CMP #$02
  REQUIRE_FALSE(cpu.get_negative());

  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP

  old_pc = cpu.PC();

  cpu.advance_instruction();

  REQUIRE(cpu.PC() == old_pc + 0x02);
}

TEST_CASE("Unit: BPL_REL") {
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, 0x02,  //
      kCMP_IMM, 0x01,  //
      kBPL_REL, 0x06,  // jump 6 (+2)
      kCMP_IMM, 0x03,  //
      kNOP,     kNOP,  //
      kNOP,     kNOP,  //
      kBPL_REL, 0xF8,  // jump -8 (+2)
      kNOP,     kNOP   //
  };

  MAKE_CPU(bytecode);

  cpu.advance_instruction();  // LDA #$02
  cpu.advance_instruction();  // CMP #$01

  REQUIRE_FALSE(cpu.get_negative());

  uint16_t old_pc = cpu.PC();

  cpu.advance_instruction();  // BPL #$06

  REQUIRE(cpu.PC() == old_pc + 0x08);

  old_pc = cpu.PC();

  cpu.advance_instruction();  // BPL #$-08

  REQUIRE(cpu.PC() == old_pc - 0x06);

  cpu.advance_instruction();  // CMP #$03
  REQUIRE(cpu.get_negative());

  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP

  old_pc = cpu.PC();

  cpu.advance_instruction();

  REQUIRE(cpu.PC() == old_pc + 0x02);
}

TEST_CASE("Unit: BRK") {
  const uint16_t addr = 0x1234;
  std::vector<uint8_t> bytecode = {
      kBRK,  //
  };

  std::unique_ptr<VectorMapper> __mapper(new VectorMapper(bytecode, addr));
  cartridge::Cartridge __cart(std::move(__mapper));
  CPU cpu(__cart);

  REQUIRE(cpu.get_break() == true);

  cpu.advance_instruction();
  REQUIRE(cpu.get_break() == true);
  // TODO NMI stuff?
  REQUIRE(cpu.PC() == addr);
}

TEST_CASE("Unit: BVC_REL") {
  // branch if overflow clear
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, 0x00,  //
      kADC_IMM, 0x01,  //
      kBVC_REL, 0x06,  // jump 6 (+2)
      kADC_IMM, 0x7F,  //
      kNOP,     kNOP,  //
      kNOP,     kNOP,  //
      kBVC_REL, 0xF8,  // jump -8 (+2)
      kNOP,     kNOP   //
  };

  MAKE_CPU(bytecode);

  cpu.advance_instruction();  // LDA #$02
  cpu.advance_instruction();  // CMP #$01

  REQUIRE_FALSE(cpu.get_negative());
  REQUIRE_FALSE(cpu.get_carry());
  REQUIRE_FALSE(cpu.get_overflow());

  uint16_t old_pc = cpu.PC();

  cpu.advance_instruction();  // BVC #$06

  REQUIRE(cpu.PC() == old_pc + 0x08);

  old_pc = cpu.PC();

  cpu.advance_instruction();  // BVC #$-08

  REQUIRE(cpu.PC() == old_pc - 0x06);

  cpu.advance_instruction();  // CMP #$03
  REQUIRE(cpu.get_negative());

  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP

  old_pc = cpu.PC();

  cpu.advance_instruction();

  REQUIRE(cpu.PC() == old_pc + 0x02);
}

TEST_CASE("Unit: BVS_REL") {
  // branch if overflow set
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, 0x7F,  //
      kADC_IMM, 0x01,  //
      kBVS_REL, 0x06,  // jump 6 (+2)
      kADC_IMM, 0x00,  //
      kNOP,     kNOP,  //
      kNOP,     kNOP,  //
      kBVS_REL, 0xF8,  // jump -8 (+2)
      kNOP,     kNOP   //
  };

  MAKE_CPU(bytecode);

  cpu.advance_instruction();  // LDA #$02
  cpu.advance_instruction();  // CMP #$03

  REQUIRE(cpu.get_negative());
  REQUIRE_FALSE(cpu.get_carry());
  REQUIRE(cpu.get_overflow());

  uint16_t old_pc = cpu.PC();

  cpu.advance_instruction();  // BVS #$06

  REQUIRE(cpu.PC() == old_pc + 0x08);

  old_pc = cpu.PC();

  cpu.advance_instruction();  // BVS #$-08

  REQUIRE(cpu.PC() == old_pc - 0x06);

  cpu.advance_instruction();  // CMP #$01
  REQUIRE(cpu.get_negative());

  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP
  cpu.advance_instruction();  // NOP

  old_pc = cpu.PC();

  cpu.advance_instruction();

  REQUIRE(cpu.PC() == old_pc + 0x02);
}

TEST_CASE("Unit: CLC") {
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, 0x01,  //
      kADC_IMM, 0xFF,  //
      kCLC             //
  };

  MAKE_CPU(bytecode);

  cpu.advance_instruction();  // LDA
  cpu.advance_instruction();  // ADC

  REQUIRE(cpu.get_carry());

  cpu.advance_cycles(2);

  REQUIRE_FALSE(cpu.get_carry());
}

TEST_CASE("Unit: CLD") {
  std::vector<uint8_t> bytecode = {
      kSED,  //
      kCLD   //
  };

  MAKE_CPU(bytecode);

  cpu.advance_instruction();

  REQUIRE(cpu.get_decimal());

  cpu.advance_cycles(2);

  REQUIRE_FALSE(cpu.get_decimal());
}

TEST_CASE("Unit: CLI") {
  std::vector<uint8_t> bytecode = {
      kCLI  //
  };

  MAKE_CPU(bytecode);

  REQUIRE(cpu.get_interrupt_disable());

  cpu.advance_cycles(2);

  REQUIRE_FALSE(cpu.get_interrupt_disable());
}

TEST_CASE("Unit: CLV") {
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, 0x7F,  //
      kADC_IMM, 0x01,  //
      kCLV             //
  };

  MAKE_CPU(bytecode);

  cpu.advance_instruction();  // LDA
  cpu.advance_instruction();  // ADC

  REQUIRE(cpu.get_overflow());

  cpu.advance_cycles(2);

  REQUIRE_FALSE(cpu.get_overflow());
}

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

TEST_CASE("Unit: JMP_ABS") {
  std::vector<uint8_t> bytecode = {
      kJMP_ABS, U16(0xABCD)  //
  };

  MAKE_CPU(bytecode);

  cpu.advance_cycles(3);

  REQUIRE(cpu.PC() == 0xABCD);
  REQUIRE(cpu.P() == 0x34);
}

TEST_CASE("Unit: JMP_IND") {
  std::vector<uint8_t> bytecode = {
      kJMP_IND, U16(0x0010)  //
  };

  MAKE_CPU(bytecode);

  cpu.write16(0x0010, 0x1234);

  REQUIRE(cpu.read16(0x010) == 0x1234);

  cpu.advance_cycles(5);

  REQUIRE(cpu.PC() == 0x1234);
  REQUIRE(cpu.P() == 0x34);
}

TEST_CASE("Unit: JSR_ABS") {
  std::vector<uint8_t> bytecode = {
      kJSR_ABS, U16(0x1234),  //
  };

  MAKE_CPU(bytecode);

  REQUIRE(cpu.SP() == 0xFD);
  uint16_t old_pc = cpu.PC();

  cpu.advance_instruction();

  CAPTURE(old_pc);

  REQUIRE(cpu.SP() == 0xFB);
  REQUIRE(cpu.peek_stack16() == old_pc + 2);
}

TEST_CASE("Unit: LDA_IMM") {
  SECTION("Postitive") {
    std::vector<uint8_t> bytecode = {
        kLDA_IMM, 0x09  //
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
        kLDA_IMM, 0x91  //
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
        kLDA_IMM, 0x00  //
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
      kLDA_ZP, 0x05  //
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
      kLDX_IMM, 0x02,  //
      kLDA_ZPX, 0x05   //
  };

  MAKE_CPU(bytecode);

  // LDX_IMM
  cpu.cycle();
  cpu.cycle();

  // LDA_ZPX - 1
  cpu.cycle();
  cpu.cycle();
  cpu.cycle();

  REQUIRE(cpu.A() == 0);
  REQUIRE(cpu.X() == 2);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());

  SECTION("Positive") {
    cpu.write(0x07, 0x09);

    cpu.cycle();

    REQUIRE(cpu.A() == 9);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Negative") {
    cpu.write(0x07, 0x91);

    cpu.cycle();

    REQUIRE(cpu.A() == 0x91);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE(cpu.get_negative());
  };

  SECTION("Zero flag") {
    cpu.write(0x07, 0x00);

    cpu.cycle();

    REQUIRE(cpu.A() == 0x00);
    REQUIRE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Wrap around") {
    std::vector<uint8_t> wrap_bytecode = {
        kLDX_IMM, 0xFF,  //
        kLDA_ZPX, 0x05   //
    };

    MAKE_CPU(wrap_bytecode);

    cpu.write(0x04, 0x42);

    // LDX_IMM
    cpu.cycle();
    cpu.cycle();

    // LDA_ZPX
    cpu.cycle();
    cpu.cycle();
    cpu.cycle();
    cpu.cycle();

    REQUIRE(cpu.A() == 0x42);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };
}

TEST_CASE("Unit: LDA_ABS") {
  std::vector<uint8_t> bytecode = {
      kLDA_ABS, 0x02, 0x00  //
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

TEST_CASE("Unit: LDA_ABSX") {
  std::vector<uint8_t> bytecode = {
      kLDX_IMM, 0x01,         //
      kLDA_ABSX, U16(0x0004)  //
  };

  MAKE_CPU(bytecode);

  // LDX_IMM
  cpu.cycle();
  cpu.cycle();

  // LDA_ABSX - 1
  cpu.cycle();
  cpu.cycle();
  cpu.cycle();

  REQUIRE(cpu.X() == 1);
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

  SECTION("Cross Page") {
    std::vector<uint8_t> bytecode = {
        kLDX_IMM, 0x01,         //
        kLDA_ABSX, U16(0x01FF)  //
    };

    MAKE_CPU(bytecode);

    // LDX
    cpu.cycle();
    cpu.cycle();

    // LDA - 1
    cpu.cycle();
    cpu.cycle();
    cpu.cycle();
    cpu.cycle();

    REQUIRE(cpu.A() == 0);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
    cpu.write(0x0200, 0x09);

    cpu.cycle();

    REQUIRE(cpu.A() == 9);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };
}

TEST_CASE("Unit: LDA_ABSY") {
  std::vector<uint8_t> bytecode = {
      kLDY_IMM, 0x01,         //
      kLDA_ABSY, U16(0x0004)  //
  };

  MAKE_CPU(bytecode);

  // LDY_IMM
  cpu.cycle();
  cpu.cycle();

  // LDA_ABSY - 1
  cpu.cycle();
  cpu.cycle();
  cpu.cycle();

  REQUIRE(cpu.Y() == 1);
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

  SECTION("Cross Page") {
    std::vector<uint8_t> bytecode = {
        kLDY_IMM, 0x01,         //
        kLDA_ABSY, U16(0x01FF)  //
    };

    MAKE_CPU(bytecode);

    // LDY
    cpu.cycle();
    cpu.cycle();

    // LDA - 1
    cpu.cycle();
    cpu.cycle();
    cpu.cycle();
    cpu.cycle();

    REQUIRE(cpu.A() == 0);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
    cpu.write(0x0200, 0x09);

    cpu.cycle();

    REQUIRE(cpu.A() == 9);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };
}

TEST_CASE("Unit: LDA_INDX") {
  std::vector<uint8_t> bytecode = {
      kLDX_IMM, 0x01,         //
      kLDA_INDX, U16(0x0004)  //
  };

  MAKE_CPU(bytecode);

  // LDX_IMM
  cpu.cycle();
  cpu.cycle();

  // LDA_INDX - 1
  cpu.cycle();
  cpu.cycle();
  cpu.cycle();
  cpu.cycle();
  cpu.cycle();

  cpu.write(0x05, 0x07);

  REQUIRE(cpu.X() == 1);
  REQUIRE(cpu.A() == 0);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());

  SECTION("Positive") {
    cpu.write(0x07, 0x09);

    cpu.cycle();

    REQUIRE(cpu.A() == 9);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Negative") {
    cpu.write(0x07, 0x91);

    cpu.cycle();

    REQUIRE(cpu.A() == 0x91);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE(cpu.get_negative());
  };

  SECTION("Zero flag") {
    cpu.write(0x07, 0x00);

    cpu.cycle();

    REQUIRE(cpu.A() == 0x00);
    REQUIRE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };
}

TEST_CASE("Unit: LDA_INDY") {
  std::vector<uint8_t> bytecode = {
      kLDY_IMM, 0x01,         //
      kLDA_INDY, U16(0x0005)  //
  };

  MAKE_CPU(bytecode);

  // LDY_IMM
  cpu.cycle();
  cpu.cycle();

  // LDA_INDY - 1
  cpu.cycle();
  cpu.cycle();
  cpu.cycle();
  cpu.cycle();

  cpu.write(0x05, 0x06);

  REQUIRE(cpu.Y() == 1);
  REQUIRE(cpu.A() == 0);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());

  SECTION("Positive") {
    cpu.write(0x07, 0x09);

    cpu.cycle();

    REQUIRE(cpu.A() == 9);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Negative") {
    cpu.write(0x07, 0x91);

    cpu.cycle();

    REQUIRE(cpu.A() == 0x91);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE(cpu.get_negative());
  };

  SECTION("Zero flag") {
    cpu.write(0x07, 0x00);

    cpu.cycle();

    REQUIRE(cpu.A() == 0x00);
    REQUIRE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };
}

TEST_CASE("Unit: LDX_IMM") {
  SECTION("Postitive") {
    std::vector<uint8_t> bytecode = {
        kLDX_IMM, 0x09  //
    };

    MAKE_CPU(bytecode);

    cpu.cycle();

    REQUIRE(cpu.X() == 0);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());

    cpu.cycle();

    REQUIRE(cpu.X() == 9);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Negative") {
    std::vector<uint8_t> bytecode = {
        kLDX_IMM, 0x91  //
    };

    MAKE_CPU(bytecode);

    cpu.cycle();

    REQUIRE(cpu.X() == 0);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());

    cpu.cycle();

    REQUIRE(cpu.X() == 0x91);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE(cpu.get_negative());
  };

  SECTION("Zero flag") {
    std::vector<uint8_t> bytecode = {
        kLDX_IMM, 0x00  //
    };

    MAKE_CPU(bytecode);

    cpu.cycle();

    REQUIRE(cpu.X() == 0);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
    cpu.cycle();

    REQUIRE(cpu.X() == 0);
    REQUIRE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };
}

TEST_CASE("Unit: LDX_ZP") {
  std::vector<uint8_t> bytecode = {
      kLDX_ZP, 0x05  //
  };

  MAKE_CPU(bytecode);

  cpu.cycle();
  cpu.cycle();

  REQUIRE(cpu.X() == 0);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());

  SECTION("Positive") {
    cpu.write(0x05, 0x09);

    cpu.cycle();

    REQUIRE(cpu.X() == 9);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Negative") {
    cpu.write(0x05, 0x91);

    cpu.cycle();

    REQUIRE(cpu.X() == 0x91);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE(cpu.get_negative());
  };

  SECTION("Zero flag") {
    cpu.write(0x05, 0x00);

    cpu.cycle();

    REQUIRE(cpu.X() == 0x00);
    REQUIRE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };
}

TEST_CASE("Unit: LDX_ZPY") {
  std::vector<uint8_t> bytecode = {
      kLDX_ZPY, 0x05  //
  };

  MAKE_CPU(bytecode);

  cpu.cycle();
  cpu.cycle();
  cpu.cycle();

  REQUIRE(cpu.X() == 0);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());

  SECTION("Positive") {
    cpu.write(0x05, 0x09);

    cpu.cycle();

    REQUIRE(cpu.X() == 9);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Negative") {
    cpu.write(0x05, 0x91);

    cpu.cycle();

    REQUIRE(cpu.X() == 0x91);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE(cpu.get_negative());
  };

  SECTION("Zero flag") {
    cpu.write(0x05, 0x00);

    cpu.cycle();

    REQUIRE(cpu.X() == 0x00);
    REQUIRE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };
}

TEST_CASE("Unit: LDX_ABS") {
  std::vector<uint8_t> bytecode = {
      kLDX_ABS, U16(0x0005)  //
  };

  MAKE_CPU(bytecode);

  cpu.cycle();
  cpu.cycle();
  cpu.cycle();

  REQUIRE(cpu.X() == 0);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());

  SECTION("Positive") {
    cpu.write(0x05, 0x09);

    cpu.cycle();

    REQUIRE(cpu.X() == 9);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Negative") {
    cpu.write(0x05, 0x91);

    cpu.cycle();

    REQUIRE(cpu.X() == 0x91);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE(cpu.get_negative());
  };

  SECTION("Zero flag") {
    cpu.write(0x05, 0x00);

    cpu.cycle();

    REQUIRE(cpu.X() == 0x00);
    REQUIRE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };
}

TEST_CASE("Unit: LDX_ABSY") {
  std::vector<uint8_t> bytecode = {
      kLDX_ABSY, U16(0x0005)  //
  };

  MAKE_CPU(bytecode);

  cpu.cycle();
  cpu.cycle();
  cpu.cycle();

  REQUIRE(cpu.X() == 0);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());

  SECTION("Positive") {
    cpu.write(0x05, 0x09);

    cpu.cycle();

    REQUIRE(cpu.X() == 9);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Negative") {
    cpu.write(0x05, 0x91);

    cpu.cycle();

    REQUIRE(cpu.X() == 0x91);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE(cpu.get_negative());
  };

  SECTION("Zero flag") {
    cpu.write(0x05, 0x00);

    cpu.cycle();

    REQUIRE(cpu.X() == 0x00);
    REQUIRE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Cross Page") {
    std::vector<uint8_t> bytecode = {
        kLDY_IMM, 0x01,         //
        kLDX_ABSY, U16(0x01FF)  //
    };

    MAKE_CPU(bytecode);

    // LDY
    cpu.cycle();
    cpu.cycle();

    // LDX - 1
    cpu.cycle();
    cpu.cycle();
    cpu.cycle();
    cpu.cycle();

    REQUIRE(cpu.X() == 0);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
    cpu.write(0x0200, 0x09);

    cpu.cycle();

    REQUIRE(cpu.X() == 9);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };
}

TEST_CASE("Unit: LDY_IMM") {
  SECTION("Postitive") {
    std::vector<uint8_t> bytecode = {
        kLDY_IMM, 0x09  //
    };

    MAKE_CPU(bytecode);

    cpu.cycle();

    REQUIRE(cpu.Y() == 0);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());

    cpu.cycle();

    REQUIRE(cpu.Y() == 9);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Negative") {
    std::vector<uint8_t> bytecode = {
        kLDY_IMM, 0x91  //
    };

    MAKE_CPU(bytecode);

    cpu.cycle();

    REQUIRE(cpu.Y() == 0);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());

    cpu.cycle();

    REQUIRE(cpu.Y() == 0x91);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE(cpu.get_negative());
  };

  SECTION("Zero flag") {
    std::vector<uint8_t> bytecode = {
        kLDY_IMM, 0x00  //
    };

    MAKE_CPU(bytecode);

    cpu.cycle();

    REQUIRE(cpu.Y() == 0);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
    cpu.cycle();

    REQUIRE(cpu.Y() == 0);
    REQUIRE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };
}

TEST_CASE("Unit: LDY_ZP") {
  std::vector<uint8_t> bytecode = {
      kLDY_ZP, 0x05  //
  };

  MAKE_CPU(bytecode);

  cpu.cycle();
  cpu.cycle();

  REQUIRE(cpu.Y() == 0);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());

  SECTION("Positive") {
    cpu.write(0x05, 0x09);

    cpu.cycle();

    REQUIRE(cpu.Y() == 9);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Negative") {
    cpu.write(0x05, 0x91);

    cpu.cycle();

    REQUIRE(cpu.Y() == 0x91);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE(cpu.get_negative());
  };

  SECTION("Zero flag") {
    cpu.write(0x05, 0x00);

    cpu.cycle();

    REQUIRE(cpu.Y() == 0x00);
    REQUIRE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };
}

TEST_CASE("Unit: LDY_ZPX") {
  std::vector<uint8_t> bytecode = {
      kLDX_IMM, 0x02,  //
      kLDY_ZPX, 0x05   //
  };

  MAKE_CPU(bytecode);

  // LDX_IMM
  cpu.cycle();
  cpu.cycle();

  // LDA_ZPX - 1
  cpu.cycle();
  cpu.cycle();
  cpu.cycle();

  REQUIRE(cpu.Y() == 0);
  REQUIRE(cpu.X() == 2);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());

  SECTION("Positive") {
    cpu.write(0x07, 0x09);

    cpu.cycle();

    REQUIRE(cpu.Y() == 9);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Negative") {
    cpu.write(0x07, 0x91);

    cpu.cycle();

    REQUIRE(cpu.Y() == 0x91);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE(cpu.get_negative());
  };

  SECTION("Zero flag") {
    cpu.write(0x07, 0x00);

    cpu.cycle();

    REQUIRE(cpu.Y() == 0x00);
    REQUIRE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Wrap around") {
    std::vector<uint8_t> wrap_bytecode = {
        kLDX_IMM, 0xFF,  //
        kLDY_ZPX, 0x05   //
    };

    MAKE_CPU(wrap_bytecode);

    cpu.write(0x04, 0x42);

    // LDX_IMM
    cpu.cycle();
    cpu.cycle();

    // LDY_ZPX
    cpu.cycle();
    cpu.cycle();
    cpu.cycle();
    cpu.cycle();

    REQUIRE(cpu.Y() == 0x42);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };
}

TEST_CASE("Unit: LDY_ABS") {
  std::vector<uint8_t> bytecode = {
      kLDY_ABS, U16(0x0005)  //
  };

  MAKE_CPU(bytecode);

  cpu.cycle();
  cpu.cycle();
  cpu.cycle();

  REQUIRE(cpu.Y() == 0);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());

  SECTION("Positive") {
    cpu.write(0x05, 0x09);

    cpu.cycle();

    REQUIRE(cpu.Y() == 9);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Negative") {
    cpu.write(0x05, 0x91);

    cpu.cycle();

    REQUIRE(cpu.Y() == 0x91);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE(cpu.get_negative());
  };

  SECTION("Zero flag") {
    cpu.write(0x05, 0x00);

    cpu.cycle();

    REQUIRE(cpu.Y() == 0x00);
    REQUIRE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };
}

TEST_CASE("Unit: LDY_ABSX") {
  std::vector<uint8_t> bytecode = {
      kLDX_IMM, 0x01,         //
      kLDY_ABSX, U16(0x0004)  //
  };

  MAKE_CPU(bytecode);

  // LDX_IMM
  cpu.cycle();
  cpu.cycle();

  // LDY_ABSX - 1
  cpu.cycle();
  cpu.cycle();
  cpu.cycle();

  REQUIRE(cpu.Y() == 0);
  REQUIRE_FALSE(cpu.get_zero());
  REQUIRE_FALSE(cpu.get_negative());

  SECTION("Positive") {
    cpu.write(0x05, 0x09);

    cpu.cycle();

    REQUIRE(cpu.Y() == 9);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Negative") {
    cpu.write(0x05, 0x91);

    cpu.cycle();

    REQUIRE(cpu.Y() == 0x91);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE(cpu.get_negative());
  };

  SECTION("Zero flag") {
    cpu.write(0x05, 0x00);

    cpu.cycle();

    REQUIRE(cpu.Y() == 0x00);
    REQUIRE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Cross Page") {
    std::vector<uint8_t> bytecode = {
        kLDX_IMM, 0x01,         //
        kLDY_ABSX, U16(0x01FF)  //
    };

    MAKE_CPU(bytecode);

    // LDX_IMM
    cpu.cycle();
    cpu.cycle();

    // LDY_ABSX - 1
    cpu.cycle();
    cpu.cycle();
    cpu.cycle();
    cpu.cycle();

    REQUIRE(cpu.Y() == 0);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
    cpu.write(0x0200, 0x09);

    cpu.cycle();

    REQUIRE(cpu.Y() == 9);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };
}

TEST_CASE("Unit: LSR_A") {}

TEST_CASE("Unit: LSR_ZP") {}

TEST_CASE("Unit: LSR_ZPX") {}

TEST_CASE("Unit: LSR_ABS") {}

TEST_CASE("Unit: LSR_ABSX") {}

TEST_CASE("Unit: NOP") {
  std::vector<uint8_t> bytecode = {
      kNOP,  //
  };

  MAKE_CPU(bytecode);

  REQUIRE(cpu.P() == 0x34);
  const uint16_t pc = cpu.PC();

  cpu.advance_cycles(2);

  REQUIRE(cpu.P() == 0x34);
  REQUIRE(cpu.PC() == pc + 1);
}

TEST_CASE("Unit: ORA_IMM") {
  SECTION("Positive") {
    std::vector<uint8_t> bytecode = {
        kLDA_IMM, 0b1001,  //
        kORA_IMM, 0b1100,  //
    };

    MAKE_CPU(bytecode);

    REQUIRE(cpu.A() == 0);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());

    // kLDA_IMM
    cpu.cycle();
    cpu.cycle();

    // kORA_IMM - 1
    cpu.cycle();

    REQUIRE(cpu.A() == 0b1001);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());

    // kORA_IMM
    cpu.cycle();

    REQUIRE(cpu.A() == 0b1101);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Negative") {
    std::vector<uint8_t> bytecode = {
        kLDA_IMM, 0b1001,       //
        kORA_IMM, 0b1000'1100,  //
    };

    MAKE_CPU(bytecode);

    // kLDA_IMM
    cpu.cycle();
    cpu.cycle();

    // kORA_IMM - 1
    cpu.cycle();

    REQUIRE(cpu.A() == 0b1001);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());

    // kORA_IMM
    cpu.cycle();

    REQUIRE(cpu.A() == 0b1000'1101);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE(cpu.get_negative());
  };

  SECTION("Zero") {
    std::vector<uint8_t> bytecode = {
        kLDA_IMM, 0,  //
        kORA_IMM, 0,  //
    };

    MAKE_CPU(bytecode);

    // kLDA_IMM
    cpu.cycle();
    cpu.cycle();

    // kORA_IMM - 1
    cpu.cycle();

    REQUIRE(cpu.A() == 0);
    REQUIRE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());

    // kORA_IMM
    cpu.cycle();

    REQUIRE(cpu.A() == 0);
    REQUIRE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };
}

TEST_CASE("Unit: ORA_ZP") {
  SECTION("Positive") {
    std::vector<uint8_t> bytecode = {
        kLDX_IMM, 0b0110'0100,  //
        kSTX_ZP,  0x10,         //

        kLDA_IMM, 0b0011'1001,  //
        kORA_ZP,  0x10,         //
    };

    MAKE_CPU(bytecode);

    REQUIRE(cpu.A() == 0);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());

    cpu.advance_instruction();
    cpu.advance_instruction();
    cpu.advance_instruction();
    cpu.advance_instruction();

    REQUIRE(cpu.A() == 0b0111'1101);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };

  SECTION("Negative") {
    std::vector<uint8_t> bytecode = {
        kLDX_IMM, 0b1000'0100,  //
        kSTX_ZP,  0x10,         //

        kLDA_IMM, 0b0000'1001,  //
        kORA_ZP,  0x10,         //
    };

    MAKE_CPU(bytecode);

    REQUIRE(cpu.A() == 0);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());

    cpu.advance_instruction();
    cpu.advance_instruction();
    cpu.advance_instruction();
    cpu.advance_instruction();

    REQUIRE(cpu.A() == 0b1000'1101);
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE(cpu.get_negative());
  };

  SECTION("Zero") {
    std::vector<uint8_t> bytecode = {
        kLDX_IMM, 0,     //
        kSTX_ZP,  0x10,  //

        kLDA_IMM, 0,     //
        kORA_ZP,  0x10,  //
    };

    MAKE_CPU(bytecode);

    cpu.advance_instruction();
    cpu.advance_instruction();
    cpu.advance_instruction();
    cpu.advance_instruction();

    REQUIRE(cpu.A() == 0);
    REQUIRE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  };
}

TEST_CASE("Unit: ORA_ZPX") {}

TEST_CASE("Unit: ORA_ABS") {}

TEST_CASE("Unit: ORA_ABSX") {}

TEST_CASE("Unit: ORA_ABSY") {}

TEST_CASE("Unit: ORA_INDX") {}

TEST_CASE("Unit: ORA_INDY") {}

TEST_CASE("Unit: PHA") {
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, 0x09,  //
      kPHA,            //
      kLDA_IMM, 0x0A,  //
      kPHA,            //
  };

  MAKE_CPU(bytecode);

  cpu.advance_instruction();
  REQUIRE(cpu.A() == 0x09);

  cpu.advance_instruction();
  REQUIRE(cpu.read(0x100 + cpu.SP() + 1) == 0x09);

  cpu.advance_instruction();
  REQUIRE(cpu.A() == 0x0A);

  cpu.advance_instruction();
  REQUIRE(cpu.read(0x100 + cpu.SP() + 1) == 0x0A);
}

TEST_CASE("Unit: PHP") {
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, 0x7F,  //
      kADC_IMM, 0x01,  //
      kPHP,            //
  };

  MAKE_CPU(bytecode);

  uint8_t status_before = cpu.P();

  cpu.advance_instruction();
  cpu.advance_instruction();
  uint8_t status_after = cpu.P();
  REQUIRE(status_after != status_before);

  cpu.advance_instruction();
  REQUIRE(cpu.read(0x100 + cpu.SP() + 1) == status_after);
}

TEST_CASE("Unit: PLA") {
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, 0x09,  //
      kPHA,            //
      kLDA_IMM, 0x0A,  //
      kPHA,            //
      kLDA_IMM, 0x00,  //
      kPLA,            //
      kPLA,            //
  };

  MAKE_CPU(bytecode);

  cpu.advance_instruction();
  REQUIRE(cpu.A() == 0x09);
  cpu.advance_instruction();

  cpu.advance_instruction();
  REQUIRE(cpu.A() == 0x0A);
  cpu.advance_instruction();

  cpu.advance_instruction();
  REQUIRE(cpu.A() == 0x00);

  cpu.advance_instruction();
  REQUIRE(cpu.A() == 0x0A);

  cpu.advance_instruction();
  REQUIRE(cpu.A() == 0x09);
}

TEST_CASE("Unit: PLP") {
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, 0x7F,  //
      kADC_IMM, 0x01,  //
      kPHP,            //
      kLDA_IMM, 0xFF,  //
      kADC_IMM, 0x01,  //
      kPLP,            //
  };

  MAKE_CPU(bytecode);

  cpu.advance_instruction();
  cpu.advance_instruction();
  uint8_t status_before = cpu.P();

  cpu.advance_instruction();
  REQUIRE(cpu.read(0x100 + cpu.SP() + 1) == status_before);

  cpu.advance_instruction();
  cpu.advance_instruction();
  uint8_t status_after = cpu.P();
  REQUIRE(status_after != status_before);

  cpu.advance_instruction();
  REQUIRE(cpu.P() == status_before);
}

TEST_CASE("Unit: ROL_A") {
  SECTION("Only carry set") {
    std::vector<uint8_t> bytecode = {
        kSEC,           // set carry flag
        kLDA_IMM, 0b0,  //
        kROL_A,         // shift bits in A one to the left
    };

    MAKE_CPU(bytecode);

    cpu.advance_instruction();
    cpu.advance_instruction();

    REQUIRE(cpu.get_carry());
    REQUIRE(cpu.A() == 0);

    cpu.advance_instruction();

    REQUIRE(cpu.A() == 0b1);
    REQUIRE_FALSE(cpu.get_carry());
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  }

  SECTION("All but carry") {
    std::vector<uint8_t> bytecode = {
        // carry not set
        kLDA_IMM, 0b1111'1111,  //
        kROL_A,                 // shift bits in A one to the left
    };

    MAKE_CPU(bytecode);

    REQUIRE_FALSE(cpu.get_carry());

    cpu.advance_instruction();
    cpu.advance_instruction();

    REQUIRE(cpu.A() == 0b1111'1110);
    REQUIRE(cpu.get_carry());
    REQUIRE_FALSE(cpu.get_zero());
  }

  SECTION("Zero") {
    std::vector<uint8_t> bytecode = {
        // carry not set
        kLDA_IMM, 0b0,  //
        kROL_A,         // shift bits in A one to the left
    };

    MAKE_CPU(bytecode);

    REQUIRE_FALSE(cpu.get_carry());

    cpu.advance_instruction();
    cpu.advance_instruction();

    REQUIRE(cpu.A() == 0);
    REQUIRE_FALSE(cpu.get_carry());
    REQUIRE(cpu.get_zero());
  }
}

TEST_CASE("Unit: ROL_ZP") {}

TEST_CASE("Unit: ROL_ZPX") {}

TEST_CASE("Unit: ROL_ABS") {}

TEST_CASE("Unit: ROL_ABSX") {}

TEST_CASE("Unit: ROR_A") {
  SECTION("Only carry set") {
    std::vector<uint8_t> bytecode = {
        kSEC,           // set carry flag
        kLDA_IMM, 0b0,  //
        kROR_A,         // shift bits in A one to the left
    };

    MAKE_CPU(bytecode);

    cpu.advance_instruction();
    cpu.advance_instruction();

    REQUIRE(cpu.get_carry());
    REQUIRE(cpu.A() == 0);

    cpu.advance_instruction();

    REQUIRE(cpu.A() == 0b1000'0000);
    REQUIRE_FALSE(cpu.get_carry());
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE(cpu.get_negative());
  }

  SECTION("All but carry") {
    std::vector<uint8_t> bytecode = {
        // carry not set
        kLDA_IMM, 0b1111'1111,  //
        kROR_A,                 // shift bits in A one to the left
    };

    MAKE_CPU(bytecode);

    REQUIRE_FALSE(cpu.get_carry());

    cpu.advance_instruction();
    cpu.advance_instruction();

    REQUIRE(cpu.A() == 0b0111'1111);
    REQUIRE(cpu.get_carry());
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_negative());
  }
}

TEST_CASE("Unit: ROR_ZP") {}

TEST_CASE("Unit: ROR_ZPX") {}

TEST_CASE("Unit: ROR_ABS") {}

TEST_CASE("Unit: ROR_ABSX") {}

TEST_CASE("Unit: RTI") {
  uint16_t addr = 0x1234;
  std::vector<uint8_t> bytecode = {
      kLDA_IMM,    0x01,  //
      kADC_IMM,    0xFF,  //
      kBRK,               //
      U16(0x5678),
  };

  std::unique_ptr<VectorMapper> __mapper(new VectorMapper(bytecode, addr));
  cartridge::Cartridge __cart(std::move(__mapper));
  CPU cpu(__cart);

  std::vector<uint8_t> function = {
      kLDA_IMM, 0x01,  //
      kADC_IMM, 0x7F,  //
      kRTI,            //
  };
  uint16_t p = addr;
  for (auto i : function) {
    cpu.write(p++, i);
  }

  REQUIRE(cpu.A() == 0x00);

  cpu.advance_instruction();
  REQUIRE(cpu.A() == 0x01);

  cpu.advance_instruction();
  REQUIRE(cpu.A() == 0x00);
  uint8_t flags_outer = cpu.P();

  cpu.advance_instruction();
  REQUIRE(fmt_hex(cpu.PC()) == fmt_hex(addr));

  cpu.advance_instruction();
  REQUIRE(cpu.A() == 0x01);

  cpu.advance_instruction();
  REQUIRE((int)cpu.A() == 0x7F + 0x01 + 1);  // carry bit is set!
  uint8_t flags_inner = cpu.P();
  CAPTURE(fmt_hex(flags_inner), fmt_hex(flags_outer));
  REQUIRE(flags_inner != flags_outer);

  cpu.advance_instruction();
  CAPTURE(fmt_hex(cpu.PC()), fmt_hex(cpu.read16(cpu.PC())));
  REQUIRE(cpu.read16(cpu.PC()) == 0x5678);
  REQUIRE(cpu.P() == flags_outer);
}

TEST_CASE("Unit: RTS") {
  uint16_t addr = 0x1234;
  std::vector<uint8_t> bytecode = {
      kLDA_IMM,    0x01,       //
      kADC_IMM,    0xFF,       //
      kJSR_ABS,    U16(addr),  //
      U16(0x5678),
  };

  MAKE_CPU(bytecode);

  std::vector<uint8_t> function = {
      kLDA_IMM, 0x08,  //
      kADC_IMM, 0x7F,  //
      kRTS,            //
  };
  uint16_t p = addr;
  for (auto i : function) {
    cpu.write(p++, i);
  }

  REQUIRE(cpu.A() == 0x00);

  cpu.advance_instruction();
  REQUIRE(cpu.A() == 0x01);

  cpu.advance_instruction();
  REQUIRE(cpu.A() == 0x00);

  cpu.advance_instruction();
  REQUIRE(fmt_hex(cpu.PC()) == fmt_hex(addr));

  cpu.advance_instruction();
  REQUIRE(cpu.A() == 0x08);

  cpu.advance_instruction();
  REQUIRE((int)cpu.A() == 0x7F + 0x08 + 1);  // carry bit is set!

  cpu.advance_instruction();
  REQUIRE(cpu.read16(cpu.PC()) == 0x5678);
}

TEST_CASE("Unit: SBC_IMM") {
  SECTION("Positive, no carry") {
    std::vector<uint8_t> bytecode = {
        kLDA_IMM, 20,  //
        kSBC_IMM, 5    //
    };

    MAKE_CPU(bytecode);

    cpu.advance_instruction();
    cpu.advance_instruction();

    // - 1 because carry is 0
    REQUIRE(cpu.A() == 20 - 5 - 1);
    REQUIRE_FALSE(cpu.get_carry());
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_overflow());
    REQUIRE_FALSE(cpu.get_negative());
  }

  SECTION("With carry") {
    std::vector<uint8_t> bytecode = {
        kLDA_IMM, 20,  //
        kSEC,          //
        kSBC_IMM, 5    //
    };

    MAKE_CPU(bytecode);

    cpu.advance_instruction();
    cpu.advance_instruction();
    cpu.advance_instruction();

    // - 1 because carry is 0
    REQUIRE(cpu.A() == 20 - 5 - 0);
    REQUIRE_FALSE(cpu.get_carry());
    REQUIRE_FALSE(cpu.get_zero());
    REQUIRE_FALSE(cpu.get_overflow());
    REQUIRE_FALSE(cpu.get_negative());
  }

  // TODO: overflow, zero
}

TEST_CASE("Unit: SBC_ZP") {}

TEST_CASE("Unit: SBC_ZPX") {}

TEST_CASE("Unit: SBC_ABS") {}

TEST_CASE("Unit: SBC_ABSX") {}

TEST_CASE("Unit: SBC_ABSY") {}

TEST_CASE("Unit: SBC_INDX") {}

TEST_CASE("Unit: SBC_INDY") {}

TEST_CASE("Unit: SEC") {
  std::vector<uint8_t> bytecode = {
      kSEC,  //
  };

  MAKE_CPU(bytecode);

  REQUIRE_FALSE(cpu.get_carry());

  cpu.advance_cycles(2);

  REQUIRE(cpu.get_carry());
}

TEST_CASE("Unit: SED") {
  std::vector<uint8_t> bytecode = {
      kSED,  //
  };

  MAKE_CPU(bytecode);

  REQUIRE_FALSE(cpu.get_decimal());

  cpu.advance_cycles(2);

  REQUIRE(cpu.get_decimal());
}

TEST_CASE("Unit: SEI") {
  std::vector<uint8_t> bytecode = {
      kCLI,  //
      kSEI,  //
  };

  MAKE_CPU(bytecode);

  // Interupt Disable flag is set by default
  REQUIRE(cpu.get_interrupt_disable());

  cpu.advance_instruction();

  REQUIRE_FALSE(cpu.get_interrupt_disable());

  cpu.advance_cycles(2);

  REQUIRE(cpu.get_interrupt_disable());
}

TEST_CASE("Unit: STA_ZP") {
  uint8_t addr = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF);
  uint8_t data = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF);
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, data,  //
      kSTA_ZP, addr,   //
  };

  MAKE_CPU(bytecode);
  cpu.write(addr, data + 1);

  cpu.advance_cycles(2);
  REQUIRE(cpu.A() == data);
  auto flags = cpu.P();

  cpu.advance_cycles(3);
  REQUIRE(cpu.read(addr) == data);
  REQUIRE(cpu.A() == data);
  REQUIRE(cpu.P() == flags);
}

TEST_CASE("Unit: STA_ZPX") {
  uint8_t addr = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF - 0x05);
  uint8_t data = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF);
  uint8_t offset = GENERATE(0x00, 0x01, 0x05);
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, data,    //
      kLDX_IMM, offset,  //
      kSTA_ZPX, addr,    //
  };

  MAKE_CPU(bytecode);
  cpu.write(addr + offset, data + 1);

  cpu.advance_cycles(2);
  REQUIRE(cpu.A() == data);

  cpu.advance_cycles(2);
  REQUIRE(cpu.X() == offset);

  cpu.advance_cycles(4);
  REQUIRE((int)cpu.read(addr + offset) == (int)data);
}

TEST_CASE("Unit: STA_ABS") {
  uint16_t addr = GENERATE(0x0000, 0x0001, 0x00FF, 0x0100, 0x1000, 0x1FFF);
  uint8_t data = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF);
  CAPTURE(fmt_hex(addr), fmt_hex(data));

  std::vector<uint8_t> bytecode = {
      kLDA_IMM, data,       //
      kSTA_ABS, U16(addr),  //
  };

  MAKE_CPU(bytecode);
  cpu.write(addr, data + 1);

  cpu.advance_cycles(2);
  REQUIRE(cpu.A() == data);

  cpu.advance_cycles(4);
  REQUIRE((int)cpu.read(addr) == (int)data);
}

TEST_CASE("Unit: STA_ABSX") {
  uint16_t addr =
      GENERATE(0x0013, 0x00015, 0x00FF, 0x0100, 0x1000, 0x2000 - 0x05 - 1);
  uint8_t data = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF);
  uint8_t offset = GENERATE(0x00, 0x01, 0x05);
  CAPTURE(fmt_hex(addr), fmt_hex(data), fmt_hex(offset));

  std::vector<uint8_t> bytecode = {
      kLDA_IMM,  data,       //
      kLDX_IMM,  offset,     //
      kSTA_ABSX, U16(addr),  //
  };

  MAKE_CPU(bytecode);
  cpu.write(addr + offset, data + 1);

  cpu.advance_cycles(2);
  REQUIRE(cpu.A() == data);

  cpu.advance_cycles(2);
  REQUIRE(cpu.X() == offset);

  cpu.advance_cycles(5);
  REQUIRE((int)cpu.read(addr + offset) == (int)data);
}

TEST_CASE("Unit: STA_ABSY") {
  uint16_t addr =
      GENERATE(0x0000, 0x0001, 0x00FF, 0x0100, 0x1000, 0x1FFF - 0x05);
  uint8_t data = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF);
  uint8_t offset = GENERATE(0x00, 0x01, 0x05);
  CAPTURE(fmt_hex(addr), fmt_hex(data), fmt_hex(offset));

  std::vector<uint8_t> bytecode = {
      kLDA_IMM,  data,       //
      kLDY_IMM,  offset,     //
      kSTA_ABSY, U16(addr),  //
  };

  MAKE_CPU(bytecode);
  cpu.write(addr + offset, data + 1);

  cpu.advance_cycles(2);
  REQUIRE(cpu.A() == data);

  cpu.advance_cycles(2);
  REQUIRE(cpu.Y() == offset);

  cpu.advance_cycles(5);
  REQUIRE((int)cpu.read(addr + offset) == (int)data);
}

TEST_CASE("Unit: STA_INDX") {
  uint8_t zp_addr = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF - 0x10);
  uint16_t addr =
      GENERATE(0x000E, 0x0023, 0x00FA, 0x0100, 0x1000, 0x1111, 0x2000 - 2);
  uint8_t data = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF);
  uint8_t offset = GENERATE(0x00, 0x01, 0x05, 0x10);
  std::vector<uint8_t> bytecode = {
      kLDA_IMM,  data,     //
      kLDX_IMM,  offset,   //
      kSTA_INDX, zp_addr,  //
  };
  CAPTURE(fmt_hex(zp_addr), fmt_hex(addr), fmt_hex(data), fmt_hex(offset));

  MAKE_CPU(bytecode);
  cpu.write16(zp_addr + offset, addr);

  cpu.advance_cycles(2);
  REQUIRE(cpu.A() == data);

  cpu.advance_cycles(2);
  REQUIRE(cpu.X() == offset);

  cpu.advance_cycles(6);
  REQUIRE((int)cpu.read(addr) == (int)data);
}

TEST_CASE("Unit: STA_INDY") {
  uint8_t zp_addr = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF);
  uint16_t addr = GENERATE(0x000E, 0x0023, 0x00FA, 0x0100, 0x1000, 0x1111,
                           0x2000 - 2 - 0x10);
  uint8_t data = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF);
  uint8_t offset = GENERATE(0x00, 0x01, 0x05, 0x10);
  std::vector<uint8_t> bytecode = {
      kLDA_IMM,  data,     //
      kLDY_IMM,  offset,   //
      kSTA_INDY, zp_addr,  //
  };
  CAPTURE(fmt_hex(zp_addr), fmt_hex(addr), fmt_hex(data), fmt_hex(offset));

  MAKE_CPU(bytecode);
  cpu.write16(zp_addr, addr);

  cpu.advance_cycles(2);
  REQUIRE(cpu.A() == data);

  cpu.advance_cycles(2);
  REQUIRE(cpu.Y() == offset);

  cpu.advance_cycles(6);
  REQUIRE((int)cpu.read(addr + offset) == (int)data);
}

TEST_CASE("Unit: STX_ZP") {
  uint8_t addr = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF);
  uint8_t data = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF);
  std::vector<uint8_t> bytecode = {
      kLDX_IMM, data,  //
      kSTX_ZP, addr,   //
  };

  MAKE_CPU(bytecode);
  cpu.write(addr, data + 1);

  cpu.advance_cycles(2);
  REQUIRE(cpu.X() == data);
  auto flags = cpu.P();

  cpu.advance_cycles(3);
  REQUIRE((int)cpu.read(addr) == (int)data);
  REQUIRE(cpu.X() == data);
  REQUIRE(cpu.P() == flags);
}

TEST_CASE("Unit: STX_ZPY") {
  uint8_t addr = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF - 0x05);
  uint8_t data = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF);
  uint8_t offset = GENERATE(0x00, 0x01, 0x05);
  std::vector<uint8_t> bytecode = {
      kLDX_IMM, data,    //
      kLDY_IMM, offset,  //
      kSTX_ZPY, addr,    //
  };

  MAKE_CPU(bytecode);
  cpu.write(addr + offset, data + 1);

  cpu.advance_cycles(2);
  REQUIRE(cpu.X() == data);

  cpu.advance_cycles(2);
  REQUIRE(cpu.Y() == offset);

  cpu.advance_cycles(4);
  REQUIRE((int)cpu.read(addr + offset) == (int)data);
}

TEST_CASE("Unit: STX_ABS") {
  uint16_t addr = GENERATE(0x0000, 0x0001, 0x00FF, 0x0100, 0x1000, 0x1FFF);
  uint8_t data = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF);
  CAPTURE(fmt_hex(addr), fmt_hex(data));

  std::vector<uint8_t> bytecode = {
      kLDX_IMM, data,       //
      kSTX_ABS, U16(addr),  //
  };

  MAKE_CPU(bytecode);
  cpu.write(addr, data + 1);

  cpu.advance_cycles(2);
  REQUIRE(cpu.X() == data);

  cpu.advance_cycles(4);
  REQUIRE((int)cpu.read(addr) == (int)data);
}

TEST_CASE("Unit: STY_ZP") {
  uint8_t addr = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF);
  uint8_t data = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF);
  std::vector<uint8_t> bytecode = {
      kLDY_IMM, data,  //
      kSTY_ZP, addr,   //
  };

  MAKE_CPU(bytecode);
  cpu.write(addr, data + 1);

  cpu.advance_cycles(2);
  REQUIRE(cpu.Y() == data);
  auto flags = cpu.P();

  cpu.advance_cycles(3);
  REQUIRE((int)cpu.read(addr) == (int)data);
  REQUIRE(cpu.Y() == data);
  REQUIRE(cpu.P() == flags);
}

TEST_CASE("Unit: STY_ZPX") {
  uint8_t addr = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF - 0x05);
  uint8_t data = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF);
  uint8_t offset = GENERATE(0x00, 0x01, 0x05);
  std::vector<uint8_t> bytecode = {
      kLDY_IMM, data,    //
      kLDX_IMM, offset,  //
      kSTY_ZPX, addr,    //
  };

  MAKE_CPU(bytecode);
  cpu.write(addr + offset, data + 1);

  cpu.advance_cycles(2);
  REQUIRE(cpu.Y() == data);

  cpu.advance_cycles(2);
  REQUIRE(cpu.X() == offset);

  cpu.advance_cycles(4);
  REQUIRE((int)cpu.read(addr + offset) == (int)data);
}

TEST_CASE("Unit: STY_ABS") {
  uint16_t addr = GENERATE(0x0000, 0x0001, 0x00FF, 0x0100, 0x1000, 0x1FFF);
  uint8_t data = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF);
  CAPTURE(fmt_hex(addr), fmt_hex(data));

  std::vector<uint8_t> bytecode = {
      kLDY_IMM, data,       //
      kSTY_ABS, U16(addr),  //
  };

  MAKE_CPU(bytecode);
  cpu.write(addr, data + 1);

  cpu.advance_cycles(2);
  REQUIRE(cpu.Y() == data);

  cpu.advance_cycles(4);
  REQUIRE((int)cpu.read(addr) == (int)data);
}

TEST_CASE("Unit: TAX") {
  uint8_t data = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF);
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, data,  //
      kTAX,            //
  };
  MAKE_CPU(bytecode);

  cpu.advance_cycles(2);
  REQUIRE(cpu.A() == data);

  cpu.advance_cycles(2);
  REQUIRE(cpu.X() == data);
}

TEST_CASE("Unit: TAY") {
  uint8_t data = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF);
  std::vector<uint8_t> bytecode = {
      kLDA_IMM, data,  //
      kTAY,            //
  };

  MAKE_CPU(bytecode);

  cpu.advance_cycles(2);
  REQUIRE(cpu.A() == data);

  cpu.advance_cycles(2);
  REQUIRE(cpu.Y() == data);
}

TEST_CASE("Unit: TSX") {
  std::vector<uint8_t> bytecode = {
      kTSX,  //
  };

  MAKE_CPU(bytecode);
  REQUIRE((int)cpu.SP() == 0xFD);
  REQUIRE((int)cpu.X() != 0xFD);

  cpu.advance_cycles(2);
  REQUIRE((int)cpu.X() == 0xFD);
}

TEST_CASE("Unit: TXA") {
  uint8_t data = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA, 0xFF);
  std::vector<uint8_t> bytecode = {
      kLDX_IMM, data,  //
      kTXA,            //
  };

  MAKE_CPU(bytecode);

  cpu.advance_cycles(2);
  REQUIRE(cpu.X() == data);

  cpu.advance_cycles(2);
  REQUIRE(cpu.A() == data);
}

TEST_CASE("Unit: TXS") {
  uint8_t data = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA);
  std::vector<uint8_t> bytecode = {
      kLDX_IMM, data,  //
      kTXS,            //
  };

  MAKE_CPU(bytecode);

  cpu.advance_cycles(2);
  REQUIRE(cpu.X() == data);

  cpu.advance_cycles(2);
  REQUIRE((int)cpu.SP() == (int)data);
}

TEST_CASE("Unit: TYA") {
  uint8_t data = GENERATE(0x00, 0x01, 0x05, 0x10, 0x66, 0xAA);
  std::vector<uint8_t> bytecode = {
      kLDY_IMM, data,  //
      kTYA,            //
  };

  MAKE_CPU(bytecode);

  cpu.advance_cycles(2);
  REQUIRE(cpu.Y() == data);

  cpu.advance_cycles(2);
  REQUIRE(cpu.A() == data);
}

TEST_CASE("instruction printer") {
  uint16_t addr = 0x0000;
  std::vector<uint8_t> bytecode = {
      kJMP_ABS, U16(addr),  //
  };

  std::vector<std::pair<std::vector<uint8_t>, std::string_view>> instructions =
      {{{kNOP}, "NOP"},
       {{kROR_A}, "ROR A"},
       {{kLDA_IMM, 0x01}, "LDA #$01"},
       {{kADC_ZP, 0xAF}, "ADC $AF"},
       {{kSTY_ZPX, 0xD3}, "STY $D3,X"},
       {{kSTX_ZPY, 0x82}, "STX $82,Y"},
       {{kLDA_ABS, U16(0x1234)}, "LDA $1234"},
       {{kLDA_ABSX, U16(0x5678)}, "LDA $5678,X"},
       {{kLDA_ABSY, U16(0x9ABC)}, "LDA $9ABC,Y"},
       {{kJMP_IND, U16(0x0F78)}, "JMP ($0F78)"},
       {{kLDA_INDX, 0x42}, "LDA ($42,X)"},
       {{kLDA_INDY, 0x21}, "LDA ($21),Y"}};

  for (auto [instruction, expected] : instructions) {
    MAKE_CPU(bytecode);
    uint16_t p = addr;
    for (uint8_t b : instruction) {
      cpu.write(p++, b);
    }
    cpu.advance_cycles(3);
    REQUIRE(cpu.PC() == addr);
    CHECK(cpu.print_instruction() == expected);
  }
}
