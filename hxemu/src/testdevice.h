#ifndef HXEMU_TESTDEVICE_H
#define HXEMU_TESTDEVICE_H

#include "expansiondevice.h"
#include "rom.h"

class CTestDevice : public CExpansionDevice {
	public:
		CTestDevice();
		~CTestDevice();

		uint8_t read(uint16_t addr, CMemoryDevice *mem);
		bool write(uint16_t addr, uint8_t data, CMemoryDevice *mem);

	private:
		int state;
		int test_no;

		CROM *rom;
		uint8_t *dynrom;
};

#endif
