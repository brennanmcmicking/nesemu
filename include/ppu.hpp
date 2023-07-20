#include <GLFW/glfw3.h>

#include <array>
#include <cstdint>
#include <memory>

#ifndef PPU_HPP
#define PPU_HPP
namespace ppu {

class PPU {
 public:
  // Data structures to store PPU output as an intermediate representation
  // before drawing a frame to screen

  static const int kNumScanlines = 262;  // pixel height, including vblank
  static const int kScreenWidth = 256;   // pixels
  static const int kScreenHeight = 240;  // pixels
  // A pixel's color on the NES is one byte to describe its hue &
  // value/brightness
  using pixel_t = uint8_t;
  using frame_t = std::array<pixel_t, kScreenWidth * kScreenHeight>;

  PPU();
  ~PPU();

  PPU(PPU&) = delete;
  PPU(PPU&&) = delete;
  PPU& operator=(PPU&) = delete;
  PPU operator=(PPU&&) = delete;

  // Interface: takes data from cpu bus, formats it in a pixel buffer, then
  // sends it to the output: a window (but ideally can be extensible)

  void init_window();

  void render_frame(frame_t& out);

 private:
  // Handle to the window. Nullptr before the window is created
  using window_ptr_t = std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)>;
  window_ptr_t window_;

  // GLFWwindow* window_;
};

}  // namespace ppu
#endif  // PPU_HPP