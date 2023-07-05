#include <boost/log/trivial.hpp>

int main(int argc, char* argv[]) {
  BOOST_LOG_TRIVIAL(info)
      << "hello world\n"
      << "this program will be a tester for the CPU emulator\n"
      << "internally, this could be used for fuzz testing or just quick "
         "testing\n";
}
