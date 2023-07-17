#include <iostream>

#include "util.hpp"
namespace util {

// IosFormatRAII::IosFormatRAII(std::ios_base::fmtflags flags) {
IosFormatRAII::IosFormatRAII() {
  // Save old flags
  old_flags_ = std::cout.flags();
}

IosFormatRAII::~IosFormatRAII() {
  // Restore flags on destruction
  std::cout.flags(old_flags_);
}
}  // namespace util