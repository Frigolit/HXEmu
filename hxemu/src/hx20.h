// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#ifndef __HX20_H__
#define __HX20_H__

#include "6301.h"
#include "ram.h"
#include "rom.h"
#include "rtc.h"
#include "lcd.h"
#include "lcdcontroller.h"
#include "fakeslave.h"
#include "iocontroller.h"

class CHX20 {
	public:
		CHX20();
		~CHX20();
		
		C6301 *mcu_master;	// FIXME: This isn't supposed to be public
		
		void think();
		void poweroff();
		void reset();
		
		void draw_lcd(SDL_Surface *dest, int x, int y);
		void keyboard_down(uint8_t c);
		void keyboard_up(uint8_t c);
		
	private:
		uint8_t  keyboard_pressed;
		uint16_t keyboard_repeat;
		
		#ifdef REALSLAVE
			C6301     *mcu_slave;
		#else
			FakeSlave *mcu_slave;
		#endif
		
		CLCD *lcd;
		CLCDController *lcd_ctls[6];
		
		CRTC *rtc;
		
		CRAM *ram0;
		CRAM *ram1;
		
		CIOController *ioctl;
		
		CROM *roms[4];
};

#endif

