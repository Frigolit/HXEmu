#ifdef FRONTEND_SDL2

#include "../../../globals.h"
#include "../../../fonts.h"
#include "../../../version.h"

#include "about.h"

AboutDialog::AboutDialog(CHX20 *hx20, int x, int y) {
	::CWidget();

	this->hx20 = hx20;

	CWidget::x = x;
	CWidget::y = y;
	CWidget::w = 992;
	CWidget::h = 528;

	surface = SDL_CreateRGBSurface(SDL_RLEACCEL, w, h, 32, 0, 0, 0, 0);
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xB0, 0xB0, 0xB0));

	SDL_Rect bg_rect;
	bg_rect.x = 0;
	bg_rect.y = 0;
	bg_rect.w = w;
	bg_rect.h = h;

	// Draw texts
	char txt_title[64];
	sprintf(txt_title, "HXEmu %d.%d.%d", APP_MAJOR, APP_MINOR, APP_REVISION);

	SDL_Color text_color {
		.r = 0,
		.g = 0,
		.b = 0
	};

	SDL_Surface *surf_text;
	SDL_Rect rect_text;

	int text_y = 3;

	// Title
	surf_text = TTF_RenderUTF8_Blended_Wrapped(font_buttons, txt_title, text_color, w);

	rect_text.w = surf_text->w;
	rect_text.h = surf_text->h;
	rect_text.x = (int)(((float)w / 2.0) - ((float)rect_text.w / 2.0));
	rect_text.y = 16;

	SDL_BlitSurface(surf_text, NULL, surface, &rect_text);
	SDL_FreeSurface(surf_text);

	// Info
	surf_text = TTF_RenderUTF8_Blended(font_buttons, "HXEmu is an open-source Epson HX-20 emulator developed by Pontus \"Frigolit\" Rodling and licensed under the MIT license.", text_color);

	rect_text.w = surf_text->w;
	rect_text.h = surf_text->h;
	rect_text.x = 16;
	rect_text.y = (text_y++) * 20;

	SDL_BlitSurface(surf_text, NULL, surface, &rect_text);
	SDL_FreeSurface(surf_text);

	surf_text = TTF_RenderUTF8_Blended(font_buttons, "It's currently under heavy development and many features are broken or not yet implemented.", text_color);

	rect_text.w = surf_text->w;
	rect_text.h = surf_text->h;
	rect_text.x = 16;
	rect_text.y = (text_y++) * 20;

	SDL_BlitSurface(surf_text, NULL, surface, &rect_text);
	SDL_FreeSurface(surf_text);

	// Acknowledgements
	text_y += 2;
	surf_text = TTF_RenderUTF8_Blended(font_buttons, "This software uses Liberation Fonts licensed under SIL Open Font License version 1.1.", text_color);

	rect_text.w = surf_text->w;
	rect_text.h = surf_text->h;
	rect_text.x = 16;
	rect_text.y = (text_y++) * 20;

	SDL_BlitSurface(surf_text, NULL, surface, &rect_text);
	SDL_FreeSurface(surf_text);

	surf_text = TTF_RenderUTF8_Blended(font_buttons, "As well as Fugue Icons by Yusuke Kamiyamane licensed under Creative Commons Attribution 3.0.", text_color);

	rect_text.w = surf_text->w;
	rect_text.h = surf_text->h;
	rect_text.x = 16;
	rect_text.y = (text_y++) * 20;

	SDL_BlitSurface(surf_text, NULL, surface, &rect_text);
	SDL_FreeSurface(surf_text);
}

AboutDialog::~AboutDialog() {
	SDL_FreeSurface(surface);
}

bool AboutDialog::update() {
	return false;
}

void AboutDialog::draw(SDL_Surface *dest) {
	CWidget::draw(dest);
}

void AboutDialog::mousedown(int x, int y) {
}

void AboutDialog::mouseup(int x, int y) {
}

#endif
