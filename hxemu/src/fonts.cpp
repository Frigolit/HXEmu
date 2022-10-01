#include "fonts.h"

#ifdef FRONTEND_SDL2
TTF_Font *font_buttons;
#endif

void fonts_init() {
	#ifdef FRONTEND_SDL2
	font_buttons = TTF_OpenFont("data/fonts/LiberationSans-Regular.ttf", 16);
	#endif
}

void fonts_quit() {
	#ifdef FRONTEND_SDL2
	TTF_CloseFont(font_buttons);
	#endif
}

