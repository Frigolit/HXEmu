#include "testdevice.h"
#include <string.h>

/*
	0x4000-0x4FFF	ROM
	0x5000-0x57FF	Dynamic ROM
	0x5800-0x5FEF	-- unused --
	0x5FF0-0x5FFF	Registers

	=== Registers ===
	0x5FF0		Status flags (read), Control bits (write)
	0x5FF1		Test result register (write-only)
	0x5FF2		Debug message register (read: to HX-20 screen, write: to computer)
*/

CTestDevice::CTestDevice() {
	state = 0;

	rom = new CROM(4096);
	rom->load_from_file((char *)"data/roms/ext-test/test.bin");

	dynrom = (uint8_t *)malloc(sizeof(uint8_t) * 2048);
	memset(dynrom, 0, 2048);

	// Load a small program into the dynamic ROM. It just sends a few characters to the test result port.
	memcpy(dynrom, "\x86H\xB7\x5F\xF1\x86X\xB7\x5F\xF1\x86""E\xB7\x5F\xF1\x86m\xB7\x5F\xF1\x86u\xB7\x5F\xF1\x39", 26);
}

CTestDevice::~CTestDevice() {
	delete(rom);
	free(dynrom);
}

uint8_t CTestDevice::read(uint16_t addr, CMemoryDevice *mem) {
	if (addr >= 0x4000 && addr < 0x6000) {
		uint8_t r = 0xFF;
		if (addr < 0x5000) {
			r = rom->read(addr - 0x4000);
		}
		else if (addr < 0x5800) {
			r = dynrom[addr - 0x5000];
			printf("Read DynROM at 0x%04X = 0x%02X\n", addr - 0x5000, r);
		}
		else if (addr == 0x5FF0) {
			return 0x80;
		}

		return r;
	}
	else return mem->read(addr);
}

void CTestDevice::write(uint16_t addr, uint8_t data, CMemoryDevice *mem) {
	if (addr >= 0x4000 && addr < 0x6000) {
		if (addr == 0x5FF0) {
			printf("[Test control] 0x%02X\n", data);
		}
		else if (addr == 0x5FF1) {
			printf("[Test result] 0x%02X\n", data);
		}
		else if (addr == 0x5FF2) {
			printf("\e[32;1m%c\e[0m", data);
		}
	}
	else mem->write(addr, data);
}

