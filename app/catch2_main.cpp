#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include "util.hpp"

int main(int argc, char* argv[]) {
  util::init_log_level();
  return Catch::Session().run(argc, argv);
}