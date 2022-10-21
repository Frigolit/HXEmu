#ifndef HXEMU_LCD_INTERFACE_H
#define HXEMU_LCD_INTERFACE_H

#include <cstdint>

class LcdInterface {
	public:
		virtual void set_pixel(uint8_t x, uint8_t y) { }
		virtual void clear_pixel(uint8_t x, uint8_t y) { }
};

#endif
