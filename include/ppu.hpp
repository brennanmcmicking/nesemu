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

  // An RGB color, suitable for rendering into a frame (after being split into
  // R, G, B)
  using color_t = uint32_t;

  // A frame, suitable to be drawn to the window. Every three elements represent
  // a color (e.g. the first pixel in the array has red value at [0], green at
  // [1], and blue at [2]). Note: a frame_t takes 184kB of storage. Be careful
  // with default initializing!
  using frame_t = std::array<uint8_t, kScreenWidth * kScreenHeight * 3>;

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

  PPU(GLFWwindow& window);
  ~PPU();

  PPU(PPU&) = delete;
  PPU(PPU&&) = delete;
  PPU& operator=(PPU&) = delete;
  PPU operator=(PPU&&) = delete;

  // Interface: takes data from cpu bus, formats it in a pixel buffer, then
  // sends it to the output: a window (but ideally can be extensible)

  /**
   * @brief Using the data currently visible from the CPU, renders a single
   * frame into the given window. Use draw() to display the rendered
   * frame, or get_frame_buffer() if you need to consume it in another way.
   */
  void render_to_window();

  /**
   * @brief Like render_to_window except writes the result to a buffer.
   *
   * @param out Overwritten with the resulting buffer
   */
  void render_to_framebuffer(frame_t& out);

  void set_PPUCTRL(uint8_t val);
  void set_PPUMASK(uint8_t val);
  uint8_t get_PPUSTATUS();
  void set_OAMADDR(uint8_t val);
  void set_OAMDATA(uint8_t val);
  uint8_t get_OAMDATA();
  void set_PPUSCROLL(uint8_t val);
  void set_PPUADDR(uint8_t val);
  uint8_t get_PPUDATA();
  void set_PPUDATA(uint8_t val);

  // Helpers to get common attributes from ppu registers
  bool greyscale();
  bool show_background();
  bool in_vblank();

 private:
  // Instead of storing whole ram array, since much of it is unused, only
  // storing used parts: nametables and palette ram indices.

  static constexpr size_t kNametableSize_ = 0x0400;  // size of one nametable
  std::array<uint8_t, kNametableSize_ * 4> nametables_;
  std::array<uint8_t, 0x0020> palette_ram_;

  // Internal buffer to which frames are rendered when rendering to window.
  // Stored in a private variable to avoid having to reallocate the whole
  // frame array every frame. Only allocated on first render_to_window.
  std::unique_ptr<frame_t> internal_frame_buf_;

  GLFWwindow& window_;

  // Registers
  uint8_t PPUCTRL_;    // w
  uint8_t PPUMASK_;    // w
  uint8_t PPUSTATUS_;  // r
  uint8_t OAMADDR_;    // w (unused)
  uint8_t OAMDATA_;    // r/w (unused)
  uint8_t PPUSCROLL_;  // w (unused)
  uint16_t PPUADDR_;   // w (note: written one byte at a time)
  uint8_t PPUDATA_;    // r/w

  // Reading/writing internal ppu ram (only accessible from within PPU or via
  // PPUADDR/PPUDATA registers)
  uint8_t read(uint16_t addr);
  bool write(uint16_t addr, uint8_t data);
};

}  // namespace ppu
#endif  // PPU_HPP