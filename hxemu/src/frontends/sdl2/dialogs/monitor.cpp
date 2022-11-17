#ifdef FRONTEND_SDL2

#include "../../../globals.h"
#include "../fonts.h"

#include "monitor.h"

MonitorDialog::MonitorDialog(CHX20 *hx20, int x, int y) : ConfigDialog(hx20, x, y) {
	draw_dialog_title("External monitor");

	// Sorry...
	SDL_Surface *surf_text = TTF_RenderUTF8_Blended(font_dialog_text, "Sorry, nothing here yet...", text_color);

	SDL_Rect rect_text {
		.x = 16,
		.y = 64,
		.w = surf_text->w,
		.h = surf_text->h
	};

	SDL_BlitSurface(surf_text, NULL, surface, &rect_text);
	SDL_FreeSurface(surf_text);
}

MonitorDialog::~MonitorDialog() {
	SDL_FreeSurface(surface);
}

bool MonitorDialog::update() {
	return true;
}

void MonitorDialog::draw(SDL_Surface *dest) {
	CWidget::draw(dest);
}

void MonitorDialog::mousedown(int x, int y) {
}

void MonitorDialog::mouseup(int x, int y) {
}


#endif
