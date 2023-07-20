#include "ppu.hpp"

#include <GLFW/glfw3.h>

#include <boost/log/trivial.hpp>

namespace ppu {
PPU::PPU() : window_(nullptr) {
  // Function called whenever glfw errors
  glfwSetErrorCallback([](int err, const char* desc) {
    BOOST_LOG_TRIVIAL(error) << "GLFW error" << err << ": " << desc << "\n";
  });
  // Setup window
  init_window();
};

PPU::~PPU() {
  // Cleanup glfw: windows and everything
  glfwTerminate();
}

void PPU::init_window() {
  if (window_) {
    // Window was already created
    BOOST_LOG_TRIVIAL(warning) << "Window already initialized\n";
    return;
  }

  if (!glfwInit()) {
    BOOST_LOG_TRIVIAL(fatal) << "Could not initialize window\n";
    glfwTerminate();
    exit(1);
  }

  window_ = glfwCreateWindow(kScreenWidth, kScreenHeight, "nesemu", NULL, NULL);

  if (!window_) {
    // Window creation failed
    BOOST_LOG_TRIVIAL(fatal) << "GLFW window creation failed\n";
  }
}

}  // namespace ppu