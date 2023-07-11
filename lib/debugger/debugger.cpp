#include "debugger.hpp"

#include <boost/log/trivial.hpp>
#include <format>
#include <iostream>
#include <sstream>

namespace debugger {

// Must construct from a ptr to an existing CPU
Debugger::Debugger(cpu::CPU* cpu) : cpu_(cpu) {}

void Debugger::debug() {
  // Loop while debugging. Listen for a command and execute it.
  std::cout << "Debug mode active; program execution is stopped.\n Use the "
               "'help' command to view all usable commands.\n"
            << std::endl;

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
  const char* cmd = cmd_name.c_str();

  if (strcmp(cmd, "help") == 0) {
    cmd_help();
  } else if (strcmp(cmd, "step") == 0) {
  } else if (strcmp(cmd, "break") == 0) {
  } else if (strcmp(cmd, "delete") == 0) {
  } else if (strcmp(cmd, "continue") == 0) {
  } else if (strcmp(cmd, "list") == 0) {
  } else if (strcmp(cmd, "clear") == 0) {
  } else if (strcmp(cmd, "registers") == 0) {
  } else if (strcmp(cmd, "set") == 0) {
  } else if (strcmp(cmd, "read") == 0) {
  } else if (strcmp(cmd, "write") == 0) {
  } else if (strcmp(cmd, "cpudump") == 0) {
  } else if (strcmp(cmd, "memdump") == 0) {
  } else if (strcmp(cmd, "") == 0) {
    // empty check for newline to allow spamming w/o error messages
  } else {
    // unrecognized command
    BOOST_LOG_TRIVIAL(info)
        << std::format("Command '{}' not recognized.\n", cmd_name);
    return false;
  }

  return true;
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