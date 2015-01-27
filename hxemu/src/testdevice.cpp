#include "testdevice.h"

CTestDevice::CTestDevice() {
	rom = new CROM(4096);
	rom->load_from_file((char *)"data/roms/ext-test/test.bin");
}

CTestDevice::~CTestDevice() {
	delete(rom);
}

uint8_t CTestDevice::read(uint16_t addr, CMemoryDevice *mem) {
	if (addr >= 0x4000 && addr <= 0x4FFF) {
		uint8_t r = rom->read(addr - 0x4000);
		printf("Read from 0x%04X = %02X\n", addr, r);
		return r;
	}
	else return mem->read(addr);
}

void CTestDevice::write(uint16_t addr, uint8_t data, CMemoryDevice *mem) {
	if (addr >= 0x4000 && addr <= 0x5FFF) {
	}
	else mem->write(addr, data);
}
