// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "fakesecondary.h"

FakeSecondary::FakeSecondary() {
	serial0 = new VirtualSerial();
	serial1 = new VirtualSerial();
}

FakeSecondary::~FakeSecondary() {
	delete(serial0);
	delete(serial1);
}

void FakeSecondary::reset() {
	state = 0;
	serial0->reset();
	serial1->reset();
}

void FakeSecondary::step() {
	if (!serial0->peek()) return;

	uint8_t c = serial0->recv();

	printf("FakeSecondary::think(): debug: received command %02X\n", c);

	if (state == 0) {
		if (c == 0x00) {
			// Secondary MCU ready check
			serial0->send(0x01);		// ACK
		}
		else if (c == 0x02) {
			// Initialize
			serial0->send(0x01);		// ACK
		}
		else if (c == 0x03) {
			// Open special command mask
			serial0->send(0x01);		// ACK
		}
		else if (c == 0x04) {
			// Close special command mask
			serial0->send(0x01);		// ACK
		}
		else if (c == 0x0C) {
			// Stop processing and enter WAIT state
			serial0->send(0x02);		// ACK
		}
		else if (c == 0x0D) {
			// Cut off power supply
			serial0->send(0x01);
			state = 1;
		}

		else if (c == 0x50) {
			// Detect plug-in options
			printf("FakeSecondary::think(): fixme: plugin options are not yet supported\n");
			serial0->send(0x00);
		}

		// Unknown command
		else {
			printf("FakeSecondary::think(): fixme: unhandled command %02X\n", c);
		}
	}
	else if (state == 1) {
		state = 0;

		if (c == 0xAA) {
			printf("FakeSecondary::think(): debug: power supply cut-off requested\n");
			serial0->send(0x01);
		}
	}
}

