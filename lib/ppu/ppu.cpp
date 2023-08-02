#include "ppu.hpp"

// #include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <boost/log/trivial.hpp>

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
  // TODO: get color data from cpu

  for (int i = 0; i < kScreenWidth * kScreenHeight; i++) {
    pixel_t nes_col = 0x16;  // TODO: get this color from cpu
    color_t col;

    try {
      col = kColorTable.at(nes_col);
    } catch (...) {
      BOOST_LOG_TRIVIAL(error) << "Bad color from CPU when rendering frame: "
                               << util::fmt_hex(nes_col) << "\n";
      // default to black
      col = 0x0;
    }

    // Update values in buffer
    out[i * 3] = (col >> 16) & 0xFF;
    out[i * 3 + 1] = (col >> 8) & 0xFF;
    out[i * 3 + 2] = col & 0xFF;
  }
}

}  // namespace ppu