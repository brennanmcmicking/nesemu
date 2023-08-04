#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include <cstdint>
#include <map>
#include <set>

#include "cpu.hpp"

namespace debugger {

class Debugger {
 public:
  using address_t = uint16_t;

  enum Register {
    kRegPC,
    kRegSP,
    kRegA,
    kRegX,
    kRegY,
    kRegP,
  };

  static inline const std::map<std::string, Register> RegisterFromString{
      {"PC", kRegPC}, {"SP", kRegSP}, {"A", kRegA},
      {"X", kRegX},   {"Y", kRegY},   {"P", kRegP}};

  Debugger() = delete;
  explicit Debugger(cpu::CPU* cpu);
  ~Debugger() = default;

  // prevent moves and copies
  Debugger(Debugger&) = delete;
  Debugger(Debugger&&) = delete;
  Debugger& operator=(Debugger&) = delete;
  Debugger operator=(Debugger&&) = delete;

  /**
   * @brief Pause program execution, listen for user input, and begin
   * interpreting their commands.
   */
  void debug();

 private:
  static constexpr char help_msg_[] =
      "Use the following commands to interact with the debugger. Short \n"
      "versions of command names are shown in parenthesis ()\n\n"
      "help (h) \n"
      "  Prints this message \n"
      "step (s) [num steps]\n"
      "  Advance the CPU by one instruction. Optionally specify a number of \n"
      "  instructions to step\n"
      "continue (c)\n"
      "  Continue execution until the next breakpoint \n"
      "break <address> \n"
      "  Pauses program execution when program counter contains <address> \n"
      "delete <address> \n"
      "  Removes the previously specified breakpoint at address \n"
      "list (l)\n"
      "  Prints the addresses of all existing breakpoints \n"
      "clear \n"
      "  Deletes all breakpoints \n"
      "read (r) <address> [bytes] \n"
      "  Prints the value in memory at the specified address in hexadecimal. \n"
      "  Optionally specify a number of bytes to read (default 1) \n"
      "write (w) <address> <data> \n"
      "  Overwrites the value in memory at the given address with specified \n"
      "  data. Note: only writes a single byte \n"
      "registers (reg)\n"
      "  Prints the names and values of all registers in hexadecimal format. \n"
      "  The register names are: \n"
      "  - PC (program counter, 16-bit) \n"
      "  - SP (stack pointer, 8-bit) \n"
      "  - A (accumulator, 8-bit) \n"
      "  - X (index register X, 8-bit) \n"
      "  - Y (index register Y, 8-bit) \n"
      "  - P (processor status flags, 8-bit) \n"
      "set <register> <value> \n"
      "  Set the specified register's value \n"
      "exit\n"
      "  exit the program\n"
      "\n";

  cpu::CPU* cpu_;

  // Set of breakpoints. Not using unordered set for ease of iterating
  std::set<address_t> breakpoints_;

  std::size_t cycles_todo_in_frame_;

  cpu::CPU::time_point frame_start_;
  cpu::CPU::time_point frame_deadline_;

  /**
   * @brief Executes 1 cycle, correctly pausing on frame boundaries/vBlank
   *
   */
  bool smart_execute_cycle();

  /**
   * @brief Read line from stdin and parse it into a command.
   *
   * If the user gives an invalid command, an error message is printed to
   * stdout
   *
   * @return true iff the debug loop should continue (i.e. didn't read EOF or
   * some other fatal error)
   */
  void read_command();

  /**
   * @brief Prints a help message describing the commands that can be used.
   */
  void cmd_help();

  /****** Breakpoint commands *******/

  /**
   * @brief Perform one or more steps, executing the next instruction as many
   * times as specified (default 1)
   */
  void cmd_step(uint num_to_step = 1);

  /**
   * @brief Continue execution of the program as normal (until it hits the next
   * breakpoint).
   */
  void cmd_continue();

  /**
   * @brief Create a new breakpoint at the specified address in program memory.
   *
   * Has no effect if there was already a breakpoint at the specified address.
   *
   * @param addr
   */
  void cmd_break(address_t addr);

  /**
   * @brief Delete the breakpoint at the specified memory address.
   *
   * Has no effect if there was no breakpoint at the specified address.
   *
   * @param addr
   */
  void cmd_delete(address_t addr);

  /**
   * @brief Print the addresses of all existing breakpoints.
   */
  void cmd_list();

  /**
   * @brief Delete all breakpoints.
   */
  void cmd_clear();

  /****** Reading/writing memory and registers ********/

  /**
   * @brief Print the value in memory at the specified address in hexadecimal.
   * Optionally specify a number of bytes to read (default 1).
   *
   * @param addr
   * @param bytes
   */
  void cmd_read(address_t addr, uint16_t bytes = 1);
  /**
   * @brief Overwrite the memory at the specified address with the given byte
   * of data.
   *
   * @param addr
   * @param data
   */
  void cmd_write(address_t addr, uint8_t data);

  /**
   * @brief Print the names and values of all registers.
   */
  void cmd_registers();

  /**
   * @brief Set the specified register's value.
   *
   * Note: registers SP, A, X, Y, and P are 8 bits in size, and the PC is 16
   * bits.
   */
  void cmd_set(std::string reg_name, uint16_t value);
};

}  // namespace debugger

#endif
