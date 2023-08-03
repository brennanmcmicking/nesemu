#include "debugger.hpp"

#include <algorithm>
#include <boost/log/trivial.hpp>
#include <cstdint>
#include <format>
#include <ios>
#include <iostream>
#include <sstream>
#include <thread>

#include "util.hpp"

namespace debugger {

// Must construct from a ptr to an existing CPU
Debugger::Debugger(cpu::CPU* cpu)
    : cpu_(cpu),
      breakpoints_{},
      cycles_todo_in_frame_(cpu::CPU::kCyclesPerFrame),
      frame_start_(std::chrono::steady_clock::now()),
      frame_deadline_(frame_start_ + cpu::CPU::kTimePerFrameMillis) {}

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

bool Debugger::smart_execute_cycle() {
  bool exited_vblank = false;
  if (cycles_todo_in_frame_ == 0) {
    std::this_thread::sleep_until(frame_deadline_);
    frame_start_ = std::chrono::steady_clock::now();
    frame_deadline_ = frame_start_ + cpu::CPU::kTimePerFrameMillis;
    cycles_todo_in_frame_ = cpu::CPU::kCyclesPerFrame;

    if (cpu_->ppu_.has_value()) {
      // Render a frame using the current cpu data
      cpu::CPU::PPU& ppu = cpu_->ppu_->get();
      ppu.render_to_window();
    }

    // NMI
    cpu_->push_stack16(cpu_->PC());
    cpu_->push_stack(cpu_->P());
    cpu_->PC_ = cpu_->read16(0xFFFA);
    exited_vblank = true;
  }

  cpu_->cycle();
  cycles_todo_in_frame_ -= 1;
  return exited_vblank;
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
  int32_t tmp_num;  // for parsing the value of user-given tokens

  std::string cmd_name;
  input_stream >> cmd_name;
  const char* cmd = cmd_name.c_str();

  if (strcmp(cmd, "help") == 0) {
    cmd_help();

  } else if (strcmp(cmd, "step") == 0) {
    cmd_step();
  } else if (strcmp(cmd, "continue") == 0) {
    cmd_continue();
  } else if (strcmp(cmd, "break") == 0) {
    address_t addr;
    tmp_num = util::extract_num(input_stream);
    if (tmp_num == -1) {
      std::cout << "Please specify an address to add a new breakpoint\n";
      return true;
    }
    addr = tmp_num;

    cmd_break(addr);

  } else if (strcmp(cmd, "delete") == 0) {
    address_t addr;
    tmp_num = util::extract_num(input_stream);
    if (tmp_num == -1) {
      std::cout << "Please specify the address where a breakpoint should be "
                   "deleted\n";
      return true;
    }
    addr = tmp_num;

    cmd_delete(addr);

  } else if (strcmp(cmd, "list") == 0) {
    cmd_list();

  } else if (strcmp(cmd, "clear") == 0) {
    cmd_clear();

  } else if (strcmp(cmd, "read") == 0) {
    address_t addr;
    uint16_t bytes;
    tmp_num = util::extract_num(input_stream);
    if (tmp_num == -1) {
      std::cout << "Bad address value\n";
      return true;
    }
    addr = tmp_num;

    tmp_num = util::extract_num(input_stream);
    if (tmp_num > -1) {
      bytes = tmp_num;
    } else {
      // it's ok if this wasn't specified, it defaults to 1.
      // TODO: doesn't check if parse error or not provided
      bytes = 1;
    }

    cmd_read(addr, bytes);

  } else if (strcmp(cmd, "write") == 0) {
    address_t addr;
    uint8_t data;

    tmp_num = util::extract_num(input_stream);
    if (tmp_num == -1) {
      std::cout << "Address parsing failed\n";
      return true;
    }
    addr = tmp_num;

    tmp_num = util::extract_num(input_stream);
    if (tmp_num == -1) {
      std::cout << "Data parsing failed\n";
      return true;
    }
    data = tmp_num;
    if (data != tmp_num) {
      std::cout << "Note: data truncated to " << util::fmt_hex(data) << "\n";
    }

    cmd_write(addr, data);

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

    tmp_num = util::extract_num(input_stream);
    if (tmp_num == -1) {
      std::cout << "No value specified\n";
      return true;
    }
    value = tmp_num;

    cmd_set(reg_name, value);

  } else if (strcmp(cmd, "") == 0) {
    // empty check for newline to allow spamming w/o error messages
  } else if (strcmp(cmd, "exit") == 0) {
    exit(0);
  } else {
    // unrecognized command
    std::cout << "Command '" << cmd_name << "' not recognized\n";
    return true;
  }

  return true;
}

void Debugger::cmd_help() { std::cout << help_msg_; }
void Debugger::cmd_step() {
  uint8_t opcode = cpu_->read(cpu_->PC());
  std::cout << "opcode: " << util::fmt_hex(opcode) << "\n";
  bool no_interrupt = true;

  std::size_t i;
  for (i = 0; (i < cpu_->cycle_count(opcode)) && no_interrupt; i++) {
    no_interrupt = !smart_execute_cycle();
  }

  std::cout << "true cycles executed: " << i << "\n";
  if (!no_interrupt) {
    std::cout << "interrupt occurred\n";
  }
  cmd_registers();

  // TODO: display data in current PC so this is more useful?
  // address_t instr_addr = cpu_->
  // cpu_->advance_instruction();

  // std::cout << "Step: executed instruction at address"
  // << util::fmt_hex(cpu_->addr_fetch(cpu::kAbsolute)) << "\n";
}
void Debugger::cmd_continue() {
  // while (!breakpoints_.contains(cpu_->PC())) {
  //   cpu_->advance_frame();
  // }

  bool frame_crossed = false;

  while (!breakpoints_.contains(cpu_->PC())) {
    if (smart_execute_cycle()) {
      frame_crossed = true;
    }
  }
  if (frame_crossed) {
    std::cout << "A frame ended and NMI was triggered during execution. Linear "
                 "execution may have been interrupted.\n";
  }
  std::cout << "Breakpoint reached: " << util::fmt_hex(cpu_->PC()) << "\n";
  cmd_registers();
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
void Debugger::cmd_read(address_t addr, uint16_t bytes) {
  for (uint16_t offset = 0; offset < bytes; ++offset) {
    if (offset > 0 && offset % 8 == 0) std::cout << "\n";
    std::cout << util::fmt_hex(cpu_->read(addr + offset)) << "\t";
  }
  std::cout << "\n";
}
void Debugger::cmd_write(address_t addr, uint8_t data) {
  std::cout << "Wrote " << util::fmt_hex(data) << " to " << util::fmt_hex(addr)
            << "\n";
  cpu_->write(addr, data);
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