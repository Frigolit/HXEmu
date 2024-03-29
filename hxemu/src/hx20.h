// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#ifndef HXEMU_HX20_H
#define HXEMU_HX20_H

#include "6301.h"
#include "ram.h"
#include "rom.h"
#include "rtc.h"
#include "lcd_interface.h"
#include "lcdcontroller.h"
#include "fakesecondary.h"
#include "iocontroller.h"
#include "expansion.h"

class CHX20 {
	public:
		CHX20();
		~CHX20();

		C6301 *mcu_master;	// FIXME: This isn't supposed to be public
		CIOController *ioctl;

		void think();
		void poweroff();
		void reset();

		void set_lcd_interface(LcdInterface *);

		void load_roms(char *dirname);
		void load_option_rom(char *path);

		void keyboard_down(uint8_t c);
		void keyboard_up(uint8_t c);

	private:
		uint8_t keyboard_pressed;
		uint16_t keyboard_repeat;

		#ifdef REALSECONDARY
			C6301 *mcu_secondary;
		#else
			FakeSecondary *mcu_secondary;
		#endif

		CLCDController *lcd_ctls[6];

		CRTC *rtc;

		CRAM *ram0;
		CRAM *ram1;

		CROM *roms[4];
		CROM *optionrom;

		CMemoryBus *membus;
		CExpansion *expansion;
};

#endif

