// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#include <stdio.h>

#include "iocontroller.h"

CIOController::CIOController() {
	memset(&keyboard_map, 0, 256);

	for (int i = 0; i < 6; i++) {
		lcd_ctls[i] = NULL;
	}

	lcd_clk_counter = 0;
	r_9g = 0x00;
}

CIOController::~CIOController() {
	for (int i = 0; i < 6; i++) {
		lcd_ctls[i] = NULL;
	}
}

void CIOController::reset() {
	lcd_clk_counter = 0;
	for (int i = 0; i < 6; i++) {
		lcd_ctls[i]->reset();
	}

	lcd_clk_counter = 0;
	r_9g = 0x00;
}

void CIOController::set_lcd_controller(uint8_t n, CLCDController *c) {
	lcd_ctls[n] = c;
}

uint8_t CIOController::read(uint16_t addr) {
	#ifdef DEBUG_IOCTL
		printf("CIOController::read(0x%04X) = 0x", addr);
	#endif

	if (addr == 0x20) {
		#ifdef DEBUG_IOCTL
			printf("%02X\n", 0x08);
		#endif
		return 0x08;
	}
	else if (addr == 0x22) {
		// KRTN 0-7
		uint8_t l = ~ram[0x20];
		uint8_t n = 0xFF;

		if (l & 0x01) n ^= keyboard_map[(uint8_t)'0']  | (keyboard_map[(uint8_t)'1'] << 1) | (keyboard_map[(uint8_t)'2'] << 2)  | (keyboard_map[(uint8_t)'3'] << 3) | (keyboard_map[(uint8_t)'4'] << 4) | (keyboard_map[(uint8_t)'5'] << 5)  | (keyboard_map[(uint8_t)'6'] << 6) | (keyboard_map[(uint8_t)'7'] << 7);
		if (l & 0x02) n ^= keyboard_map[(uint8_t)'8']  | (keyboard_map[(uint8_t)'9'] << 1) | (keyboard_map[(uint8_t)':'] << 2)  | (keyboard_map[(uint8_t)';'] << 3) | (keyboard_map[(uint8_t)','] << 4) | (keyboard_map[(uint8_t)'-'] << 5)  | (keyboard_map[(uint8_t)'.'] << 6) | (keyboard_map[(uint8_t)'/'] << 7);
		if (l & 0x04) n ^= keyboard_map[(uint8_t)'@']  | (keyboard_map[(uint8_t)'a'] << 1) | (keyboard_map[(uint8_t)'b'] << 2)  | (keyboard_map[(uint8_t)'c'] << 3) | (keyboard_map[(uint8_t)'d'] << 4) | (keyboard_map[(uint8_t)'e'] << 5)  | (keyboard_map[(uint8_t)'f'] << 6) | (keyboard_map[(uint8_t)'g'] << 7);
		if (l & 0x08) n ^= keyboard_map[(uint8_t)'h']  | (keyboard_map[(uint8_t)'i'] << 1) | (keyboard_map[(uint8_t)'j'] << 2)  | (keyboard_map[(uint8_t)'k'] << 3) | (keyboard_map[(uint8_t)'l'] << 4) | (keyboard_map[(uint8_t)'m'] << 5)  | (keyboard_map[(uint8_t)'n'] << 6) | (keyboard_map[(uint8_t)'o'] << 7);
		if (l & 0x10) n ^= keyboard_map[(uint8_t)'p']  | (keyboard_map[(uint8_t)'q'] << 1) | (keyboard_map[(uint8_t)'r'] << 2)  | (keyboard_map[(uint8_t)'s'] << 3) | (keyboard_map[(uint8_t)'t'] << 4) | (keyboard_map[(uint8_t)'u'] << 5)  | (keyboard_map[(uint8_t)'v'] << 6) | (keyboard_map[(uint8_t)'w'] << 7);
		if (l & 0x20) n ^= keyboard_map[(uint8_t)'x']  | (keyboard_map[(uint8_t)'y'] << 1) | (keyboard_map[(uint8_t)'z'] << 2)  | (keyboard_map[(uint8_t)'['] << 3) | (keyboard_map[(uint8_t)']'] << 4) | (keyboard_map[(uint8_t)'\\'] << 5) | (keyboard_map[0] << 6) | (keyboard_map[0] << 7);
		if (l & 0x40) n ^= keyboard_map[(uint8_t)'\n'] | (keyboard_map[(uint8_t)' '] << 1) | (keyboard_map[(uint8_t)'\t'] << 2) | (keyboard_map[0] << 3) | (keyboard_map[0] << 4) | (keyboard_map[0] << 5) | (keyboard_map[0] << 6) | (keyboard_map[0] << 7);
		if (l & 0x80) n ^= keyboard_map[0] | (keyboard_map[0] << 1) | (keyboard_map[0] << 2) | (keyboard_map[0] << 3) | (keyboard_map[0] << 4) | (keyboard_map[0] << 5) | (keyboard_map[0] << 6) | (keyboard_map[0] << 7);

		#ifdef DEBUG_IOCTL
			printf("%02X\n", n);
		#endif
		return n;
	}
	else if (addr == 0x28) {
		bool b_power = true;
		bool b_busy = true;

		uint8_t l = ~ram[0x20];
		uint8_t k = 0x03;

		if (l & 0x01) k ^= 0x00;
		if (l & 0x02) k ^= 0x00;
		if (l & 0x04) k ^= 0x20;
		if (l & 0x08) k ^= 0x00;
		if (l & 0x10) k ^= 0x00;
		if (l & 0x20) k ^= (keyboard_map[0x00] | keyboard_map[0x01]) << 1;	// Shift
		if (l & 0x40) k ^= 0x00;
		if (l & 0x80) k ^= 0x00;

		uint8_t n = (b_power << 7) | (b_busy << 6) | k;

		#ifdef DEBUG_IOCTL
			printf("%02X\n", n);
		#endif
		return n;
	}
	else if (addr == 0x2A || addr == 0x2B) {
		if (++lcd_clk_counter == 8) {
			lcd_clk_counter = 0;

			if (r_9g & 0x07) {
				uint8_t n = (r_9g & 0x07) - 1;

				if (r_9g & 0x08) lcd_ctls[n]->command(ram[0x2A]);
				else lcd_ctls[n]->data(ram[0x2A]);
			}
		}
	}

	#ifdef DEBUG_IOCTL
		printf("FF\n");
	#endif
	return 0xFF;
}

void CIOController::write(uint16_t addr, uint8_t data) {
	#ifdef DEBUG_IOCTL
		printf("CIOController::write(0x%04X, 0x%02X)\n", addr, data);
	#endif

	if (addr == 0x26) r_9g = data;
	else ram[addr] = data;
}

