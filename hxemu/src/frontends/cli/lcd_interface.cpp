#ifdef FRONTEND_CLI

#include <cstdio>
#include <cstring>

#include "lcd_interface.h"

#define PIXEL(x, y) (!!(pixels[((x) + ((y) * 120)) / 8] & (1 << ((x) % 8))))

CliLcdInterface::CliLcdInterface() {
	memset(pixels, 0xFF, 480);
	memset(changes, 0, 120);
}

void CliLcdInterface::set_pixel(uint8_t x, uint8_t y) {
	int pn = (x + (y * 120)) / 8;

	pixels[pn] |= (1 << (x % 8));
}

void CliLcdInterface::clear_pixel(uint8_t x, uint8_t y) {
	int pn = (x + (y * 120)) / 8;

	pixels[pn] &= ~(1 << (x % 8));
}

void CliLcdInterface::update() {
}

void CliLcdInterface::redraw() {
	printf("\e[1;1H");

	printf("\e[38;2;18;21;19m");
	printf("\e[48;2;118;132;125m");
	printf("##############################################################\n");

	int px, py;
	for (int y = 0; y < 16; y++) {
			putc('#', stdout);
			for (int x = 0; x < 60; x++) {
					px = x * 2;
					py = y * 2;

					//printf("%d, %d = %d - %d\n", px, py, ((px) + ((py) * 120)) / 8, ((px + 1) + ((py + 1) * 120)) / 8);
					//continue;

					uint8_t c =
						(PIXEL(px, py))
						| (PIXEL(px + 1, py) << 1)
						| (PIXEL(px, py + 1) << 2)
						| (PIXEL(px + 1, py + 1) << 3);

					// todo
					if (c == 0) {
						putc(' ', stdout);
					}
					else if (c == 1) {
						printf("\xE2\x96\x98");
					}
					else if (c == 2) {
						printf("\xE2\x96\x9D");
					}
					else if (c == 3) {
						printf("\xE2\x96\x80");
					}
					else if (c == 4) {
						printf("\xE2\x96\x96");
					}
					else if (c == 5) {
						printf("\xE2\x96\x8C");
					}
					else if (c == 6) {
						printf("\xE2\x96\x9E");
					}
					else if (c == 7) {
						printf("\xE2\x96\x9B");
					}
					else if (c == 8) {
						printf("\xE2\x96\x97");
					}
					else if (c == 9) {
						printf("\xE2\x96\x9A");
					}
					else if (c == 10) {
						printf("\xE2\x96\x90");
					}
					else if (c == 11) {
						printf("\xE2\x96\x9C");
					}
					else if (c == 12) {
						printf("\xE2\x96\x84");
					}
					else if (c == 13) {
						printf("\xE2\x96\x99");
					}
					else if (c == 14) {
						printf("\xE2\x96\x9F");
					}
					else if (c == 15) {
						printf("\xE2\x96\x88");
					}
			}
			printf("#\n");
	}

	for (int i = 0; i < 62; i++) {
		putc('#', stdout);
	}

	printf("\e[0m\n");

}

#endif
