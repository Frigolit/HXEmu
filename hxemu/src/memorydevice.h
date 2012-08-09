// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @version $Id: memorydevice.h 94 2011-06-27 21:42:57Z frigolit $
// =============================================================================

using namespace std;

#ifndef __MEMORYDEVICE_H__
#define __MEMORYDEVICE_H__

#include <stdint.h>

class CMemoryDevice {
	public:
		virtual uint8_t read(uint16_t addr) { return 0x00; }
		virtual void write(uint16_t addr, uint8_t data) { }
};

#endif

