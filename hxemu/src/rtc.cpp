// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "logging/logger.h"

#include "rtc.h"

CRTC::CRTC() {
	memset(ram, 0, 50);
}

CRTC::~CRTC() {
}

// TODO!
uint8_t CRTC::read(uint16_t addr) {
	if (addr > 63) {
		logger->fatal("Assertion failed: addr < 64 in CRTC::read()");
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

	if (addr == 0x00 || addr == 0x02 || addr == 0x04 || addr == 0x06 || addr == 0x07 || addr == 0x08 || addr == 0x09) {
		time_t t = time(NULL);
		tm *lt = localtime(&t);

		switch (addr) {
			case 0x00: return ((lt->tm_sec / 10) << 4) | (lt->tm_sec % 10);
			case 0x02: return ((lt->tm_min / 10) << 4) | (lt->tm_min % 10);
			case 0x04: return ((lt->tm_hour / 10) << 4) | (lt->tm_hour % 10);
			case 0x06: return lt->tm_wday;
			case 0x07: return ((lt->tm_mday / 10) << 4) | (lt->tm_mday % 10);
			case 0x08: return (((lt->tm_mon + 1) / 10) << 4) | ((lt->tm_mon + 1) % 10);
			case 0x09: {
				int y = lt->tm_year % 100;
				return ((y / 10) << 4) | (y % 10);
			}
			default:
				return 0x00;
		}
	}
	else if (addr < 0x0E) {
		return 0x00;
	}
	else {
		#ifdef DEBUG_RTC
			printf("\e[32m\e[1mCRTC::read(0x%04X[RAM 0x%02X]) = %02X\e[0m\n", addr, (uint8_t)(addr - 0x0E), ram[addr - 0x0E]);
		#endif
		return ram[addr - 0x0E];
	}
}

bool CRTC::write(uint16_t addr, uint8_t data) {
	if (addr > 63) {
		logger->fatal("Assertion failed: addr < 64 in CRTC::read()");
		exit(1);
	}

	if (addr >= 0x0E) {
		#ifdef DEBUG_RTC
			printf("\e[31m\e[1mCRTC::write(0x%04X [RAM 0x%02X], %02X)\e[0m\n", addr, (uint8_t)(addr - 0x0E), data);
		#endif
		ram[addr - 0x0E] = data;
	}

	return true;
}

