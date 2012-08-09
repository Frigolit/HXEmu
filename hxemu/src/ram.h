// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @version $Id: ram.h 166 2012-04-05 10:06:00Z frigolit $
// =============================================================================

#ifndef __RAM_H__
#define __RAM_H__

#include <stdint.h>

#include "memorydevice.h"

class CRAM : public CMemoryDevice {
	public:
		CRAM(uint16_t size);
		~CRAM();
		
		uint8_t read(uint16_t addr);
		void    write(uint16_t addr, uint8_t data);
		
	private:
		uint16_t size;
		uint8_t  *ram;
};

#endif

