= nesemu
A simple Nintendo Entertainment System emulator written in C++ for SENG475.

Built by:
- Brennan McMicking (V00939944)
- Malcolm Seyd (V00938975)
- Anika Bookout (V009662976)
- Max Thomson (V00969053)


== Quickstart
$ export INSTALL_DIR=install
$ cmake -H. -Bbuild -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR/
$ cmake --build build --clean-first --target install
$ ./$INSTALL_DIR/bin/demo

== Demo Program

Note: The provided demo program times out after a short time 
(30 seconds) to pass the requirements of assignment_precheck.
This simply closes the program after the timeout duration.


== Development

We are using the Google C++ style guidelines. Please use clang-format and skim
over the style guide to ensure that your code conforms.
https://google.github.io/styleguide/cppguide.html

This project requires the Catch2 (v2), Boost, OpenGL, and GLFW libraries.

=== Building + Running

Building the main emulator program:

1. Run CMake Build
2. `./nesemu <rom path>`
E.g. `$INSTALL_DIR/bin/nesemu $INSTALL_DIR/usr/share/nesemu/color_test_nosprites.nes`


(OPTIONAL) Building the color_test_nosprites.nes ROM file from source
(vendor/color_test_nosprites/color_test_nosprites.s)

1. Install the cc65 assembler (https://cc65.github.io/)
2. `cd <repo-root>/vendor`
3. `make`

=== Debugging

The main emulator program can be run in "debug mode" using the "--debug" flag.
This causes the program to stop execution on the first instruction so that the
user may perform various debugging commands such as instruction stepping,
setting breakpoints, and reading/writing memory and registers.

The following is an example of how the program may be run in debug mode:

$ ./$INSTALL_DIR/bin/nesemu $INSTALL_DIR/usr/share/nesemu/color_test_nosprites.nes --debug

The program then prints a message explaining that debug mode is active and that
you may run the "help" command to view all commands. Running the "help" command
will explain how each command may be used.

One of the most notable commands is "step" which executes the next instruction
and prints the name of the instruction which was executed and the subsequent
CPU register state.

==== Caveats

1. Some memory addresses are clear-on-read and reading them from the debugger
will clear them. For example: reading `$2002` when the VBlank bit is set will
clear the VBlank bit. As a result, the cartridge you're running may end up
acting in unexpected ways.

2. The emulator program is not guaranteed to work with any ROM other than the
provided "color_test_nosprites.nes" file. Most (if not almost all) ROMs use
unsupported and untested features such as the APU (audio), scrolling, and
sprite rendering which result in undefined behaviour (these unsupported
features were specified in the project approval).

=== Tests

There are unit tests for the CPU instructions & Cartridge reading. After
building, the `test-cpu` & `test-cartridge` executables are available in the
build directory (NOT the install directory).

For `test-cpu`, all 151 CPU instructions were thouroughly tested with each
possible branch (for flags, etc) being covered.

=== Continuous Integration

There is a GitHub Actions Workflow (`.github/workflows/ci.yml`) that runs both
unit tests on every commit to provide a source of truth. The "commits" view in
GitHub will show either a green checkmark or a red crossmark on each commit.
Clicking on this icon will show the CI (test) output
https://github.com/uvic-seng475-2023-05/cpp_project-nesemu/commits/main

The CI job runs a custom built Docker image that represents the SDE. The source
is in `Dockerfile` and runs the SDE setup scripts to configure the environment.


== Stats

Total Commits:           245+
Files Changed:           16116 insertions(+) 5484 deletions(-)
Monster Energy Consumed: 8

-------------------------------------------------------------------------------
 Language            Files        Lines         Code     Comments       Blanks
-------------------------------------------------------------------------------
 C++                    18        10001         7732          373         1896
 C++ Header              6          906          536          253          117
 Python                  1          245          126           85           34
 CMake                   2          125           89           16           20
 Dockerfile              1           86           45           25           16
 Makefile                1           12            9            0            3
 BASH                    1           11            6            1            4
-------------------------------------------------------------------------------
 Total                  30        11386         8543          753         2090
-------------------------------------------------------------------------------
