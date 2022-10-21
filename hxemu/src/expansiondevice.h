#ifndef HXEMU_EXPANSIONDEVICE_H
#define HXEMU_EXPANSIONDEVICE_H

#include <stdint.h>
#include "memorydevice.h"

class CExpansionDevice {
	public:
		virtual ~CExpansionDevice() { }
		virtual uint8_t read(uint16_t addr, CMemoryDevice *mem) { return mem->read(addr); }
		virtual bool write(uint16_t addr, uint8_t data, CMemoryDevice *mem) { return mem->write(addr, data); }
};

#endif

