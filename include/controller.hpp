#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP
#include <GLFW/glfw3.h>

#include <cstdint>

namespace controller {
class Controller {
 public:
  Controller(GLFWwindow& window);
  ~Controller();
  void write_strobe(uint8_t value);
  uint8_t read_joy1();

 private:
  GLFWwindow& window_;
  uint8_t joy1_register_;
  bool strobe_;
};
}  // namespace controller
#endif  // CONTROLLER_HPP