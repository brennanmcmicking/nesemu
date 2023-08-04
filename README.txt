= nesemu
A simple Nintendo Entertainment System emulator written in C++ for SENG475

Built by:
- Brennan McMicking (V00939944)
- Malcolm Seyd (V00938975)
- Anika Bookout (V009662976)
- Max Thomson (V00969053)

== Quickstart
$ cmake -H. -Bbuild -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR/
$ cmake --build build --clean-first --target install
$ ./$INSTALL_DIR/bin/demo

== Development

We are using the Google C++ style guidelines. Please use clang-format and skim
over the style guide to ensure that your code conforms.
https://google.github.io/styleguide/cppguide.html

This project requires Catch2 (v2), Boost, & GLFW
