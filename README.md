# nesemu

## Development

We are using the Google C++ style guidelines. Please use clang-format and skim
over the [style guide](https://google.github.io/styleguide/cppguide.html) to
ensure that your code conforms.

This project requires the Catch2 (v2) and Boost libraries.

## Building + Running

1. Run CMake
2. `nesemu <repo-root>/vendor/out/<rom-name>.nes`
   For example: `./nesemu ../vendor/out/color_test_nosprites.nes`
