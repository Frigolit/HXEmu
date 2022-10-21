// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#ifndef HXEMU_MEMORYDEVICE_H
#define HXEMU_MEMORYDEVICE_H

#include <stdint.h>

class CMemoryDevice {
	public:
		virtual uint8_t read(uint16_t addr) { return 0x00; }
		virtual bool write(uint16_t addr, uint8_t data) { return false; }
};

#endif

