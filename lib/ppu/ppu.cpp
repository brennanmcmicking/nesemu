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

  // bg palette is four palettes with 3 colors each
  // TODO: test data. should be loaded from addresses above (set by cpu/game)
  pixel_t bg_color = 0x10,                     // light grey
      bg_palette[3 * 4] = {0x01, 0x02, 0x11,   // blues
                           0x09, 0x19, 0x1A,   // greens
                           0x06, 0x16, 0x26,   // reds
                           0x32, 0x34, 0x38};  // light colors

  for (int y = 0; y < kScreenHeight; ++y) {
    for (int x = 0; x < kScreenWidth; ++x) {
      int i = y * kScreenWidth + x;

      // Calculate part of attribute table that corresponds to this pixel

      // Width is 256 pixels, attribute table is 8x8 chunks. Each byte in the
      // attribute table contains four colors for each of its four 16x16 px
      // quadrants.
      constexpr int byte_px_width = kScreenWidth / 8;  // TODO: rename

      // Coordinates of the attribute table entry this pixel belongs to. Between
      // 0 and 7
      int attr_y = y / byte_px_width;
      int attr_x = x / byte_px_width;
      assert(attr_y < 8 && attr_x < 8);

      // Coordinates of quadrant in byte; either 0 or 1
      uint8_t quadrant_y = (y / 16) % 2;
      uint8_t quadrant_x = (x / 16) % 2;
      uint8_t quadrant_idx = quadrant_y * 2 + quadrant_x;

      // Attribute table is 64 bytes at $23C0, a 8x8 byte array
      // std::array<uint8_t, 64> attr_table; //TODO: bus time
      constexpr uint16_t attr_table_start = 0x23C0;

      // Attribute is for color selection from current palette
      // TODO: get from cpu (attr_table_start + i * 8), literal for testing

      uint8_t attribute = 0x12;  // TODO: the actual color data

      // Mask color out of attribute depending on quadrant
      pixel_t quadrant_color_idx = (attribute >> quadrant_idx) & 0b11;

      // Use color index in current frame palette to find actual color
      pixel_t nes_color = bg_palette[quadrant_color_idx];

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

}  // namespace ppu