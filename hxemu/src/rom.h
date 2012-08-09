// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @version $Id: rom.h 166 2012-04-05 10:06:00Z frigolit $
// =============================================================================

#ifndef __ROM_H__
#define __ROM_H__

#include <stdint.h>

#include "memorydevice.h"

class CROM : public CMemoryDevice {
	public:
		CROM(uint16_t size);
		~CROM();
		
		uint8_t read(uint16_t addr);
		void    write(uint16_t addr, uint8_t data);
		
		void    load(char *src, int offset, int size);
		bool    load_from_file(char *fn);
		
	private:
		uint16_t size;
		uint8_t  *rom;
		
		bool allocate();
};

#endif

