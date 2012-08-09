// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @version $Id: hx20.cpp 167 2012-06-05 15:29:03Z frigolit $
// =============================================================================

using namespace std;

#include <stdio.h>

#include "6301.h"
#include "hx20.h"
#include "hash.h"
#include "rtc.h"
#include "iocontroller.h"
#include "lcd.h"
#include "lcdcontroller.h"

CHX20::CHX20() {
	// Initialize CPUs
	mcu_master = new C6301(4);
	mcu_slave = new FakeSlave();
	
	// Link serial endpoints
	mcu_master->serial0->set_endpoint(mcu_slave->serial_master);
	
	// Initialize keyboard
	keyboard_pressed = 0;
	keyboard_repeat = 0;
	
	// Initialize RTC
	rtc = new CRTC();
	
	// Initialize RAM
	ram0 = new CRAM(16128);
	ram1 = new CRAM(128);
	
	// Initialize I/O select
	ioctl = new CIOController();
	
	// Initialize ROMs
	for (int i = 0; i < 5; i++) {
		roms[i] = new CROM(8192);
	}
	
	// Initialize test device
	#ifndef NOTESTDEVICE
		testdev = new CTestDevice();
		if (!testdev->connect_to_host()) {
			delete testdev;
			testdev = NULL;
		}
	#endif
	
	// Initialize LCD and controllers
	lcd = new CLCD();
	for (int i = 0; i < 6; i++) {
		lcd_ctls[i] = new CLCDController();
		lcd_ctls[i]->set_lcd(lcd, (i % 3) * 40, (i / 3) * 16);
	}
	
	// Add master MCU memory devices
	mcu_master->membus->add(ioctl,    0x0020, 32,    0x0020);
	mcu_master->membus->add(rtc,      0x0040, 64,    0x0000);
	mcu_master->membus->add(ram1,     0x0080, 128,   0x0000);
	mcu_master->membus->add(ram0,     0x0100, 16128, 0x0000);
	mcu_master->membus->add(roms[4],  0x6000, 8192,  0x0000);
	
	#ifndef NOTESTDEVICE
		if (testdev != NULL) {
			// Attach test device to the upper 32kB (hooked via expansion port on the real hardware)
			mcu_master->membus->add(testdev, 0x8000, 32768, 0x0000);
		}
		else {
			// Attach ROMs
			mcu_master->membus->add(roms[3],  0x8000, 8192,  0x0000);
			mcu_master->membus->add(roms[2],  0xA000, 8192,  0x0000);
			mcu_master->membus->add(roms[1],  0xC000, 8192,  0x0000);
			mcu_master->membus->add(roms[0],  0xE000, 8192,  0x0000);
		}
	#else
		mcu_master->membus->add(roms[3],  0x8000, 8192,  0x0000);
		mcu_master->membus->add(roms[2],  0xA000, 8192,  0x0000);
		mcu_master->membus->add(roms[1],  0xC000, 8192,  0x0000);
		mcu_master->membus->add(roms[0],  0xE000, 8192,  0x0000);
	#endif
	
	// Attach hardware to I/O controller
	for (int i = 0; i < 6; i++) {
		ioctl->set_lcd_controller(i, lcd_ctls[i]);
	}
	
	// Load ROMs
	roms[0]->load_from_file((char *)"data/roms/firmware/1.1/rom0.bin");
	roms[1]->load_from_file((char *)"data/roms/firmware/1.1/rom1.bin");
	roms[2]->load_from_file((char *)"data/roms/firmware/1.1/rom2.bin");
	roms[3]->load_from_file((char *)"data/roms/firmware/1.1/rom3.bin");
	
	// Checksum ROMs
	CHash *hash = new CHash();
	uint8_t buf[8192];
	for (int i = 0; i < 4; i++) {
		for (int n = 0; n < 8192; n++) {
			buf[n] = roms[i]->read(n);
		}
		printf("ROM #%d - Checksum: %08X\n", i, hash->crc32(buf, 8192));
	}
	delete hash;
	
	// Reset
	reset();
}

Serial* CHX20::get_rs232() {
	return mcu_master->serial1;
}

void CHX20::keyboard_down(uint8_t c) {
	keyboard_pressed++;
	ioctl->keyboard_map[c] = 1;
	mcu_master->set_port1(0b01011000);
}

void CHX20::keyboard_up(uint8_t c) {
	if (--keyboard_pressed) {
		keyboard_repeat = 0;
		mcu_master->set_port1(0b01111000);
	}
	ioctl->keyboard_map[c] = 0;
}

CHX20::~CHX20() {
	delete(mcu_master);
	delete(mcu_slave);
	
	delete(ioctl);
	
	delete(ram0);
	delete(ram1);
	
	for (int i = 0; i < 5; i++) delete(roms[i]);
}

void CHX20::reset() {
	mcu_master->reset();
	mcu_slave->reset();
	ioctl->reset();
}

void CHX20::think() {
	mcu_master->step();
	mcu_slave->think();
	
	if (keyboard_pressed && !keyboard_repeat++) {
		mcu_master->interrupt();
	}
}

void CHX20::draw_lcd(SDL_Surface *dest, int x, int y) {
	lcd->draw(dest, x, y);
}

