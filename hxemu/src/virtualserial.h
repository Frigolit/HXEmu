// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#ifndef __VIRTUALSERIAL_H__
#define __VIRTUALSERIAL_H__

#include <stdint.h>

#include "serial.h"

class VirtualSerial : public Serial {
	public:
		VirtualSerial();
		~VirtualSerial();
		
		void    reset();
		void    send(uint8_t c);
		void    _recv(uint8_t c);
		uint8_t recv();
		uint8_t peek();
		
	private:
		uint8_t  *buf;
		uint16_t buflen;
		
		void     buffer_append(uint8_t c);
		uint8_t  buffer_pop();
};

#endif

