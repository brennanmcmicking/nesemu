# nesemu

"If you have any undefined behaviour in your spec you're a liberal sissy"

## Development

We are using the Google C++ style guidelines. Please use clang-format and skim
over the [style guide](https://google.github.io/styleguide/cppguide.html) to
ensure that your code conforms.

This project requires the Catch2 (v2) and Boost libraries.

## Building + Running

1. Run CMake
2. `nesemu <repo-root>/vendor/out/<rom-name>.nes`
   For example: `./nesemu ../vendor/out/color_test_nosprites.nes`

## Caveats

1. Some memory addresses are clear-on-read and reading them from the debugger _will_ clear them. For example: reading `$2002` when the VBlank bit is set will clear the VBlank bit. As a result, the cartridge you're running may end up being acting in unexpected ways.
