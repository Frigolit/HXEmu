// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#ifndef HXEMU_MEMORYBUS_H
#define HXEMU_MEMORYBUS_H

#include <stdint.h>
#include <vector>

#include "memorydevice.h"

class CMemoryBus: public CMemoryDevice {
	public:
		CMemoryBus();
		virtual ~CMemoryBus();

		uint8_t read(uint16_t addr);
		bool write(uint16_t addr, uint8_t data);

		void add(CMemoryDevice *dev, uint16_t start, uint16_t size, const char *name);

		void set_fallback(CMemoryDevice *dev);

	private:
		class CMemoryBusEntry {
			public:
				CMemoryDevice *p_device;
				uint16_t i_start;
				uint16_t i_size;
				char *name;
		};

		CMemoryDevice *p_fallback;

		std::vector<CMemoryBusEntry*> *entries;
};

#endif

