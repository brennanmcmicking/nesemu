#include <GLFW/glfw3.h>

#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>

#include "cartridge.hpp"
#include "cpu.hpp"
#include "debugger.hpp"
#include "ppu.hpp"
#include "util.hpp"

constexpr char help_msg[] =
    "Usage: nesemu <FILE> [FLAGS]... \n"
    "Supported flags: \n"
    "\t-D, --debug \texecution breaks on the first instruction \n"
    "\t-h \t\tprints this message \n"
    "Emulate a Nintendo Entertainment System that has loaded a cartridge from "
    "FILE, \n"
    "where the contents of FILE conform to the iNES file format. \n"
    "Controller input is emulated using the following keyboard inputs: \n"
    "D-Pad: arrow keys \n"
    "'B' button: Z \n"
    "'A' button: X \n"
    "'Select' button: A \n"
    "'Start' button: S \n";

// For argument parsing
namespace po = boost::program_options;

// this is just here to make it compile
class DummyMapper : public cartridge::Mapper {
 public:
  DummyMapper(){};
  uint8_t prg_read(uint16_t addr) override { return 0; };
  void prg_write(uint16_t addr, uint8_t data) override{};
};

/**
 * @brief Create the window to display PPU output.
 *
 * @returns A handle to the newly created window
 */
GLFWwindow* init_window() {
  if (!glfwInit()) {
    BOOST_LOG_TRIVIAL(fatal) << "Could not initialize window\n";
    glfwTerminate();
    exit(1);
  }

  GLFWwindow* window = glfwCreateWindow(
      ppu::PPU::kScreenWidth, ppu::PPU::kScreenHeight, "nesemu", NULL, NULL);

  if (!window) {
    // Window creation failed
    BOOST_LOG_TRIVIAL(fatal) << "GLFW window creation failed\n";
    exit(1);
  }

  // Create OpenGL context and setup opengl stuff
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  glDisable(GL_DEPTH_TEST);

  return window;

  // Create callback to close the window when expected
  // TODO:
  // glfwSetWindowCloseCallback(window_, []())

  // TODO: initialize key callbacks here
}

int main(int argc, char* argv[]) {
  // Declare the supported options. Sourced from
  // https://www.boost.org/doc/libs/1_82_0/doc/html/program_options/tutorial.html#id-1.3.30.4.3
  po::options_description desc("Allowed options");
  desc.add_options()("help,h", "produce help message")(
      "debug,D", "run in debug mode")("input-file", "input file");

  // Add the file to load as a positional argument
  po::positional_options_description p;
  p.add("input-file", -1);

  po::variables_map vm;
  po::store(
      po::command_line_parser(argc, argv).options(desc).positional(p).run(),
      vm);
  po::notify(vm);

  // Check args
  std::string input_filename;
  bool debug_mode = false;

  if (vm.count("help")) {
    std::cout << help_msg;
    return 1;
  }

  if (vm.count("input-file")) {
    input_filename = vm["input-file"].as<std::string>();
  } else {
    // No input file specified, die
    // std::cout << desc;
    std::cout << help_msg;

    return 1;
  }

  if (vm.count("debug")) {
    std::cout << "Running in debug mode.\n";
    debug_mode = true;
  }

  util::init_log_level();

  GLFWwindow* window_handle = init_window();

  ppu::PPU ppu(*window_handle);
  std::ifstream in(input_filename);
  if (!in.is_open()) {
    BOOST_LOG_TRIVIAL(fatal)
        << "input file cannot be found: " << input_filename;
    return 1;
  }
  cartridge::Cartridge cart(in);
  cpu::CPU cpu(cart, std::ref(ppu));
  BOOST_LOG_TRIVIAL(info) << "hello world\n";
  BOOST_LOG_TRIVIAL(info)
      << "this program will be the fully integrated nes emulator!\n";

  if (debug_mode) {
    // Only when the program was started in debug mode can it be debugged
    debugger::Debugger debugger(&cpu);
    // Debugger runs infinite loop here
    debugger.debug();
  } else {
    cpu.begin_cpu_loop();
  }

  // Cleanup
  glfwTerminate();
  return 0;
}