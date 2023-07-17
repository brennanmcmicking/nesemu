#include "debugger.hpp"

#include <algorithm>
#include <boost/log/trivial.hpp>
#include <cstdint>
#include <format>
#include <ios>
#include <iostream>
#include <sstream>

#include "util.hpp"

namespace debugger {

// Must construct from a ptr to an existing CPU
Debugger::Debugger(cpu::CPU* cpu) : cpu_(cpu) {}

void Debugger::debug() {
  // Loop while debugging. Listen for a command and execute it.
  std::cout << "Debug mode active; program execution is stopped.\n Use the "
               "'help' command to view all usable commands.\n"
            << std::endl;

  while (1) {
    if (!read_command()) break;
  }
}

// temporary fn to make results more clear during implementation
void _unimplemented() {
  BOOST_LOG_TRIVIAL(info) << "Command not yet implemented\n";
}

bool Debugger::read_command() {
  std::string input;
  std::cout << "> ";

  if (!std::getline(std::cin, input)) {
    // Just die on input error. In terms of usability, this prevents stuff like
    // piping a file with a series of commands for example. Might change later.
    if (std::cin.eof())
      std::cout << "EOF\n";
    else
      std::cout << "Error reading input. Exiting\n";
    exit(1);
  }

  std::stringstream input_stream(input);

  std::string cmd_name;
  input_stream >> cmd_name;
  const char* cmd = cmd_name.c_str();

  if (strcmp(cmd, "help") == 0) {
    cmd_help();
  } else if (strcmp(cmd, "step") == 0) {
    cmd_step();
  } else if (strcmp(cmd, "continue") == 0) {
    _unimplemented();  // TODO:
  } else if (strcmp(cmd, "break") == 0) {
    _unimplemented();  // TODO:
  } else if (strcmp(cmd, "delete") == 0) {
    _unimplemented();  // TODO:
  } else if (strcmp(cmd, "list") == 0) {
    _unimplemented();  // TODO:
  } else if (strcmp(cmd, "clear") == 0) {
    _unimplemented();  // TODO:
  } else if (strcmp(cmd, "read") == 0) {
    _unimplemented();  // TODO:
  } else if (strcmp(cmd, "write") == 0) {
    _unimplemented();  // TODO:
  } else if (strcmp(cmd, "registers") == 0) {
    cmd_registers();
  } else if (strcmp(cmd, "set") == 0) {
    std::string reg_name;
    uint16_t value;

    // Parse register name
    if (!(input_stream >> reg_name)) {
      std::cout << "No register specified\n";
      return true;
    }

    // Parse value
    if (!(input_stream >> value)) {
      std::cout << "No value specified\n";
      return true;
    }

    cmd_set(reg_name, value);
  } else if (strcmp(cmd, "") == 0) {
    // empty check for newline to allow spamming w/o error messages
  } else {
    // unrecognized command
    std::cout << std::format("Command '{}' not recognized.\n", cmd_name);
    return true;
  }

  return true;
}

void Debugger::cmd_help() { std::cout << help_msg_; }
void Debugger::cmd_step() {
  // TODO: is this logic good for stepping?

  uint8_t instruction = cpu_->read(cpu_->PC());
  auto cycles = cpu_->cycle_count(instruction);

  if (cpu_->cycles_todo_ != 0) {
    // TODO: is this a problem?
    BOOST_LOG_TRIVIAL(info) << "Step: previous command not finished cycling\n";
    cpu_->cycles_todo_ = 0;
  }

  // Advancing the current instruction's number of cycles with no cycles_todo_
  // is equivalent to executing the command and leaving no remaining cycles
  cpu_->advance(cycles);

  // cpu_->advance()
}
void Debugger::cmd_continue() {
  // TODO:
}
void Debugger::cmd_break(address_t addr) {
  // TODO:
}
void Debugger::cmd_delete(address_t addr) {
  // TODO:
}
void Debugger::cmd_list() {
  // TODO:
}
void Debugger::cmd_clear() {
  // TODO:
}
void Debugger::cmd_read(address_t addr, uint bytes) {
  // TODO:
}
void Debugger::cmd_write(address_t addr, uint8_t data) {
  // TODO:
}
void Debugger::cmd_registers() {
  // "  - PC (program counter, 16-bit) \n"
  // "  - SP (stack pointer, 8-bit) \n"
  // "  - A (accumulator, 8-bit) \n"
  // "  - X (index register X, 8-bit) \n"
  // "  - Y (index register Y, 8-bit) \n"
  // "  - P (processor status flags, 8-bit) \n"
  // TODO:

  std::cout << "PC (16-bit): " << util::fmt_hex(cpu_->PC()) << "\n"  //
            << "SP (8-bit):  " << util::fmt_hex(cpu_->SP()) << "\n"  //
            << "A  (8-bit):  " << util::fmt_hex(cpu_->A()) << "\n"   //
            << "X  (8-bit):  " << util::fmt_hex(cpu_->X()) << "\n"   //
            << "Y  (8-bit):  " << util::fmt_hex(cpu_->Y()) << "\n"   //
            << "P  (8-bit):  " << util::fmt_hex(cpu_->P()) << "\n"   //
      // TODO: explain flags
      ;
}
void Debugger::cmd_set(std::string reg_name, uint16_t value) {
  // TODO: Normalize to allow writing mixed case register names (e.g. 'pc')
  //  std::string normalized_reg_name;
  //  std::transform(reg_name.cbegin(), reg_name.cend(),
  //                 normalized_reg_name.begin(),
  //                 [](char c) { return std::toupper(c); });

  Register reg;
  try {
    reg = RegisterFromString.at(reg_name);
  } catch (const std::out_of_range& e) {
    std::cout << "Unknown register '" << reg_name << "'\n";
    return;
  }
  util::IosFormatRAII flag_saver;

  std::cout << std::hex << "Register " << reg_name << " set to "
            << util::fmt_hex(value) << "\n";

  switch (reg) {
    case kRegPC:
      cpu_->PC_ = value;
      break;
    case kRegSP:
      cpu_->SP_ = value;
      break;
    case kRegA:
      cpu_->A_ = value;
      break;
    case kRegX:
      cpu_->X_ = value;
      break;
    case kRegY:
      cpu_->Y_ = value;
      break;
    case kRegP:
      cpu_->P_ = value;
      break;
  }
}
}  // namespace debugger