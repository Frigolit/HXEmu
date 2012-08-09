// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @version $Id: testdevice.cpp 151 2012-02-29 09:37:57Z frigolit $
// =============================================================================

#ifndef NOTESTDEVICE

#include <stdio.h>
#include <unistd.h>

#include "version.h"
#include "testdevice.h"

/**
 * === Memory map ===
 * If active, the testing device takes over the address space 0x8000-0xFFFF (32 KiB).
 * 
 * E000-FFFF    ROM (data/roms/test/external.bin)
 * C000-DFFF    ROM (testing host uploads this)
 * A003-BFFF    --- Unused ---
 * A002         Serial output register
 * A001         Control register
 * A000         Status register
 * 8000-CFFF    --- Unused ---
 *
 * === A000 - Status register ===
 * Bit 0: Test ready
 *        This tells the program that there's a test ready.
 *        The program should branch to C000.
 * 
 * === A001 - Control register ===
 * Bit 0: Ready
 *        Tell the device that the program is running!
 * 
 * Bit 1: Test starting
 *        This instructs the device that the program is about to start the test.
 *        The device should clear bit 0 in the status register when this is written.
 * 
 * Bit 2: Test complete
 *        This instructs the device that the test has finished.
 * 
 * Bit 3: TRAP
 *        This instructs the device that a TRAP has triggered.
 *        Set this bit and write the contents of A, B, X, SP, CCR the registers from the stack to A002.
 *        Push zeroes for A, B, X and CCR and E000 for SP onto the stack and return from the interrupt.
 */

CTestDevice::CTestDevice() {
	rom = new CROM(8192);
	testrom = new CRAM(8192);
	
	romok = false;
	
	if (rom->load_from_file((char *)"data/roms/test/external.bin")) {
		romok = true;
	}
}

CTestDevice::~CTestDevice() {
	delete(rom);
	delete(testrom);
}

bool CTestDevice::connect_to_host() {
	// If we couldn't load the ROM, don't even bother connecting
	if (!romok) return false;
	
	// Initialize socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) return false;
	
	// Connect to 127.0.0.1:31735
	struct sockaddr_in addr_server;
	addr_server.sin_family = AF_INET;
	addr_server.sin_addr.s_addr = htonl(0x7F000001);	// 127.0.0.1
	addr_server.sin_port = htons(31735);
	
	if (connect(sock, (struct sockaddr *)&addr_server, sizeof(addr_server)) < 0) return false;
	
	// Say hello to the host and tell it what version we're running
	char buf[128];
	int r = sprintf(buf, "HXEmu%c%c%c", APP_MAJOR, APP_MINOR, APP_REVISION);
	r = ::write(sock, buf, r);
	
	r = ::read(sock, buf, 1);
	if (r < 1 || buf[0] != 1) return false;
	
	// Everything's just fine~
	return true;
}

uint8_t CTestDevice::read(uint16_t addr) {
	if (addr >= 0xE000) return rom->read(addr - 0xE000);
	else if (addr >= 0xC000) return testrom->read(addr - 0xC000);
	else return 0xFF;
}

void CTestDevice::write(uint16_t addr, uint8_t data) {
}

#endif

