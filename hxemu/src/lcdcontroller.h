// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#ifndef __LCDCONTROLLER_H__
#define __LCDCONTROLLER_H__

#include <stdint.h>

#include "lcd.h"

class CLCDController {
	public:
		CLCDController();
		~CLCDController();

		void reset();

		void set_lcd(CLCD *l, uint8_t x, uint8_t y);

		void data(uint8_t n);
		void command(uint8_t n);

	private:
		CLCD *lcd;
		uint8_t lcd_x;
		uint8_t lcd_y;

		uint8_t *ram;
		uint16_t ptr;
		uint8_t ptrop;
		uint8_t mode;

		void apply_ptr_op(uint8_t op);
};

#endif

