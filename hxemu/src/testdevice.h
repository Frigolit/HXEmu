// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @version $Id: testdevice.h 151 2012-02-29 09:37:57Z frigolit $
// =============================================================================

#ifndef __TESTDEVICE_H__
#define __TESTDEVICE_H__

#ifndef NOTESTDEVICE

#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "memorydevice.h"
#include "rom.h"
#include "ram.h"

class CTestDevice : public CMemoryDevice {
	public:
		CTestDevice();
		~CTestDevice();
		
		bool connect_to_host();
		
		uint8_t read(uint16_t addr);
		void    write(uint16_t addr, uint8_t data);
		
	private:
		CROM *rom;
		CRAM *testrom;
		
		bool romok;
		
		int sock;
};

#endif

#endif

