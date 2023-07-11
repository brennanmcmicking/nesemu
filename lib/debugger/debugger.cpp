#include "debugger.hpp"

#include <boost/log/trivial.hpp>
#include <format>
#include <iostream>
#include <sstream>
namespace debugger {
void Debugger::debug() {
  // Loop while debugging. Listen for a command and execute it.
  while (1) {
    read_command();
  }
}

bool Debugger::read_command() {
  std::string input;
  std::getline(std::cin, input);

  if (!std::cin) {
    // input failed
    return false;
  }

  std::stringstream input_stream(input);

  std::string cmd_name;
  input_stream >> cmd_name;

  if (cmd_name == "help") {
    cmd_help();
  } else if (cmd_name == "step") {
  } else if (cmd_name == "break") {
  } else if (cmd_name == "delete") {
  } else if (cmd_name == "continue") {
  } else if (cmd_name == "list") {
  } else if (cmd_name == "clear") {
  } else if (cmd_name == "registers") {
  } else if (cmd_name == "set") {
  } else if (cmd_name == "read") {
  } else if (cmd_name == "write") {
  } else if (cmd_name == "cpudump") {
  } else if (cmd_name == "memdump") {
  } else {
    // unrecognized command
    BOOST_LOG_TRIVIAL(info) << std::format("Command '{}' not recognized.\n");
    return false;
  }
}

void Debugger::cmd_help() { std::cout << help_msg_; }
void Debugger::cmd_step() {}
void Debugger::cmd_break(address_t addr) {}
void Debugger::cmd_delete(address_t addr) {}
void Debugger::cmd_continue() {}
void Debugger::cmd_list() {}
void Debugger::cmd_clear() {}
void Debugger::cmd_registers() {}
void Debugger::cmd_set(Register reg, uint16_t value) {}
void Debugger::cmd_read(address_t addr, uint bytes) {}
void Debugger::cmd_write(address_t addr, uint8_t data) {}
void Debugger::cmd_cpudump() {}
void Debugger::cmd_memdump(const char* filename) {}
}  // namespace debugger