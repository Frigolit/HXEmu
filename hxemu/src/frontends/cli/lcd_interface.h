#ifdef FRONTEND_CLI

#ifndef __FRONTENDS_CLI_LCD_INTERFACE_H__
#define __FRONTENDS_CLI_LCD_INTERFACE_H__

#include <mutex>

#include "../../lcd_interface.h"

class CliLcdInterface : public LcdInterface {
	public:
		CliLcdInterface();
		~CliLcdInterface();

		virtual void set_pixel(uint8_t x, uint8_t y);
		virtual void clear_pixel(uint8_t x, uint8_t y);

		void update();
		void redraw();

	private:
		uint32_t pixels[120];
		uint16_t changes[60];
		std::mutex *mtx;
};

#endif
#endif
