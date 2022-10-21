// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#ifndef HXEMU_VIRTUALSERIAL_H
#define HXEMU_VIRTUALSERIAL_H

#include <stdint.h>

#include "serial.h"

class VirtualSerial : public Serial {
	public:
		VirtualSerial();
		virtual ~VirtualSerial();

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

