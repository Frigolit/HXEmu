# HXEmu
HXEmu is a Epson HX-20 emulator written in C++ for Windows, Linux and Mac OS X.
It's licensed under the MIT license, see the LICENSE file for more information.

This project is still under heavy development and many features are not yet available and/or buggy.

## Binary downloads
Binaries are provided for the following platforms:
- Linux x86_64 (SDL2) via Flatpak
- Windows x86_64 (SDL2)

See the [releases](https://github.com/Frigolit/HXEmu/releases) in GitHub or visit the [project page](https://frigolit.net/projects/hxemu).

## How to compile
The compilation script is written in [Pike](https://pike.lysator.liu.se/) ("pike8.0" or similar package on e.g. Debian), simply run the compilation script after installing the Pike runtime and it'll perform some basic auto-detection of the platform and build environment.
It requires a compiler (e.g. GCC, Clang) with support for the C++11 standard as well as the following libraries (in some Linux distributions these will be "-dev" packages):
- jsoncpp
- pthreads

For SDL2 frontend support:
- SDL2
- SDL2_ttf
- SDL2_image

### Frontends
Frontends are enabled if support is available by default, but can be enabled or disabled specifically by passing the "--frontend-NNN" and "--no-frontend-NNN" flags to the compilation script.
- sdl2 - Simple DirectMedia Layer 2.x
- cli - Command-Line Interface (incomplete, just LCD output for now)

Note: The CLI frontend requires a Unicode-capable terminal emulator and font for proper display.

## Contributors
Thanks to everyone who has contributed to this project.
- Pontus Rodling (@frigolit) - Project creator, lead developer.
- Harald Lapp (@aurora) - Various fixes, support for compiling on OS X.
- Dmitrii Eliuseev (@dmitryelj) - SDL2 support.
