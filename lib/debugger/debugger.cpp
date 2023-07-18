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
Debugger::Debugger(cpu::CPU* cpu) : cpu_(cpu), breakpoints_{} {}

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
    address_t addr;
    if (!(input_stream >> addr)) {
      std::cout << "Please specify an address to add a new breakpoint\n";
      return true;
    }
    cmd_break(addr);

  } else if (strcmp(cmd, "delete") == 0) {
    address_t addr;
    if (!(input_stream >> addr)) {
      std::cout << "Please specify the address where a breakpoint should be "
                   "deleted\n";
      return true;
    }
    cmd_delete(addr);

  } else if (strcmp(cmd, "list") == 0) {
    cmd_list();

  } else if (strcmp(cmd, "clear") == 0) {
    cmd_clear();

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
    std::cout << "Command '" << cmd_name << "' not recognized\n";
    return true;
  }

  return true;
}

void Debugger::cmd_help() { std::cout << help_msg_; }
void Debugger::cmd_step() {
  // TODO: display data in current PC so this is more useful?
  // address_t instr_addr = cpu_->
  cpu_->advance_instruction();

  std::cout << "Step: executed instruction at address"
            << util::fmt_hex(cpu_->addr_fetch(cpu::kAbsolute)) << "\n";
}
void Debugger::cmd_continue() {
  // TODO:
}
void Debugger::cmd_break(address_t addr) {
  auto res = breakpoints_.insert(addr);
  if (res.second) {
    std::cout << "Breakpoint added at address " << util::fmt_hex(addr) << "\n";
  } else {
    std::cout << "Breakpoint already created for address "
              << util::fmt_hex(addr) << "\n";
  }
}

void Debugger::cmd_delete(address_t addr) {
  if (breakpoints_.erase(addr)) {
    std::cout << "Breakpoint removed at address " << util::fmt_hex(addr)
              << "\n";
  } else {
    std::cout << "No breakpoint exists at address " << util::fmt_hex(addr)
              << "\n";
  }
}
void Debugger::cmd_list() {
  std::cout << "Breakpoints: \n";
  for (const address_t& addr : breakpoints_) {
    std::cout << util::fmt_hex(addr) << "\n";
  }
}
void Debugger::cmd_clear() {
  breakpoints_.clear();
  std::cout << "Breakpoints cleared\n";
}
void Debugger::cmd_read(address_t addr, uint bytes) {
  // TODO:
}
void Debugger::cmd_write(address_t addr, uint8_t data) {
  // TODO:
}
void Debugger::cmd_registers() {
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