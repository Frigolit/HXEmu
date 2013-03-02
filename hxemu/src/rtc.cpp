// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#include <stdio.h>
#include <string.h>

#include "rtc.h"

CRTC::CRTC() {
	memset(&memory, 0, 50);
}

CRTC::~CRTC() {
}

// TODO!
uint8_t CRTC::read(uint16_t addr) {
	if (addr > 63) {
		addr = addr % 64;
		printf("\e[31m\e[1mCRTC::read(0x%04X): warning: read outside range - wrapping\e[0m\n", addr);
	}

	/*
	00     Seconds
	01     Seconds (alarm)
	02     Minutes
	03     Minutes (alarm)
	04     Hours
	05     Hours (alarm)
	06     Day of week
	07     Date
	08     Month
	09     Year
	0A-0D  Control registers
	0E-3F  General-purpose RAM
	*/

	if (addr <= 0x0D) return 0x00;
	else return memory[addr - 0x0D];
}

void CRTC::write(uint16_t addr, uint8_t data) {
	if (addr > 63) {
		addr = addr % 64;
		printf("\e[31m\e[1mCRTC::read(0x%04X, 0x%02X): warning: write outside range - wrapping\e[0m\n", addr, data);
	}

	if (addr > 0x0D) memory[addr - 0x0D] = data;
}

