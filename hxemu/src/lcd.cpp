// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#include <stdio.h>
#include <stdlib.h>

#include "lcd.h"

CLCD::CLCD() {
	#ifdef FRONTEND_SDL2
	surface = SDL_CreateRGBSurface(SDL_RLEACCEL, 120 * 4, 32 * 4, 32, 0, 0, 0, 0);
	mtx = SDL_CreateMutex();

	color_set = SDL_MapRGB(surface->format, 0x12, 0x15, 0x13);
	color_clear = SDL_MapRGB(surface->format, 0x76, 0x84, 0x7d);

	SDL_Rect r;
	r.x = 0;
	r.y = 0;
	r.w = 120 * 4;
	r.h = 32 * 4;

	SDL_FillRect(surface, &r, color_set);
	#endif
}

CLCD::~CLCD() {
	#ifdef FRONTEND_SDL2
	SDL_FreeSurface(surface);
	SDL_DestroyMutex(mtx);
	#endif
}

#ifdef FRONTEND_SDL2
void CLCD::draw(SDL_Surface *dest, int x, int y) {
	SDL_Rect dst;

	dst.x = x;
	dst.y = y;
	dst.w = 120 * 4;
	dst.h = 32 * 4;

	SDL_mutexP(mtx);
	SDL_BlitSurface(surface, NULL, dest, &dst);
	SDL_mutexV(mtx);
}

void CLCD::set_pixel(uint8_t x, uint8_t y) {
	SDL_Rect r;
	r.x = x * 4;
	r.y = y * 4;
	r.w = 4;
	r.h = 4;

	SDL_mutexP(mtx);
	SDL_FillRect(surface, &r, color_set);
	SDL_mutexV(mtx);
}

void CLCD::clear_pixel(uint8_t x, uint8_t y) {
	SDL_Rect r;
	r.x = x * 4;
	r.y = y * 4;
	r.w = 4;
	r.h = 4;

	SDL_mutexP(mtx);
	SDL_FillRect(surface, &r, color_clear);
	SDL_mutexV(mtx);
}
#endif

#ifdef FRONTEND_CLI
void CLCD::draw() {
	// TODO
}

void CLCD::set_pixel(uint8_t x, uint8_t y) {
	// TODO
}

void CLCD::clear_pixel(uint8_t x, uint8_t y) {
	// TODO
}
#endif
