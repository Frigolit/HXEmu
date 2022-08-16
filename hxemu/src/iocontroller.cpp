// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#include <stdio.h>

#include "iocontroller.h"

CIOController::CIOController() {
	b_power = true;

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

void CIOController::set_power_button(bool p) {
	b_power = p;
}

bool CIOController::get_power_button() {
	return b_power;
}

void CIOController::set_lcd_controller(uint8_t n, CLCDController *c) {
	lcd_ctls[n] = c;
}

uint8_t CIOController::read(uint16_t addr) {
#ifdef DEBUG_IOCTL
	uint8_t r = _read(addr);
	printf("CIOController::read(0x%04X) = 0x%02X\n", addr + 0x20, r);
	return r;
}

uint8_t CIOController::_read(uint16_t addr) {
#endif
	addr += 0x20;	// Makes it easier to follow the documentation.

	if (addr == 0x20) {
		printf("CIOController::_read(): Read from 0x20 (KRTN) = 0x%02X\n", ram[0x20]);
		return ram[0x20];
	}
	else if (addr == 0x22) {
		// KRTN 0-7
		uint8_t l = ~ram[0x20];
		uint8_t n = 0x00;

		if (l & 0x01) {
			n |=
				keyboard_map['0']
				| (keyboard_map['1'] << 1)
				| (keyboard_map['2'] << 2)
				| (keyboard_map['3'] << 3)
				| (keyboard_map['4'] << 4)
				| (keyboard_map['5'] << 5)
				| (keyboard_map['6'] << 6)
				| (keyboard_map['7'] << 7);
		}

		if (l & 0x02) {
			n |=
				keyboard_map['8']
				| (keyboard_map['9'] << 1)
				| (keyboard_map[':'] << 2)
				| (keyboard_map[';'] << 3)
				| (keyboard_map[','] << 4)
				| (keyboard_map['-'] << 5)
				| (keyboard_map['.'] << 6)
				| (keyboard_map['/'] << 7);
		}

		if (l & 0x04) {
			n |=
				keyboard_map['@']
				| (keyboard_map['a'] << 1)
				| (keyboard_map['b'] << 2)
				| (keyboard_map['c'] << 3)
				| (keyboard_map['d'] << 4)
				| (keyboard_map['e'] << 5)
				| (keyboard_map['f'] << 6)
				| (keyboard_map['g'] << 7);
		}

		if (l & 0x08) {
			n |=
				keyboard_map['h']
				| (keyboard_map['i'] << 1)
				| (keyboard_map['j'] << 2)
				| (keyboard_map['k'] << 3)
				| (keyboard_map['l'] << 4)
				| (keyboard_map['m'] << 5)
				| (keyboard_map['n'] << 6)
				| (keyboard_map['o'] << 7);
		}

		if (l & 0x10) {
			n |=
				keyboard_map['p']
				| (keyboard_map['q'] << 1)
				| (keyboard_map['r'] << 2)
				| (keyboard_map['s'] << 3)
				| (keyboard_map['t'] << 4)
				| (keyboard_map['u'] << 5)
				| (keyboard_map['v'] << 6)
				| (keyboard_map['w'] << 7);
		}

		if (l & 0x20) {
			n |=
				keyboard_map['x']
				| (keyboard_map['y'] << 1)
				| (keyboard_map['z'] << 2)
				| (keyboard_map['['] << 3)
				| (keyboard_map[']'] << 4)
				| (keyboard_map['\\'] << 5)
				/*| (keyboard_map[0] << 6)
				| (keyboard_map[0] << 7)*/;
		}

		if (l & 0x40) {
			n |=
				keyboard_map['\n']
				| (keyboard_map[' '] << 1)
				| (keyboard_map['\t'] << 2)
				/*| (keyboard_map[0] << 3)
				| (keyboard_map[0] << 4)
				| (keyboard_map[0] << 5)
				| (keyboard_map[0] << 6)
				| (keyboard_map[0] << 7)*/;
		}

		if (l & 0x80) {
			/*
			n ^=
				keyboard_map[0]
				| (keyboard_map[0] << 1)
				| (keyboard_map[0] << 2)
				| (keyboard_map[0] << 3)
				| (keyboard_map[0] << 4)
				| (keyboard_map[0] << 5)
				| (keyboard_map[0] << 6)
				| (keyboard_map[0] << 7);
				*/
		}

		return 0xFF ^ n;
	}
	else if (addr == 0x28) {
		bool b_busy = true;

		uint8_t l = ~ram[0x20];
		uint8_t k = 0xFF;

		if (l & 0x01) { k ^= 0x00; }
		if (l & 0x02) { k ^= 0x00; }
		if (l & 0x04) { k ^= 0x20; }
		if (l & 0x08) { k ^= 0x00; }
		if (l & 0x10) { k ^= 0x00; }
		if (l & 0x20) { k ^= (keyboard_map[0x00] | keyboard_map[0x01]) << 1; }	// Shift
		if (l & 0x40) { k ^= (keyboard_map[0x02] | keyboard_map[0x03]) << 1; }	// Ctrl
		if (l & 0x80) { k ^= 0x00; }

		uint8_t n = (b_power << 7) | (b_busy << 6) | k;

		return n;
	}
	else if (addr == 0x2A || addr == 0x2B) {
		// LCD serial output
		// On the real hardware, each access sends one bit of data to the LCD.
		// We don't care about sending it bit by bit, so we just send it at every byte, hence the counter.

		// r_9g represents register "9G" on the motherboard (TODO: verify!) which holds LCD control bits.
		// xxxxBAAA
		//
		// AAA = LCD controller selection (0: None, 7: Unknown behaviour)
		// B   = Command/Data

		if (++lcd_clk_counter == 8) {
			lcd_clk_counter = 0;

			// TODO: n==6 is ignored due to array out-of-bounds. Find out how the real hardware handles this case.
			if (r_9g & 0x07) {
				uint8_t n = (r_9g & 0x07) - 1;

				if (n < 6) {
					if (r_9g & 0x08) {
						lcd_ctls[n]->command(ram[0x2A]);
					}
					else {
						lcd_ctls[n]->data(ram[0x2A]);
					}
				}
			}
		}

		// TODO: Identify proper return value for these addresses.
		return 0xFF;
	}

	return 0xFF;
}

bool CIOController::write(uint16_t addr, uint8_t data) {
	addr += 0x20;	// Makes it easier to follow the documentation.

	#ifdef DEBUG_IOCTL
		printf("CIOController::write(0x%04X, 0x%02X)\n", addr, data);
	#endif

	if (addr == 0x26) {
		r_9g = data;
	}
	else {
		ram[addr] = data;
	}

	return true;
}

