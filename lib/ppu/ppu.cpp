#include "ppu.hpp"

// #include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <boost/log/trivial.hpp>
#include <format>

#include "util.hpp"

namespace ppu {
PPU::PPU(GLFWwindow& window) : internal_frame_buf_(), window_(window) {
  // Function called whenever glfw errors
  glfwSetErrorCallback([](int err, const char* desc) {
    BOOST_LOG_TRIVIAL(error) << "GLFW error" << err << ": " << desc << "\n";
  });
};

PPU::~PPU() {
  // Cleanup glfw: windows and everything
  glfwTerminate();
}

// TODO: where we gonna get cpu data. bus??
void PPU::render_to_window() {
  if (!internal_frame_buf_) {
    // Unallocated, allocate it
    try {
      internal_frame_buf_ = std::make_unique<frame_t>();
    } catch (const std::bad_alloc&) {
      BOOST_LOG_TRIVIAL(error) << "render_to_window: Error when allocating "
                                  "memory for internal framebuffer\n";
      throw;
    }
  }

  // Render current cpu data to internal buf
  render_to_framebuffer(*internal_frame_buf_);

  // TODO: draw it to the window

  // Actual size of window
  int width, height;
  // TODO: glfwSetFramebufferSizeCallback to be notified when window size
  // changes

  // Setup viewport (full window)
  glfwGetFramebufferSize(&window_, &width, &height);
  glViewport(0, 0, width, height);

  // Clear window contents
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // chatgpt told me to write these two lines

  glDrawPixels(kScreenWidth, kScreenHeight, GL_RGB, GL_UNSIGNED_BYTE,
               internal_frame_buf_->data());

  // glfw windows are double buffered
  glfwSwapBuffers(&window_);
  // process all waiting events
  glfwPollEvents();  // TODO: window refresh callback as in fn hint
}

void PPU::render_to_framebuffer(frame_t& out) {
  // From wiki:
  //       $3F00 	Universal background color
  // $3F01-$3F03 	Background palette 0
  // $3F05-$3F07 	Background palette 1
  // $3F09-$3F0B 	Background palette 2
  // $3F0D-$3F0F 	Background palette 3

  for (int y = 0; y < kScreenHeight; ++y) {
    for (int x = 0; x < kScreenWidth; ++x) {
      int i = y * kScreenWidth + x;

      // Calculate part of attribute table that corresponds to this pixel

      // Width is 256 pixels, attribute table is 8x8 chunks. Each byte in the
      // attribute table contains four colors for each of its four 16x16 px
      // quadrants.
      constexpr int byte_px_width = kScreenWidth / 8;

      // Coordinates of the attribute table entry this pixel belongs to. Between
      // 0 and 7
      uint8_t attr_y = y / byte_px_width;
      uint8_t attr_x = x / byte_px_width;
      assert(attr_y < 8 && attr_x < 8);

      // Coordinates of quadrant in byte; either 0 or 1
      uint8_t quadrant_y = (y / 16) % 2;
      uint8_t quadrant_x = (x / 16) % 2;
      uint8_t quadrant_idx = quadrant_y * 2 + quadrant_x;

      // Attribute table is 64 bytes at $23C0, a 8x8 byte array
      constexpr uint16_t attr_table_start = 0x23C0;

      // Nametable: indexes into pattern table
      // There are 4 palettes for background: 0 to 3
      // Attribute table selects which palette the given tile is using

      // Hardcoded to 1; bit 1 of four in palette (0 is transparent). This data
      // is from pattern table!
      uint8_t index_into_palette = 0x1;

      // Attribute is for palette selection. It's one byte for color selection
      // for each 16x16px quadrant of the 32x32px tile from current palette (2
      // bits for each quadrant)
      uint8_t attribute = read(attr_table_start + i * 8);

      // Mask color out of attribute depending on quadrant
      pixel_t quadrant_color_idx = (attribute >> quadrant_idx) & 0b11;

      // Use color index in current frame palette to find actual color. //TODO:
      // Lowest two bits are supposed to be for tile data but we don't implement
      // tiles so hardcoded to index_into_palette
      constexpr uint16_t palette_ram_start = 0x3F00;
      uint8_t color_offset = (quadrant_color_idx << 2) & index_into_palette;
      pixel_t nes_color = read(palette_ram_start + color_offset);

      color_t color;
      try {
        color = kColorTable.at(nes_color);
      } catch (...) {
        BOOST_LOG_TRIVIAL(error) << "Bad color from CPU when rendering frame: "
                                 << util::fmt_hex(nes_color) << "\n";
        // default to black
        color = 0x0;
      }

      // Update values in buffer
      out[i * 3] = (color >> 16) & 0xFF;
      out[i * 3 + 1] = (color >> 8) & 0xFF;
      out[i * 3 + 2] = color & 0xFF;
    }
  }
}

void PPU::set_PPUCTRL(uint8_t val) { PPUCTRL_ = val; }
void PPU::set_PPUMASK(uint8_t val) { PPUMASK_ = val; }
uint8_t PPU::get_PPUSTATUS() { return PPUSTATUS_; }
void PPU::set_OAMADDR(uint8_t val) { OAMADDR_ = val; }
void PPU::set_OAMDATA(uint8_t val) { OAMDATA_ = val; }
uint8_t PPU::get_OAMDATA() { return OAMDATA_; }
void PPU::set_PPUSCROLL(uint8_t val) { PPUSCROLL_ = val; }
void PPU::set_PPUADDR(uint8_t val) { PPUADDR_ = (PPUADDR_ << 8) | val; }
uint8_t PPU::get_PPUDATA() {
  // wiki: After access, the video memory address will increment by an amount
  // determined by bit 2 of $2000
  return PPUDATA_;
}
void PPU::set_PPUDATA(uint8_t val) {
  // wiki: After access, the video memory address will increment by an amount
  // determined by bit 2 of $2000.
  PPUDATA_ = val;
}

uint8_t PPU::read(uint16_t addr) {
  switch (addr) {
    case 0x0000 ... 0x1FFF:  // Pattern table 0
      BOOST_LOG_TRIVIAL(info)
          << "Pattern tables not implemented: " << util::fmt_hex(addr);
      return 0xAA;
    case 0x2000 ... 0x2FFF:  // Nametables
      BOOST_LOG_TRIVIAL(info) << "Read from nametable: " << util::fmt_hex(addr);
    case 0x3000 ... 0x3EFF:  // Nametable mirror
      return nametables_[addr - 0x2000];
      BOOST_LOG_TRIVIAL(info)
          << "Read from nametable mirror: " << util::fmt_hex(addr);
      return nametables_[addr - 0x3000];
    case 0x3F00 ... 0x3F1F:  // Palette RAM
      BOOST_LOG_TRIVIAL(info)
          << "Read from palette ram: " << util::fmt_hex(addr);
      return palette_ram_[addr - 0x3F00];
    case 0x3F20 ... 0x3FFF:  // Palette RAM mirror
      BOOST_LOG_TRIVIAL(info)
          << "Read from palette ram mirror: " << util::fmt_hex(addr);
      return palette_ram_[addr - 0x3F20];
    default:
      BOOST_LOG_TRIVIAL(info)
          << "Invalid PPU memory address read: " << util::fmt_hex(addr);
      return 0xAA;
  }
}

bool PPU::write(uint16_t addr, uint8_t data) {
  switch (addr) {
    case 0x0000 ... 0x1FFF:  // Pattern table 0
      BOOST_LOG_TRIVIAL(info)
          << "Pattern tables not implemented: " << util::fmt_hex(addr);
      return false;
    case 0x2000 ... 0x2FFF:  // Nametables
      BOOST_LOG_TRIVIAL(info) << "Write to nametable: " << util::fmt_hex(addr);
      nametables_[addr - 0x2000] = data;
      return true;
    case 0x3000 ... 0x3EFF:  // Nametable mirror
      BOOST_LOG_TRIVIAL(info)
          << "Write to nametable mirror: " << util::fmt_hex(addr);
      nametables_[addr - 0x3000] = data;
      return true;
    case 0x3F00 ... 0x3F1F:  // Palette RAM
      BOOST_LOG_TRIVIAL(info)
          << "Write to palette ram: " << util::fmt_hex(addr);
      palette_ram_[addr - 0x3F00] = data;
      return true;
    case 0x3F20 ... 0x3FFF:  // Palette RAM mirror
      BOOST_LOG_TRIVIAL(info)
          << "Write to palette ram mirror: " << util::fmt_hex(addr);
      palette_ram_[addr - 0x3F20] = data;
      return true;
    default:
      BOOST_LOG_TRIVIAL(info)
          << "Invalid PPU memory address write: " << util::fmt_hex(addr);
      return false;
  }
}

}  // namespace ppu