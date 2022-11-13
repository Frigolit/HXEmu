#ifdef FRONTEND_SDL2

#include "../../../globals.h"
#include "../../../fonts.h"

#include "expert.h"

ExpertDialog::ExpertDialog(CHX20 *hx20, int x, int y) {
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
	SDL_FillRect(surface, &bg_rect, SDL_MapRGB(surface->format, 0x4a, 0x82, 0x52));

	// Draw texts
	char txt_title[64] = "Expert features";

	SDL_Color text_color {
		.r = 0,
		.g = 32,
		.b = 0
	};

	SDL_Color text_color2 {
		.r = 255,
		.g = 255,
		.b = 255
	};

	SDL_Surface *surf_text;
	SDL_Rect rect_text;

	// Title
	surf_text = TTF_RenderUTF8_Blended_Wrapped(font_buttons, txt_title, text_color, w);

	rect_text.w = surf_text->w;
	rect_text.h = surf_text->h;
	rect_text.x = (int)(((float)w / 2.0) - ((float)rect_text.w / 2.0));
	rect_text.y = 15;

	SDL_BlitSurface(surf_text, NULL, surface, &rect_text);
	SDL_FreeSurface(surf_text);

	surf_text = TTF_RenderUTF8_Blended_Wrapped(font_buttons, txt_title, text_color2, w);
	rect_text.y = 16;

	SDL_BlitSurface(surf_text, NULL, surface, &rect_text);
	SDL_FreeSurface(surf_text);

	// Sorry...
	surf_text = TTF_RenderUTF8_Blended(font_dialog_text, "Sorry, nothing here yet...", text_color);

	rect_text.w = surf_text->w;
	rect_text.h = surf_text->h;
	rect_text.x = 16;
	rect_text.y = 64;

	SDL_BlitSurface(surf_text, NULL, surface, &rect_text);
	SDL_FreeSurface(surf_text);
}

ExpertDialog::~ExpertDialog() {
	SDL_FreeSurface(surface);
}

bool ExpertDialog::update() {
	return true;
}

void ExpertDialog::draw(SDL_Surface *dest) {
	CWidget::draw(dest);
}

void ExpertDialog::mousedown(int x, int y) {
}

void ExpertDialog::mouseup(int x, int y) {
}


#endif
