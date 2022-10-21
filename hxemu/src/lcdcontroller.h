// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#ifndef HXEMU_LCDCONTROLLER_H
#define HXEMU_LCDCONTROLLER_H

#include <stdint.h>

#include "lcd_interface.h"

class CLCDController {
	public:
		CLCDController(uint8_t x, uint8_t y);
		~CLCDController();

		void reset();

		void set_lcd(LcdInterface *);

		void data(uint8_t n);
		void command(uint8_t n);

	private:
		LcdInterface *lcd;
		uint8_t lcd_x;
		uint8_t lcd_y;

		uint8_t *ram;
		uint16_t ptr;
		uint8_t ptrop;
		uint8_t mode;

		void apply_ptr_op(uint8_t op);
};

#endif

