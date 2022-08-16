// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#ifndef __FAKESECONDARY_H__
#define __FAKESECONDARY_H__

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

