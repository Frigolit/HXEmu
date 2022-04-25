#include "fonts.h"
#include <stdio.h>
#include <unistd.h>

TTF_Font *font_buttons;

void fonts_init() {
	font_buttons = TTF_OpenFont("Vera.ttf", 10);
	if (!font_buttons) {
	    printf("The font was not loaded: %s\n", TTF_GetError());
	} else {
	    printf("The font was loaded\n");
	}
}

void fonts_close() {
	TTF_CloseFont(font_buttons);
}

