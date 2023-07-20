#include <GLFW/glfw3.h>

#include <array>
#include <cstdint>
#include <memory>
#include <unordered_map>

#ifndef PPU_HPP
#define PPU_HPP
namespace ppu {

class PPU {
 public:
  // Data structures to store PPU output as an intermediate representation
  // before drawing a frame to screen

  static inline const int kNumScanlines = 262;  // pixel height with vblank
  static inline const int kScreenWidth = 256;   // pixels
  static inline const int kScreenHeight = 240;  // pixels

  // The color of a "pixel" on the NES is 6 bits, where the top 3 control
  //   brightness and the bottom 3 control hue. The two most significant bits
  //   are ignored. Note that the emphasis bits of the PPUMASK register ($2001)
  //   modify the color as well.
  // "Pixel" is in quotes because on NES hardware, there is no such thing as a
  //  pixel, only a signal. To represent colors, we're using precalculated
  //  values that others have converted from the composite signal to the RGB
  //  color space.

  // A color, as it is represented in the NES
  using pixel_t = uint8_t;

  // An RGB color, suitable for rendering into a frame
  using color_t = uint16_t;

  // A frame, suitable to be drawn to the window. Note: a frame_t takes 61kB of
  // storage. Be careful with default initializing!
  using frame_t = std::array<color_t, kScreenWidth * kScreenHeight>;

  // Color map converts value represented by NES into RGB. Taken from 2C02
  // palette https://www.nesdev.org/wiki/PPU_palettes.
  // Note:  0x0D is the weird forbidden color. Just handling it as
  // normal black (same as 0x0F for example)
  static inline const std::unordered_map<pixel_t, color_t> kColorTable{
      {0x00, 0x626262}, {0x01, 0x0D226B}, {0x02, 0x241476}, {0x03, 0x3B0A6B},
      {0x04, 0x4C074D}, {0x05, 0x520C24}, {0x06, 0x4C1700}, {0x07, 0x3B2600},
      {0x08, 0x243400}, {0x09, 0x0D3D00}, {0x0A, 0x004000}, {0x0B, 0x003B24},
      {0x0C, 0x00304D}, {0x0D, 0x000000}, {0x0E, 0x000000}, {0x0F, 0x000000},
      {0x10, 0xABABAB}, {0x11, 0x3156B1}, {0x12, 0x5043C5}, {0x13, 0x7034BB},
      {0x14, 0x892F95}, {0x15, 0x94345F}, {0x16, 0x8E4226}, {0x17, 0x795500},
      {0x18, 0x5B6800}, {0x19, 0x3B7700}, {0x1A, 0x227C15}, {0x1B, 0x17774C},
      {0x1C, 0x1D6985}, {0x1D, 0x000000}, {0x1E, 0x000000}, {0x1F, 0x000000},
      {0x20, 0xFFFFFF}, {0x21, 0x7CAAFF}, {0x22, 0x9B96FF}, {0x23, 0xBD86FF},
      {0x24, 0xD87EF1}, {0x25, 0xE682BA}, {0x26, 0xE38F7F}, {0x27, 0xD0A24E},
      {0x28, 0xB2B734}, {0x29, 0x90C739}, {0x2A, 0x74CE5C}, {0x2B, 0x66CB92},
      {0x2C, 0x69BECE}, {0x2D, 0x4E4E4E}, {0x2E, 0x000000}, {0x2F, 0x000000},
      {0x30, 0xFFFFFF}, {0x31, 0xC9DEFC}, {0x32, 0xD5D6FF}, {0x33, 0xE2CFFF},
      {0x34, 0xEECCFC}, {0x35, 0xF5CCE7}, {0x36, 0xF5D1CF}, {0x37, 0xEED8BB},
      {0x38, 0xE2E1AE}, {0x39, 0xD5E8AE}, {0x3A, 0xC9EBBB}, {0x3B, 0xC2EBCF},
      {0x3C, 0xC2E6E7}, {0x3D, 0xB8B8B8}, {0x3E, 0x000000}, {0x3F, 0x000000}};

  PPU();
  ~PPU();

  PPU(PPU&) = delete;
  PPU(PPU&&) = delete;
  PPU& operator=(PPU&) = delete;
  PPU operator=(PPU&&) = delete;

  // Interface: takes data from cpu bus, formats it in a pixel buffer, then
  // sends it to the output: a window (but ideally can be extensible)

  /**
   * @brief Create the window to display PPU output
   */
  void init_window();

  /**
   * @brief Using the data currently visible from the CPU, renders a single
   * frame into an internal framebuffer. Use draw() to display the rendered
   * frame, or get_frame_buffer() if you need to consume it in another way.
   *
   * @param out The rendered frame
   */
  void render_frame();

  /**
   * @brief Draw a rendered frame to the window. Precondition: init_window() has
   * been called already.
   *
   * @param frame Frame object to be rendered
   */
  void draw();

  /**
   * @brief Get a reference to the internal frame buffer object. For consuming a
   * frame in some other way than rendering to the window. Note: do not modify.
   * If you need to modify, make a copy instead!
   *
   * @param out Overwritten with the internal framebuffer
   */
  void get_frame_buffer(frame_t& out);

 private:
  // Handle to the window. Nullptr before the window is created. Needs to be a
  // raw pointer because GLFWwindow is an opaque type and it's overly
  // complicated to make it into a smart pointer
  GLFWwindow* window_;

  // Internal buffer to which frames are rendered
  frame_t frame_buffer_;

  void debug_make_solid_color(pixel_t color, frame_t& out);
};

}  // namespace ppu
#endif  // PPU_HPP