#ifndef __FONTS_H__
#define __FONTS_H__

#ifdef FRONTEND_SDL2
#include <SDL2/SDL_ttf.h>

extern TTF_Font *font_buttons;
#endif

void fonts_init(void);
void fonts_quit(void);

#endif

