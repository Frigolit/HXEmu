// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @version $Id: fakeslave.cpp 138 2012-02-20 13:35:19Z frigolit $
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "fakeslave.h"

FakeSlave::FakeSlave() {
	serial_master = new VirtualSerial();
	serial_rs232 = new VirtualSerial();
}

FakeSlave::~FakeSlave() {
	delete(serial_master);
	delete(serial_rs232);
}

void FakeSlave::reset() {
	state = 0;
	serial_master->reset();
	serial_rs232->reset();
}

void FakeSlave::think() {
	if (!serial_master->peek()) return;
	
	uint8_t c = serial_master->recv();
	
	printf("FakeSlave::think(): debug: received command %02X\n", c);
	
	if (state == 0) {
		if (c == 0x00) {
			// Slave MCU ready check
			serial_master->send(0x01);		// ACK
		}
		else if (c == 0x02) {
			// Initialize
			serial_master->send(0x01);		// ACK
		}
		else if (c == 0x03) {
			// Open special command mask
			serial_master->send(0x01);		// ACK
		}
		else if (c == 0x04) {
			// Close special command mask
			serial_master->send(0x01);		// ACK
		}
		else if (c == 0x0C) {
			// Stop processing and enter WAIT state
			serial_master->send(0x02);		// ACK
		}
		else if (c == 0x0D) {
			// Cut off power supply
			serial_master->send(0x01);
			state = 1;
		}
		
		else if (c == 0x50) {
			// Detect plug-in options
			printf("FakeSlave::think(): fixme: plugin options are not yet supported\n");
			serial_master->send(0x00);
		}
		
		// Unknown command
		else {
			printf("FakeSlave::think(): fixme: unhandled command %02X\n", c);
		}
	}
	else if (state == 1) {
		state = 0;
		
		if (c == 0xAA) {
			printf("FakeSlave::think(): debug: power supply cut-off requested\n");
			serial_master->send(0x01);
		}
	}
}

