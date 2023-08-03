#include "controller.hpp"

#include <GLFW/glfw3.h>

#include <boost/log/trivial.hpp>
#include <format>

struct Input {
  bool A;
  bool B;
  bool Select;
  bool Start;
  bool Up;
  bool Down;
  bool Left;
  bool Right;
};

namespace controller {
Controller::Controller(GLFWwindow& window) : window_(window) {}
Controller::~Controller() = default;

void Controller::write_strobe(uint8_t value) {
  bool new_strobe = value & 0x01;
  // transition strobe from high to low
  if (strobe_ && !new_strobe) {
    Input input;
    input.A = glfwGetKey(&window_, GLFW_KEY_X) == GLFW_PRESS;
    input.B = glfwGetKey(&window_, GLFW_KEY_Z) == GLFW_PRESS;
    input.Select = glfwGetKey(&window_, GLFW_KEY_A) == GLFW_PRESS;
    input.Start = glfwGetKey(&window_, GLFW_KEY_S) == GLFW_PRESS;
    input.Up = glfwGetKey(&window_, GLFW_KEY_UP) == GLFW_PRESS;
    input.Down = glfwGetKey(&window_, GLFW_KEY_DOWN) == GLFW_PRESS;
    input.Left = glfwGetKey(&window_, GLFW_KEY_LEFT) == GLFW_PRESS;
    input.Right = glfwGetKey(&window_, GLFW_KEY_RIGHT) == GLFW_PRESS;

    joy1_register_ = input.A |              //
                     (input.B << 1) |       //
                     (input.Select << 2) |  //
                     (input.Start << 3) |   //
                     (input.Up << 4) |      //
                     (input.Down << 5) |    //
                     (input.Left << 6) |    //
                     (input.Right << 7);

    BOOST_LOG_TRIVIAL(debug)
        << "polling controller with input: "
        << std::format(
               "A: {}, B: {}, Select: {}, "
               "Start: {}, Up: {}, Down: {}, "
               "Left: {}, Right: {}",
               input.A, input.B, input.Select, input.Start, input.Up,
               input.Down, input.Left, input.Right);
  }
  strobe_ = new_strobe;
};

uint8_t Controller::read_joy1() {
  uint8_t bit = joy1_register_ & 0x01;
  joy1_register_ >>= 1;
  return bit;
};

}  // namespace controller