#ifndef __EXPANSIONDEVICE_H__
#define __EXPANSIONDEVICE_H__

#include <stdint.h>
#include "memorydevice.h"

class CExpansionDevice {
	public:
		virtual ~CExpansionDevice() { }
		virtual uint8_t read(uint16_t addr, CMemoryDevice *mem) { return mem->read(addr); }
		virtual void write(uint16_t addr, uint8_t data, CMemoryDevice *mem) { mem->write(addr, data); }
};

#endif

