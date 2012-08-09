// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @version $Id: rom.cpp 161 2012-03-05 08:01:06Z frigolit $
// =============================================================================

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rom.h"

CROM::CROM(uint16_t s) {
	rom = NULL;
	size = s;
}

CROM::~CROM() {
	if (rom != NULL) free(rom);
}

uint8_t CROM::read(uint16_t addr) {
	if (rom == NULL) return 0xFF;
	
	if (addr >= size) {
		printf("CROM::read(): error: address 0x%04X is out of bounds - this should never happen\n", addr);
		exit(1);
	}
	
	return rom[addr];
}

void CROM::write(uint16_t addr, uint8_t data) {
	printf("CROM::write(): warning: cannot write to rom\n");
}

bool CROM::load_from_file(char *fn) {
	FILE *fp = fopen(fn, "rb");
	if (fp == NULL) {
		printf("CROM::load_from_file(): error: couldn't open rom \"%s\"\n", fn);
		return false;
	}
	
	if (rom == NULL) {
		if (!allocate()) return false;
	}
	
	long int r = fread(rom, 1, size, fp);
	if (r < size) {
		printf("CROM::load_from_file(): warning: rom file \"%s\" only contains %ld byte(s), should be %d byte(s)\n", fn, r, size);
	}
	
	fclose(fp);
	return true;
}

void CROM::load(char *src, int offset, int size) {
	if (rom == NULL) {
		if (!allocate()) return;
	}
	
	memcpy(rom + offset, src, size);
}

bool CROM::allocate() {
	if (rom != NULL) return false;
	
	rom = (uint8_t*)malloc(size);
	if (rom == NULL) {
		printf("CROM::allocate(): error: couldn't allocate %d byte rom\n", size);
		return false;
	}
	
	memset(rom, 0xFF, size);
	return true;
}

