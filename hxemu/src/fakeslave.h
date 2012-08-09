// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @version $Id: fakeslave.h 127 2012-02-15 16:32:01Z frigolit $
// =============================================================================

#ifndef __FAKESLAVE_H__
#define __FAKESLAVE_H__

#include <stdint.h>

#include "virtualserial.h"

class FakeSlave {
	public:
		FakeSlave();
		~FakeSlave();
		
		void reset();
		void think();
		
		VirtualSerial *serial_master;
		VirtualSerial *serial_rs232;
		
	private:
		int state;
};

#endif

