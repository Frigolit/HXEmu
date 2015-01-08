// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#ifndef __MEMORYBUS_H__
#define __MEMORYBUS_H__

#include <stdint.h>
#include <vector>

#include "memorydevice.h"

class CMemoryBus: public CMemoryDevice {
	public:
		CMemoryBus();
		virtual ~CMemoryBus();
		
		uint8_t read(uint16_t addr);
		void    write(uint16_t addr, uint8_t data);
		
		void    add(CMemoryDevice *dev, uint16_t start, uint16_t size, uint16_t offset);
		
		void    set_fallback(CMemoryDevice *dev);
		
	private:
		class CMemoryBusEntry {
			public:
				CMemoryDevice *p_device;
				uint16_t i_start;
				uint16_t i_size;
				uint16_t i_offset;
		};
		
		CMemoryDevice *p_fallback;
		
		std::vector<CMemoryBusEntry*> *entries;
};

#endif

