#include <cstdint>
#include <ios>
#include <istream>

namespace util {
void init_log_level();

/**
 * @brief Format a given number to be pretty-printed in hex in the format
 * $XX or $XXXX depending on if the given number is a uint8_t or uint16_t.
 */
std::string fmt_hex(uint8_t);
std::string fmt_hex(uint16_t);
std::string fmt_hex(uint32_t);

/**
 * @brief Parse a number from given string. Works with decimal numbers and hex
 * numbers in the format "$X" where X is between one and four hex digits.
 *
 * @param str The string to parse
 * @return int32_t If the string was formatted correctly, returns a number that
 * can be safely cast to uint16_t. If not, returns -1.
 */
int32_t parse_num(const std::string& str);

/**
 * @brief Wrapper around parse_num to extract a number in either decimal or hex
 * from the given stream
 *
 * @param stream
 * @return int32_t A non-negative number on success, or //TODO: failure
 */
int32_t extract_num(std::iostream& stream);

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