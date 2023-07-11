#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include <cstdint>

#include "cpu.hpp"

namespace debugger {

/*
help
  Prints this message
break <address>
  Pauses program execution when program counter contains <address>
delete <address>
  Removes the previously specified breakpoint at address
step
  Advance the CPU by one instruction
continue
  Continue execution until the next breakpoint
clear
  Deletes all breakpoints
x/<N> <address>
  Prints N bytes starting at the location <address>. This only supports RAM
  access, everything else is undefined. Bytes are printed in hexadecimal
  format
registers
  Prints the names and values of all registers in hexadecimal format. The
  register names are:
  - PC (program counter, 16-bit)
  - SP (stack pointer, 8-bit)
  - A (accumulator, 8-bit)
  - X (index register X, 8-bit)
  - Y (index register Y, 8-bit)
  - P (processor status flags, 8-bit)

  MICHAEL'S REQUESTS: //TODO: get feedback

set <register> <value>
  Set the specified register's value
read <address> [bytes]            //TODO: duplicate of 'x/<N>' above
  Prints the value in memory at the specified address in hexadecimal. Optionally
  specify a number of bytes to read (default 1).
write <address> <data>

cpudump
  Dumps the processor state: registers and flags. //TODO: to file or stdout?
memdump <file>
  Creates a file with the given name and dumps the contents of CPU memory to
  that file
*/

class Debugger {
  using address_t = uint16_t;

  enum Register {
    kRegPC,
    kRegSP,
    kRegA,
    kRegX,
    kRegY,
    kRegP,
  };

 public:
  /**
   * @brief Pause program execution, listen for user input, and begin
   * interpreting their commands.
   */
  void debug();

 private:
  static constexpr char help_msg_[] = "asdf";

  /**
   * @brief Read line from stdin and parse it into a command.
   *
   * If the user gives an invalid command, an error message is printed to
   * stdout
   *
   * @return true if the command was parsed and ran successfully
   * @return false if there was a parsing error
   */
  bool read_command();

  /**
   * @brief Prints a help message describing the commands that can be used.
   */
  void cmd_help();

  /****** Breakpoint commands *******/

  /**
   * @brief Perform one step, executing the next instruction.
   */
  void cmd_step();

  /**
   * @brief Create a new breakpoint at the specified address in program memory.
   *
   * Has no effect if there was already a breakpoint at the specified address.
   *
   * @param addr //TODO: where is program memory
   */
  void cmd_break(address_t addr);

  /**
   * @brief Delete the breakpoint at the specified memory address.
   *
   * Has no effect if there was no breakpoint at the specified address.
   *
   * @param addr //TODO: where is program memory
   */
  void cmd_delete(address_t addr);

  /**
   * @brief Continue execution of the program as normal (until it hits the next
   * breakpoint).
   */
  void cmd_continue();

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
   * @brief Print the names and values of all registers.
   */
  void cmd_registers();

  /**
   * @brief Set the specified register's value.
   *
   * Note: registers SP, A, X, Y, and P are 8 bits in size, and the PC is 16
   * bits.
   */
  void cmd_set(Register reg, uint16_t value);

  /**
   * @brief Print the value in memory at the specified address in hexadecimal.
   * Optionally specify a number of bytes to read (default 1).
   *
   * @param addr
   * @param bytes
   */
  void cmd_read(address_t addr, uint bytes = 1);

  /**
   * @brief Overwrite the memory at the specified address with the given byte of
   * data.
   *
   * @param addr
   * @param data
   */
  void cmd_write(address_t addr, uint8_t data);

  /**
   * @brief Dumps the processor state (registers and flags) //TODO: stdout or
   * file? Or just have single dump command for all?
   */
  void cmd_cpudump();

  /**
   * @brief Dumps the entire contents of memory to file.
   *
   * @param filename Name of file to be created and written to. If this file
   * exists already, it is not overwritten.
   */
  void cmd_memdump(const char* filename);
};

}  // namespace debugger

#endif
