#include "fonts.h"

TTF_Font *font_buttons;

void fonts_init() {
	font_buttons = TTF_OpenFont("/usr/share/fonts/ttf-bitstream-vera/Vera.ttf", 10);
}

void fonts_quit() {
	TTF_CloseFont(font_buttons);
}

