#include "expansiondevice.h"

uint8_t CExpansionDevice::read(uint16_t addr, CMemoryDevice *mem) {
	if (addr >= 0x4000 && addr <= 0x5FFF) {
		return ram[addr - 0x4000];
	}
	else return mem->read(addr);
}

void CExpansionDevice::write(uint16_t addr, uint8_t data, CMemoryDevice *mem) {
	if (addr >= 0x4000 && addr <= 0x5FFF) {
		ram[addr - 0x4000] = data;
	}
	else mem->write(addr, data);
}

