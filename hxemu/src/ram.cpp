// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @version $Id: ram.cpp 166 2012-04-05 10:06:00Z frigolit $
// =============================================================================

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ram.h"

CRAM::CRAM(uint16_t size) {
	this->size = size;
	ram = (uint8_t*)malloc(size);
	if (ram == NULL) {
		printf("CRAM::CRAM(): error: couldn't initialize %d byte ram\n", size);
		return;
	}
	
	memset(ram, 0xFF, size);
}

CRAM::~CRAM() {
	free(ram);
}

uint8_t CRAM::read(uint16_t addr) {
	if (addr >= size) {
		printf("CRAM::read(): error: address 0x%04X is out of bounds - this should never happen\n", addr);
		exit(1);
	}
	
	return ram[addr];
}

void CRAM::write(uint16_t addr, uint8_t data) {
	if (addr >= size) {
		printf("CRAM::write(): error: address 0x%04X is out of bounds - this should never happen\n", addr);
		exit(1);
	}
	
	ram[addr] = data;
}

