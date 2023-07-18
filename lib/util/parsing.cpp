
#include <cstdlib>
#include <format>

#include "boost/log/trivial.hpp"
#include "util.hpp"

namespace util {

std::string fmt_hex(uint8_t val) { return std::format("${:02X}", val); }
std::string fmt_hex(uint16_t val) { return std::format("${:04X}", val); }
std::string fmt_hex(uint32_t val) { return std::format("${:08X}", val); }

int32_t extract_num(std::iostream& stream) {
  std::string tok;
  // tok << stream;
  stream >> tok;
  return parse_num(tok);
}

int32_t parse_num(const std::string& str) {
  // Expected format for hex is $XXXX, decimal is just XXXXX

  // Need to handle common possible failure and strtol doesn't do
  // that very well so I used this instead https://stackoverflow.com/a/39052987

  uint32_t val = 0;
  uint8_t first_byte = str[0];
  // Check base
  if ((first_byte >= '0' && first_byte <= '9') || (first_byte == '-')) {
    // Decimal
    long long_val = strtol(str.c_str(), 0, 0);
    val = static_cast<uint16_t>(long_val);
    if (val != long_val) {
      // Overflow (but that might be intentional)
      BOOST_LOG_TRIVIAL(info)
          << "Decimal parsing caused overflow: string '" << str
          << "' gave number " << fmt_hex(val) << "\n";
    }

    return val;

  } else if (first_byte == '$') {
    // Hex
    if (str.length() == 1) {
      // It's just a $
      BOOST_LOG_TRIVIAL(info) << "Invalid hex value '" << str << "'\n";
      return -1;
    }
    const char* hex = str.c_str();
    ++hex;
    uint32_t hex_val = 0;
    while (*hex) {
      // get current character then increment
      uint8_t byte = *hex++;
      // transform hex character to the 4bit equivalent number, using the ascii
      // table indexes
      if (byte >= '0' && byte <= '9')
        byte = byte - '0';
      else if (byte >= 'a' && byte <= 'f')
        byte = byte - 'a' + 10;
      else if (byte >= 'A' && byte <= 'F')
        byte = byte - 'A' + 10;
      else {
        // Bad character
        BOOST_LOG_TRIVIAL(info) << "Invalid hex value '" << str << "'\n";
        return -1;
      }
      // shift 4 to make space for new digit, and add the 4 bits of the new
      // digit
      hex_val = (hex_val << 4) | (byte & 0xF);
    }

    // If val doesn't fit in a uint16_t, overflow!
    val = static_cast<uint16_t>(hex_val);
    if (val != hex_val) {
      // Overflow (but that might be intentional)
      BOOST_LOG_TRIVIAL(info) << "Hex parsing caused overflow: string '" << str
                              << "' gave number" << fmt_hex(val) << "\n";
    }

    return val;
  }

  // Implicit else: not hex or decimal, bad string
  return -1;
}
}  // namespace util
