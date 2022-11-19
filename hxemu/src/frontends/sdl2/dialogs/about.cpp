#ifdef FRONTEND_SDL2

#include "../../../globals.h"
#include "../../../version.h"
#include "../fonts.h"

#include "about.h"

AboutDialog::AboutDialog(CHX20 *hx20, int x, int y) : ConfigDialog(hx20, x, y) {
	char txt_title[64];
	sprintf(txt_title, "HXEmu %d.%d.%d", APP_MAJOR, APP_MINOR, APP_REVISION);

	draw_dialog_title((const char *)txt_title);

	// Text
	SDL_Surface *surf_text = TTF_RenderUTF8_Blended_Wrapped(
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

	SDL_Rect rect_text {
		.x = 16,
		.y = 64,
		.w = surf_text->w,
		.h = surf_text->h
	};

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

#endif
