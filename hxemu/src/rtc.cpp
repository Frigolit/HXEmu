// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @version $Id: rtc.cpp 94 2011-06-27 21:42:57Z frigolit $
// =============================================================================

#include <stdio.h>
#include <string.h>

#include "rtc.h"

CRTC::CRTC() {
	memset(&memory, 0, 64);
}

CRTC::~CRTC() {
}

// TODO!
uint8_t CRTC::read(uint16_t addr) {
	//printf("CRTC::read(0x%04X): fixme: stub\n", addr);
	return memory[addr];
}

void CRTC::write(uint16_t addr, uint8_t data) {
	//printf("CRTC::write(0x%04X, 0x%02X): fixme: stub\n", addr, data);
	memory[addr] = data;
}

