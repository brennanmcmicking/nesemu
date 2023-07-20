#include "ppu.hpp"

// #include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <boost/log/trivial.hpp>

#include "util.hpp"

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

  // Create OpenGL context and setup opengl stuff
  glfwMakeContextCurrent(window_);
  glfwSwapInterval(1);

  glDisable(GL_DEPTH_TEST);

  // Create callback to close the window when expected
  // TODO:
  // glfwSetWindowCloseCallback(window_, []())

  // TODO: initialize key callbacks here
}

// TODO: where we gonna get cpu data. bus??
void PPU::render_frame() {
  for (color_t* iter = frame_buffer_.begin(); iter != frame_buffer_.end();
       ++iter) {
    pixel_t nes_col = 0x19;  // TODO: get color from cpu
    color_t col;
    try {
      col = kColorTable.at(nes_col);
    } catch (...) {
      BOOST_LOG_TRIVIAL(error) << "Bad color from CPU when rendering frame: "
                               << util::fmt_hex(nes_col) << "\n";
      // default to black
      col = 0x0;
    }

    *iter = col;
  }
}

void PPU::draw() {
  // Actual size of window
  int width, height;
  // TODO: glfwSetFramebufferSizeCallback to be notified when window size
  // changes

  // Setup viewport (full window)
  glfwGetFramebufferSize(window_, &width, &height);
  glViewport(0, 0, width, height);
  // Clear window contents
  glClear(GL_COLOR_BUFFER_BIT);

  glDrawPixels(kScreenWidth, kScreenHeight, GL_RGB, GL_UNSIGNED_SHORT_4_4_4_4,
               frame_buffer_.data());

  // glfw windows are double buffered
  glfwSwapBuffers(window_);
  // process all waiting events
  glfwPollEvents();  // TODO: window refresh callback as in fn hint
}

void PPU::get_frame_buffer(frame_t& out) { out = frame_buffer_; }

void PPU::debug_make_solid_color(pixel_t color, frame_t& out) {
  for (color_t* iter = out.begin(); iter != out.end(); ++iter) {
    *iter = color;
  }
}

}  // namespace ppu