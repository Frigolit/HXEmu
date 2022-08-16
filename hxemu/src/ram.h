// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#ifndef __RAM_H__
#define __RAM_H__

#include <stdint.h>

#include "memorydevice.h"
#include "stateful.h"

class CRAM : public CMemoryDevice, public CStateful {
	public:
		CRAM(uint16_t size);
		virtual ~CRAM();

		uint8_t read(uint16_t addr);
		bool write(uint16_t addr, uint8_t data);

	private:
		uint16_t size;
		uint8_t *ram;
};

#endif

