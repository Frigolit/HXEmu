// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rtc.h"

CRTC::CRTC() {
	memset(ram, 0, 50);
}

CRTC::~CRTC() {
}

// TODO!
uint8_t CRTC::read(uint16_t addr) {
	if (addr > 63) {
		printf("Assertion failed: addr < 64 in CRTC::read()\n");
		exit(1);
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

	if (addr < 0x0E) return 0x00;
	else {
		#ifdef DEBUG_RTC
			printf("\e[32m\e[1mCRTC::read(0x%04X[RAM 0x%02X]) = %02X\e[0m\n", addr, (uint8_t)(addr - 0x0E), ram[addr - 0x0E]);
		#endif
		return ram[addr - 0x0E];
	}
}

void CRTC::write(uint16_t addr, uint8_t data) {
	if (addr > 63) {
		printf("Assertion failed: addr < 64 in CRTC::read()\n");
		exit(1);
	}

	if (addr >= 0x0E) {
		#ifdef DEBUG_RTC
			printf("\e[31m\e[1mCRTC::write(0x%04X [RAM 0x%02X], %02X)\e[0m\n", addr, (uint8_t)(addr - 0x0E), data);
		#endif
		ram[addr - 0x0E] = data;
	}
}

