#include <cstdint>
#include <ios>

namespace util {
void init_log_level();

/**
 * @brief Format a given number to be pretty-printed in hex in the format
 * $XX or $XXXX depending on if the given number is a uint8_t or uint16_t.
 */
std::string fmt_hex(uint8_t);
std::string fmt_hex(uint16_t);

/**
 * @brief RAII class to save io flags. Alternative to boost's ios_flags_saver
 * since our version is too old to support it. Use this so "std::cout <<
 * std::hex" doesn't cause numbers to be printed in hex forever.
 */
class IosFormatRAII {
 public:
  IosFormatRAII();
  ~IosFormatRAII();

  // Not copyable or movable
  IosFormatRAII(const IosFormatRAII&) = delete;
  IosFormatRAII& operator=(const IosFormatRAII&) = delete;
  IosFormatRAII(IosFormatRAII&&) = delete;
  IosFormatRAII& operator=(IosFormatRAII&&) = delete;

 private:
  std::ios_base::fmtflags old_flags_;
};
}  // namespace util