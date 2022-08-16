// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#ifndef __EXPANSION_H__
#define __EXPANSION_H__

#include "memorydevice.h"
#include "expansiondevice.h"

class CExpansion : public CMemoryDevice {
	public:
		CExpansion(CMemoryDevice *mem);
		virtual ~CExpansion();

		virtual uint8_t read(uint16_t addr);
		virtual bool write(uint16_t addr, uint8_t data);

		void attach_device(CExpansionDevice *dev);
		CExpansionDevice * detach_device();

	private:
		CMemoryDevice *memory;
		CExpansionDevice *expdevice;
};

#endif

