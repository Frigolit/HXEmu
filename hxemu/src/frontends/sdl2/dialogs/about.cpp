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
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xC0, 0xC0, 0xC0));

	SDL_Rect bg_rect;
	bg_rect.x = 0;
	bg_rect.y = 0;
	bg_rect.w = w;
	bg_rect.h = 48;
	SDL_FillRect(surface, &bg_rect, SDL_MapRGB(surface->format, 0xA0, 0xA0, 0xA0));

	// Draw texts
	char txt_title[64];
	sprintf(txt_title, "HXEmu %d.%d.%d", APP_MAJOR, APP_MINOR, APP_REVISION);

	SDL_Color text_color {
		.r = 32,
		.g = 32,
		.b = 32
	};

	SDL_Color text_color2 {
		.r = 180,
		.g = 180,
		.b = 180
	};

	SDL_Surface *surf_text;
	SDL_Rect rect_text;

	// Title
	surf_text = TTF_RenderUTF8_Blended_Wrapped(font_buttons, txt_title, text_color2, w);

	rect_text.w = surf_text->w;
	rect_text.h = surf_text->h;
	rect_text.x = (int)(((float)w / 2.0) - ((float)rect_text.w / 2.0));
	rect_text.y = 17;

	SDL_BlitSurface(surf_text, NULL, surface, &rect_text);
	SDL_FreeSurface(surf_text);

	surf_text = TTF_RenderUTF8_Blended_Wrapped(font_buttons, txt_title, text_color, w);
	rect_text.y = 16;

	SDL_BlitSurface(surf_text, NULL, surface, &rect_text);
	SDL_FreeSurface(surf_text);

	// Text
	surf_text = TTF_RenderUTF8_Blended_Wrapped(
		font_dialog_text,
		"HXEmu is an open-source Epson HX-20 emulator licensed under the MIT license.\n"
		"It's currently under heavy development and many features are broken or not yet implemented.\n"
		"\n"
		"This software uses Liberation Fonts licensed under SIL Open Font License version 1.1.\n"
		"As well as Fugue Icons by Yusuke Kamiyamane licensed under Creative Commons Attribution 3.0.\n"
		"\n"
		"Thanks to everyone who has contributed to this project:\n"
		"- Pontus Rodling (@frigolit)\n"
		"- Harald Lapp (@aurora)\n"
		"- Dmitrii Eliuseev (@dmitryelj)",
		text_color,
		w - 32
	);

	rect_text.w = surf_text->w;
	rect_text.h = surf_text->h;
	rect_text.x = 16;
	rect_text.y = 64;

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
