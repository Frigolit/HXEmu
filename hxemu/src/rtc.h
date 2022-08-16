// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#ifndef __CRTC_H__
#define __CRTC_H__

#include <stdint.h>

#include "memorydevice.h"

class CRTC : public CMemoryDevice {
	public:
		CRTC();
		virtual ~CRTC();

		uint8_t read(uint16_t addr);
		bool write(uint16_t addr, uint8_t data);

	private:
		uint8_t ram[50];
};

#endif

