cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(nesemu LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -g3")

include(Sanitizers.cmake)

find_package(Catch2 2 REQUIRED)
find_package(Boost COMPONENTS log REQUIRED)

include_directories(include)
include_directories(${Boost_INCLUDE_DIR})

file(GLOB CPU_SOURCES "lib/cpu/*.cpp")
file(GLOB PPU_SOURCES "lib/ppu/*.cpp")
file(GLOB CARTRIDGE_SOURCES "lib/cartridge/*.cpp")

add_library(cpu STATIC ${CPU_SOURCES})
add_library(ppu STATIC ${PPU_SOURCES})
add_library(cartridge STATIC ${CARTRIDGE_SOURCES})

# deliverables
add_executable(nesemu app/nesemu.cpp) 
target_link_libraries(nesemu PUBLIC cpu ppu cartridge "${Boost_LOG_LIBRARY}")

add_executable(nesemu-cpu app/nesemu-cpu.cpp) 
target_link_libraries(nesemu-cpu PUBLIC cpu ppu cartridge "${Boost_LOG_LIBRARY}")

add_executable(nesemu-cartridge app/nesemu-cartridge.cpp) 
target_link_libraries(nesemu-cartridge PUBLIC cartridge "${Boost_LOG_LIBRARY}")

# testing
add_executable(test-cpu app/catch2_main.cpp app/test-cpu.cpp)
target_link_libraries(test-cpu PUBLIC cpu ppu cartridge "${Boost_LOG_LIBRARY}")

add_executable(test-ppu app/catch2_main.cpp app/test-ppu.cpp)
target_link_libraries(test-ppu PUBLIC ppu "${Boost_LOG_LIBRARY}")

add_executable(test-cartridge app/catch2_main.cpp app/test-cartridge.cpp)
target_link_libraries(test-cartridge PUBLIC cartridge "${Boost_LOG_LIBRARY}")
