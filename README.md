HXEmu
=====
HXEmu is a Epson HX-20 emulator written in C++ for Windows, Linux and Mac OS X.
It's licensed under the MIT license, see the LICENSE file for more information.

How to compile
--------------
You can easily compile HXEmu by running the ./compile script which performs some basic auto-detection of the platform and build environment.
It requires a compiler (e.g. GCC, Clang) with support for the C++11 standard.
It also requires the following libraries (in some Linux distributions these will be "-dev" packages):
- SDL2
- SDL2_ttf
- SDL2_image
- jsoncpp
- pthreads

Contributors
------------
Thanks to everyone who has contributed to this project.
- Pontus Rodling (@frigolit) - Project creator, lead developer.
- Harald Lapp (@aurora) - Various fixes, support for compiling on OS X.
- Dmitrii Eliuseev (@dmitryelj) - SDL2 support.
