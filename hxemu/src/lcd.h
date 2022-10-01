// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#ifndef __LCD_H__
#define __LCD_H__

#include <stdint.h>

#ifdef FRONTEND_SDL2
#include <SDL2/SDL.h>
#endif

class CLCD {
	public:
		CLCD();
		~CLCD();

		void command(uint8_t c, uint8_t n);
		void data(uint8_t c, uint8_t n);

		#ifdef FRONTEND_SDL2
		void draw(SDL_Surface *dest, int x, int y);
		#endif

		#ifdef FRONTEND_CLI
		void draw(void);
		#endif

		void set_pixel(uint8_t x, uint8_t y);
		void clear_pixel(uint8_t x, uint8_t y);

	private:
		#ifdef FRONTEND_SDL2
		SDL_Surface *surface;
		SDL_mutex *mtx;

		uint32_t color_set;
		uint32_t color_clear;
		#endif
};

#endif

