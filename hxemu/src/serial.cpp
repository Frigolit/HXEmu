// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @version $Id: serial.cpp 106 2012-02-11 11:13:30Z frigolit $
// =============================================================================

#include <stdio.h>
#include <stdint.h>

#include "serial.h"

Serial::Serial() {
	endpoint = NULL;
}

Serial::~Serial() {
	if (endpoint != NULL) {
		endpoint->endpoint = NULL;
		endpoint = NULL;
	}
}

void Serial::set_endpoint(Serial *ep) {
	if (ep == NULL) {
		if (endpoint != NULL) endpoint->endpoint = NULL;
		endpoint = NULL;
		return;
	}
	
	if (endpoint == ep) return;
	if (endpoint != NULL) {
		// Disconnect remote endpoint
		endpoint->endpoint = NULL;
	}
	
	// Set new endpoint and connect it to this end
	endpoint = ep;
	endpoint->endpoint = this;
}

