#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <cstdlib>
#include <format>

#include "util.hpp"

namespace log = boost::log;

namespace util {
void init_log_level() {
  char* log_level = std::getenv("LOG_LEVEL");
  if (log_level == nullptr) {
    log_level = "INFO";
  }

  log::trivial::severity_level level = log::trivial::info;

  switch (log_level[0]) {
    case 't':
    case 'T':
      level = log::trivial::trace;
      break;
    case 'd':
    case 'D':
      level = log::trivial::debug;
      break;
    case 'i':
    case 'I':
      level = log::trivial::info;
      break;
    case 'w':
    case 'W':
      level = log::trivial::warning;
      break;
    case 'e':
    case 'E':
      level = log::trivial::error;
      break;
    case 'f':
    case 'F':
      level = log::trivial::fatal;
      break;
  }
  log::core::get()->set_filter(log::trivial::severity >= level);
  return;
}

std::string fmt_hex(uint8_t val) { return std::format("${:02X}", val); }
std::string fmt_hex(uint16_t val) { return std::format("${:04X}", val); }
}  // namespace util