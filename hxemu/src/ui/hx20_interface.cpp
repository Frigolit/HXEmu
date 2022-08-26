#include "hx20_interface.h"
#include <stdio.h>

CHX20InterfaceWidget::CHX20InterfaceWidget(CHX20 *hx20, int x, int y) {
	::CWidget();

	this->hx20 = hx20;

	CWidget::x = x;
	CWidget::y = y;
	CWidget::w = 256 + 480 + 256;
	CWidget::h = 128 + 400;

	surface = SDL_CreateRGBSurface(SDL_RLEACCEL, w, h, 32, 0, 0, 0, 0);
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0x99, 0x99, 0x99));
}

bool CHX20InterfaceWidget::update() {
	/*
	SDL_Color text_fg;
	SDL_Color text_bg;

	text_fg.r = text_fg.g = text_fg.b = 0;
	text_bg.r = text_bg.g = text_bg.b = 0xC9;

	text_fg.a = text_bg.a = 255;

	uint32_t c0 = SDL_MapRGB(surface->format, 0xDD, 0xDD, 0xDD);
	uint32_t c1 = SDL_MapRGB(surface->format, 0xC9, 0xC9, 0xC9);
	uint32_t c2 = SDL_MapRGB(surface->format, 0xB3, 0xB3, 0xB3);

	SDL_Rect r;

	r.x = 0; r.y = 0; r.w = w; r.h = h;
	SDL_FillRect(surface, &r, c0);

	r.x++;
	r.y++;
	SDL_FillRect(surface, &r, c2);

	r.w--;
	r.h--;
	SDL_FillRect(surface, &r, c1);

	// Draw label
	SDL_Surface *surf_text = TTF_RenderText_Shaded(font_buttons, caption, text_fg, text_bg);

	SDL_Rect rect_text;
	rect_text.w = surf_text->w;
	rect_text.h = surf_text->h;
	rect_text.x = (int)(((float)w / 2.0) - ((float)rect_text.w / 2.0));
	rect_text.y = (int)(((float)h / 2.0) - ((float)rect_text.h / 2.0));

	SDL_BlitSurface(surf_text, NULL, surface, &rect_text);
	SDL_FreeSurface(surf_text);

	updated = true;
	*/
	return true;
}

void CHX20InterfaceWidget::draw(SDL_Surface *dest) {
	hx20->lcd->draw(surface, 256, 0);
	//controls->render(dest, x + 480, y);

	CWidget::draw(dest);
}

