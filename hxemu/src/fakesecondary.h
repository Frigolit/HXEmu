// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#ifndef HXEMU_FAKESECONDARY_H
#define HXEMU_FAKESECONDARY_H

#include <stdint.h>

#include "virtualserial.h"

class FakeSecondary {
	public:
		FakeSecondary();
		~FakeSecondary();

		void reset();
		void step();

		VirtualSerial *serial0;
		VirtualSerial *serial1;

	private:
		int state;
};

#endif

