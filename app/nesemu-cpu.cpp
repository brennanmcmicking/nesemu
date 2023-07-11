#include <boost/log/trivial.hpp>

#include "util.hpp"

int main(int argc, char* argv[]) {
  util::init_log_level();
  BOOST_LOG_TRIVIAL(info)
      << "hello world\n"
      << "this program will be a tester for the CPU emulator\n"
      << "internally, this could be used for fuzz testing or just quick "
         "testing\n";
}
