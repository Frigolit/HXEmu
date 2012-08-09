// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @version $Id: virtualserial.cpp 106 2012-02-11 11:13:30Z frigolit $
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "virtualserial.h"
#include "config.h"

VirtualSerial::VirtualSerial() {
	buf = (uint8_t*)malloc(VIRTUALSERIAL_BUFFERSIZE);
	buflen = 0;
}

VirtualSerial::~VirtualSerial() {
	free(buf);
}

void VirtualSerial::reset() {
	buflen = 0;
}

void VirtualSerial::send(uint8_t c) {
	if (endpoint != NULL) endpoint->_recv(c);
}

void VirtualSerial::_recv(uint8_t c) {
	buffer_append(c);
}

uint8_t VirtualSerial::recv() {
	return buffer_pop();
}

uint8_t VirtualSerial::peek() {
	return !!buflen;
}

void VirtualSerial::buffer_append(uint8_t c) {
	if (buflen == VIRTUALSERIAL_BUFFERSIZE) {
		#ifndef AVR
			printf("VirtualSerial::buffer_append(): warning: buffer overflow - character dropped\n");
		#endif
		return;
	}
	
	buf[buflen++] = c;
}

uint8_t VirtualSerial::buffer_pop() {
	if (!buflen) return 0;
	uint8_t r = buf[0];
	for (uint16_t n = 0; n < buflen - 1; n++) {
		buf[n] = buf[n + 1];
	}
	buflen--;
	return r;
}

