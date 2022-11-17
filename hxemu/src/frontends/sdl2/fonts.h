#ifndef HXEMU_FONTS_H
#define HXEMU_FONTS_H

#ifdef FRONTEND_SDL2
#include <SDL2/SDL_ttf.h>

extern TTF_Font *font_buttons;
extern TTF_Font *font_dialog_text;
#endif

void fonts_init(void);
void fonts_quit(void);

#endif

