// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @version $Id: memorybus.cpp 94 2011-06-27 21:42:57Z frigolit $
// =============================================================================

#include <stdint.h>
#include <stdio.h>

#include "memorybus.h"

CMemoryBus::CMemoryBus() {
	entries = new vector<CMemoryBusEntry *>();
	p_fallback = NULL;
}

CMemoryBus::~CMemoryBus() {
	delete(entries);
}

void CMemoryBus::add(CMemoryDevice *dev, uint16_t start, uint16_t size, uint16_t offset) {
	CMemoryBusEntry *ent = new CMemoryBusEntry();
	ent->p_device = dev;
	ent->i_start = start;
	ent->i_size = size;
	ent->i_offset = offset;
	
	entries->push_back(ent);
}

uint8_t CMemoryBus::read(uint16_t addr) {
	//printf("CMemoryBus::read(0x%04X)\n", addr);
	
	int sz = entries->size();
	
	for (int i = 0; i < sz; i++) {
		CMemoryBusEntry *e = entries->at(i);
		if (addr >= e->i_start && addr < e->i_start + e->i_size) {
			return e->p_device->read(addr - e->i_start + e->i_offset);
		}
	}
	
	if (p_fallback == NULL) {
		//printf("CMemoryBus::read(): debug: unhandled address 0x%04X\n", addr);
		return 0xFF;
	}
	else return p_fallback->read(addr);
}

void CMemoryBus::write(uint16_t addr, uint8_t data) {
	//printf("CMemoryBus::write(0x%04X, %02X)\n", addr, data);
	
	int sz = entries->size();
	
	for (int i = 0; i < sz; i++) {
		CMemoryBusEntry *e = entries->at(i);
		if (addr >= e->i_start && addr < e->i_start + e->i_size) {
			e->p_device->write(addr - e->i_start + e->i_offset, data);
			return;
		}
	}
	
	if (p_fallback != NULL) p_fallback->write(addr, data);
	else {
		//printf("CMemoryBus::write(): debug: unhandled address 0x%04X\n", addr);
	}
}

