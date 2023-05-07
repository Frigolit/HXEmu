// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#ifdef FRONTEND_SDL2

#ifndef HXEMU_FRONTENDS_SDL2_LCD_INTERFACE
#define HXEMU_FRONTENDS_SDL2_LCD_INTERFACE

#include <stdint.h>

#include <SDL2/SDL.h>

#include "../../lcd_interface.h"

class Sdl2LcdInterface : public LcdInterface {
	public:
		Sdl2LcdInterface();
		~Sdl2LcdInterface();

		void draw(SDL_Surface *dest, int x, int y);
		bool update();

		virtual void set_pixel(uint8_t x, uint8_t y);
		virtual void clear_pixel(uint8_t x, uint8_t y);

	private:
		SDL_Surface *surface;
		SDL_mutex *mtx;

		uint32_t color_set;
		uint32_t color_clear;

		bool updated;
};

#endif
#endif
