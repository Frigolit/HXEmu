#ifndef __TESTDEVICE_H__
#define __TESTDEVICE_H__

#include "expansiondevice.h"
#include "rom.h"

class CTestDevice : public CExpansionDevice {
	public:
		CTestDevice();
		~CTestDevice();
		uint8_t read(uint16_t addr, CMemoryDevice *mem);
		void write(uint16_t addr, uint8_t data, CMemoryDevice *mem);

	private:
		CROM *rom;
};

#endif
