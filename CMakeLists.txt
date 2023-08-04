cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(nesemu LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall -Wextra -g3")
set(OpenGL_GL_PREFERENCE "LEGACY")
include(Sanitizers.cmake)

find_package(Catch2 2 REQUIRED)
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
set(Boost_NO_WARN_NEW_VERSIONS 1)
find_package(Boost COMPONENTS log program_options REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(include)
include_directories(${Boost_INCLUDE_DIR})

file(GLOB CPU_SOURCES "lib/cpu/*.cpp")
file(GLOB PPU_SOURCES "lib/ppu/*.cpp")
file(GLOB CARTRIDGE_SOURCES "lib/cartridge/*.cpp")
file(GLOB CONTROLLER_SOURCES "lib/controller/*.cpp")
file(GLOB DEBUGGER_SOURCES "lib/debugger/*.cpp")
file(GLOB UTIL_SOURCES "lib/util/*.cpp")

add_library(cpu STATIC ${CPU_SOURCES})
target_link_libraries(cpu PUBLIC Boost::log)

add_library(ppu STATIC ${PPU_SOURCES})
target_link_libraries(ppu PUBLIC Boost::log glfw OpenGL::GL)

add_library(cartridge STATIC ${CARTRIDGE_SOURCES})
target_link_libraries(cartridge PUBLIC Boost::log)

add_library(controller STATIC ${CONTROLLER_SOURCES})
target_link_libraries(controller PUBLIC Boost::log)

add_library(debugger STATIC ${DEBUGGER_SOURCES})
target_link_libraries(debugger PUBLIC Boost::log)

add_library(util STATIC ${UTIL_SOURCES})
target_link_libraries(util PUBLIC Boost::log)

# deliverables
add_executable(nesemu app/nesemu.cpp)
target_link_libraries(nesemu PUBLIC cpu ppu cartridge controller debugger util Boost::log Boost::program_options glfw)

add_executable(nesemu-cpu app/nesemu-cpu.cpp)
target_link_libraries(nesemu-cpu PUBLIC cpu ppu controller cartridge util Boost::log)

add_executable(nesemu-cartridge app/nesemu-cartridge.cpp)
target_link_libraries(nesemu-cartridge PUBLIC cartridge util Boost::log)

# testing
add_executable(test-cpu app/catch2_main.cpp app/test-cpu.cpp)
target_link_libraries(test-cpu PUBLIC cpu ppu cartridge controller util Boost::log)

add_executable(test-ppu app/catch2_main.cpp app/test-ppu.cpp)
target_link_libraries(test-ppu PUBLIC ppu util Boost::log)

add_executable(test-cartridge app/catch2_main.cpp app/test-cartridge.cpp)
target_link_libraries(test-cartridge PUBLIC cartridge util Boost::log)

# install
install(TARGETS nesemu nesemu-cpu nesemu-cartridge DESTINATION bin)
install(PROGRAMS demo DESTINATION bin)
install(FILES vendor/out/color_test_nosprites.nes DESTINATION usr/share/nesemu)