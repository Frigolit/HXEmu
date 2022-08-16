// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#include "expansion.h"

CExpansion::CExpansion(CMemoryDevice *mem) {
	memory = mem;
}

CExpansion::~CExpansion() {
	memory = NULL;
	expdevice = NULL;
}

uint8_t CExpansion::read(uint16_t addr) {
	if (expdevice != NULL) return expdevice->read(addr, memory);
	else return memory->read(addr);
}

bool CExpansion::write(uint16_t addr, uint8_t data) {
	if (expdevice != NULL) {
		return expdevice->write(addr, data, memory);
	}
	else {
		return memory->write(addr, data);
	}
}

void CExpansion::attach_device(CExpansionDevice *dev) {
	expdevice = dev;
}

CExpansionDevice * CExpansion::detach_device() {
	CExpansionDevice *r = expdevice;
	expdevice = NULL;
	return r;
}

