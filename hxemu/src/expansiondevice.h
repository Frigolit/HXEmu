#ifndef __EXPANSIONDEVICE_H__
#define __EXPANSIONDEVICE_H__

#include <stdint.h>
#include "memorydevice.h"

class CExpansionDevice {
	public:
		uint8_t read(uint16_t addr, CMemoryDevice *mem);
		void write(uint16_t addr, uint8_t data, CMemoryDevice *mem);

	private:
		char ram[8192];
};

#endif

