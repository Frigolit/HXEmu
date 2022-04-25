// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#ifndef __ROM_H__
#define __ROM_H__

#include <stdint.h>

#include "memorydevice.h"

class CROM : public CMemoryDevice {
	public:
		CROM(uint16_t size);
		virtual ~CROM();

		uint8_t read(uint16_t addr);
		void    write(uint16_t addr, uint8_t data);

		void    load(char *src, int offset, int size);
		bool    load_from_file(const char *fn);
		void    eject();

	private:
		uint16_t size;
		uint8_t  *rom;

		bool allocate();
};

#endif

