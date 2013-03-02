// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @version $Id: rtc.h 94 2011-06-27 21:42:57Z frigolit $
// =============================================================================

#ifndef __CRTC_H__
#define __CRTC_H__

#include <stdint.h>

#include "memorydevice.h"

class CRTC : public CMemoryDevice {
	public:
		CRTC();
		~CRTC();
		
		uint8_t read(uint16_t addr);
		void write(uint16_t addr, uint8_t data);
		
	private:
		uint8_t memory[50];
};

#endif

