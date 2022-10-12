#ifdef FRONTEND_CLI

#include <cstring>
#include <iostream>
#include <mutex>

#include "lcd_interface.h"

#define PIXEL(x, y) (!!(pixels[x] & (1 << (y))))

CliLcdInterface::CliLcdInterface() {
	mtx = new std::mutex();

	memset(pixels, 0xFF, 480);
	memset(changes, 0xFF, 120);
}

CliLcdInterface::~CliLcdInterface() {
	delete(mtx);
}

void CliLcdInterface::set_pixel(uint8_t x, uint8_t y) {
	mtx->lock();

	pixels[x] |= (1 << y);
	changes[(x / 2)] |= ((uint16_t)1 << (y / 2));

	mtx->unlock();
}

void CliLcdInterface::clear_pixel(uint8_t x, uint8_t y) {
	mtx->lock();

	pixels[x] &= ~((uint32_t)1 << y);
	changes[(x / 2)] |= ((uint16_t)1 << (y / 2));

	mtx->unlock();
}

void CliLcdInterface::update() {
	bool did_draw = false;
	int px, py;

	mtx->lock();

	for (int y = 0; y < 16; y++) {
		for (int x = 0; x < 60; x++) {
			if (changes[x] & ((uint32_t)1 << y)) {
				if (!did_draw) {
					did_draw = true;
					std::cout << "\e[38;2;18;21;19m";
					std::cout << "\e[48;2;118;132;125m";
				}

				px = x * 2;
				py = y * 2;

				std::cout << "\e[" << (y + 2) << ";" << (x + 2) << "H";

				uint8_t c =
					(PIXEL(px, py))
					| (PIXEL(px + 1, py) << 1)
					| (PIXEL(px, py + 1) << 2)
					| (PIXEL(px + 1, py + 1) << 3);

				if (c == 0) {
					std::cout << " ";
				}
				else if (c == 1) {
					std::cout << "\xE2\x96\x98";
				}
				else if (c == 2) {
					std::cout << "\xE2\x96\x9D";
				}
				else if (c == 3) {
					std::cout << "\xE2\x96\x80";
				}
				else if (c == 4) {
					std::cout << "\xE2\x96\x96";
				}
				else if (c == 5) {
					std::cout << "\xE2\x96\x8C";
				}
				else if (c == 6) {
					std::cout << "\xE2\x96\x9E";
				}
				else if (c == 7) {
					std::cout << "\xE2\x96\x9B";
				}
				else if (c == 8) {
					std::cout << "\xE2\x96\x97";
				}
				else if (c == 9) {
					std::cout << "\xE2\x96\x9A";
				}
				else if (c == 10) {
					std::cout << "\xE2\x96\x90";
				}
				else if (c == 11) {
					std::cout << "\xE2\x96\x9C";
				}
				else if (c == 12) {
					std::cout << "\xE2\x96\x84";
				}
				else if (c == 13) {
					std::cout << "\xE2\x96\x99";
				}
				else if (c == 14) {
					std::cout << "\xE2\x96\x9F";
				}
				else if (c == 15) {
					std::cout << "\xE2\x96\x88";
				}
			}
		}
	}

	if (did_draw) {
		// Clear colours and flush output
		std::cout << "\e[0m" << std::flush;
	}

	memset(changes, 0, 120);

	mtx->unlock();
}

void CliLcdInterface::redraw() {
	mtx->lock();

	std::cout << "\e[1;1H";

	std::cout << "\e[38;2;18;21;19m";
	std::cout << "\e[48;2;118;132;125m";

	for (int i = 0; i < 62; i++) {
		std::cout << "\xE2\x96\x91";
	}
	std::cout << std::endl;

	int px, py;
	for (int y = 0; y < 16; y++) {
			std::cout << "\xE2\x96\x91";
			for (int x = 0; x < 60; x++) {
					px = x * 2;
					py = y * 2;

					uint8_t c =
						(PIXEL(px, py))
						| (PIXEL(px + 1, py) << 1)
						| (PIXEL(px, py + 1) << 2)
						| (PIXEL(px + 1, py + 1) << 3);

					if (c == 0) {
						std::cout << " ";
					}
					else if (c == 1) {
						std::cout << "\xE2\x96\x98";
					}
					else if (c == 2) {
						std::cout << "\xE2\x96\x9D";
					}
					else if (c == 3) {
						std::cout << "\xE2\x96\x80";
					}
					else if (c == 4) {
						std::cout << "\xE2\x96\x96";
					}
					else if (c == 5) {
						std::cout << "\xE2\x96\x8C";
					}
					else if (c == 6) {
						std::cout << "\xE2\x96\x9E";
					}
					else if (c == 7) {
						std::cout << "\xE2\x96\x9B";
					}
					else if (c == 8) {
						std::cout << "\xE2\x96\x97";
					}
					else if (c == 9) {
						std::cout << "\xE2\x96\x9A";
					}
					else if (c == 10) {
						std::cout << "\xE2\x96\x90";
					}
					else if (c == 11) {
						std::cout << "\xE2\x96\x9C";
					}
					else if (c == 12) {
						std::cout << "\xE2\x96\x84";
					}
					else if (c == 13) {
						std::cout << "\xE2\x96\x99";
					}
					else if (c == 14) {
						std::cout << "\xE2\x96\x9F";
					}
					else if (c == 15) {
						std::cout << "\xE2\x96\x88";
					}
			}
			std::cout << "\xE2\x96\x91" << std::endl;
	}

	for (int i = 0; i < 62; i++) {
		std::cout << "\xE2\x96\x91";
	}

	std::cout << "\e[0m" << std::flush;

	memset(changes, 0, 120);

	mtx->unlock();
}

#endif
