#include "globals.h"

#include "fonts.h"

#ifdef FRONTEND_SDL2
TTF_Font *font_buttons;
TTF_Font *font_dialog_text;
#endif

void fonts_init() {
	#ifdef FRONTEND_SDL2
	char fontpath[512];
	get_data_path(fontpath, "fonts/LiberationSans-Regular.ttf", 512);

	font_buttons = TTF_OpenFont(fontpath, 16);
	font_dialog_text = TTF_OpenFont(fontpath, 12);
	#endif
}

void fonts_quit() {
	#ifdef FRONTEND_SDL2
	TTF_CloseFont(font_buttons);
	TTF_CloseFont(font_dialog_text);
	#endif
}

