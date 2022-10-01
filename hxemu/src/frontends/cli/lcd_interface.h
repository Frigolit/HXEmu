#ifdef FRONTEND_CLI

#ifndef __FRONTENDS_CLI_LCD_INTERFACE_H__
#define __FRONTENDS_CLI_LCD_INTERFACE_H__

#include "../../lcd_interface.h"

class CliLcdInterface : public LcdInterface {
	public:
		CliLcdInterface();

		virtual void set_pixel(uint8_t x, uint8_t y);
		virtual void clear_pixel(uint8_t x, uint8_t y);

		void update();
		void redraw();

	private:
		uint8_t pixels[480];
		uint8_t changes[120];
};

#endif
#endif
