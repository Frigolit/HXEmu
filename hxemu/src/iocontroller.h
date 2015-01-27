// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#ifndef __IOCONTROLLER_H__
#define __IOCONTROLLER_H__

#include <stdint.h>

#include "memorydevice.h"
#include "lcdcontroller.h"

class CIOController: public CMemoryDevice {
	public:
		CIOController();
		virtual ~CIOController();

		void    reset();

		uint8_t read(uint16_t addr);
		void    write(uint16_t addr, uint8_t data);

		void    set_lcd_controller(uint8_t n, CLCDController *c);

		void    set_power_button(bool p);
		bool    get_power_button();

		char    keyboard_map[256];

		/**
		 * LCD chip select
		 * Keyboard interrupt mask
		 * Cartridge control
		 * and something else...
		 */
		uint8_t r_9g;

	private:
		bool b_power;

		CLCDController *lcd_ctls[6];

		uint8_t lcd_clk_counter;
		uint8_t ram[64];

		#ifdef DEBUG_IOCTL
		uint8_t _read(uint16_t addr);
		#endif
};

#endif

