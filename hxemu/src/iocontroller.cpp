// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#include <cstddef>
#include <stdio.h>

#include "logging/logger.h"

#include "iocontroller.h"

CIOController::CIOController() {
	b_power = true;

	//memset(&keyboard_map, 0, 256);

	for (int i = 0; i < 6; i++) {
		lcd_ctls[i] = NULL;
	}

	lcd_clk_counter = 0;
	r_9g = 0x00;

	for (int i = 0; i < 8; i++) {
		krtn_map[i] = 0;
	}

	// TODO: Allow this to be configured via the graphical interface
	// But for now, set DIP3 for the Swedish character set.
	krtn_map[2] |= (1 << 9);

	// DIP1-3 = Character set selection
	//          000 = ASCII (Swedish)
	//          001 = Swedish
	//          010 = ASCII (French)
	//          011 = French
	//          100 = ASCII (German)
	//          101 = German
	//          110 = Danish
	//          111 = Norwegian
	// DIP4 = Terminal Floppy connected
}

void CIOController::get_dip_switch(bool *d0, bool *d1, bool *d2, bool *d3) {
	*d0 = !!(krtn_map[0] & (1 << 9));
	*d1 = !!(krtn_map[1] & (1 << 9));
	*d2 = !!(krtn_map[2] & (1 << 9));
	*d3 = !!(krtn_map[3] & (1 << 9));
}

void CIOController::set_dip_switch(bool d0, bool d1, bool d2, bool d3) {
	krtn_map[0] = (krtn_map[0] & ~(uint16_t)(1 << 9)) | (!!d0 << 9);
	krtn_map[1] = (krtn_map[1] & ~(uint16_t)(1 << 9)) | (!!d1 << 9);
	krtn_map[2] = (krtn_map[2] & ~(uint16_t)(1 << 9)) | (!!d2 << 9);
	krtn_map[3] = (krtn_map[3] & ~(uint16_t)(1 << 9)) | (!!d3 << 9);
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
	char logbuf[256];
	sprintf(logbuf, "CIOController::read(0x%04X) = 0x%02X", addr + 0x20, r);
	logger->debug(logbuf);
	return r;
}

uint8_t CIOController::_read(uint16_t addr) {
#endif
	addr += 0x20;	// Makes it easier to follow the documentation.

	if (addr == 0x20) {
		char logbuf[256];
		sprintf(logbuf, "CIOController::_read(): Read from 0x20 (KRTN) = 0x%02X\n", ram[0x20]);
		logger->debug(logbuf);

		return ram[0x20];
	}
	else if (addr == 0x22) {
		// KRTN 0-7
		uint8_t l = ~ram[0x20];
		uint8_t n = 0x00;

		if (l & 0x01) {
			n |= (krtn_map[0] & 0xFF);
		}

		if (l & 0x02) {
			n |= (krtn_map[1] & 0xFF);
		}

		if (l & 0x04) {
			n |= (krtn_map[2] & 0xFF);
		}

		if (l & 0x08) {
			n |= (krtn_map[3] & 0xFF);
		}

		if (l & 0x10) {
			n |= (krtn_map[4] & 0xFF);
		}

		if (l & 0x20) {
			n |= (krtn_map[5] & 0xFF);
		}

		if (l & 0x40) {
			n |= (krtn_map[6] & 0xFF);
		}

		if (l & 0x80) {
			n |= (krtn_map[7] & 0xFF);
		}

		return 0xFF ^ n;
	}
	else if (addr == 0x26) {
		// Key interrupt mask
		logger->warn("CIOController()::read(): address 0x26 (key interrupt mask) not implemented");
	}
	else if (addr == 0x28) {
		// KRTN 8-9
		bool b_busy = true;

		uint8_t l = ~ram[0x20];
		uint8_t n = 0x00;

		if (l & 0x01) {
			n |= (krtn_map[0] >> 8) & 0x03;
		}

		if (l & 0x02) {
			n |= (krtn_map[1] >> 8) & 0x03;
		}

		if (l & 0x04) {
			n |= (krtn_map[2] >> 8) & 0x03;
		}

		if (l & 0x08) {
			n |= (krtn_map[3] >> 8) & 0x03;
		}

		if (l & 0x10) {
			n |= (krtn_map[4] >> 8) & 0x03;
		}

		if (l & 0x20) {
			n |= (krtn_map[5] >> 8) & 0x03;
		}

		if (l & 0x40) {
			n |= (krtn_map[6] >> 8) & 0x03;
		}

		if (l & 0x80) {
			n |= (krtn_map[7] >> 8) & 0x03;
		}

		n |= (!b_power << 7) | (!b_busy << 6);

		return 0xFF ^ n;
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
		char logbuf[256];
		sprintf(logbuf, "CIOController::write(0x%04X, 0x%02X)", addr, data);
		logger->debug(logbuf);
	#endif

	if (addr == 0x26) {
		r_9g = data;
	}
	else {
		ram[addr] = data;
	}

	return true;
}

