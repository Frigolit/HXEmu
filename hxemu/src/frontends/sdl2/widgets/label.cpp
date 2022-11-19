#ifdef FRONTEND_SDL2

#include <SDL2/SDL_ttf.h>

#include "label.h"

#include "../fonts.h"

#include "../../../globals.h"

Label::Label(CWidget *p, int x, int y, int w, int h) : CWidget(p) {
	caption = NULL;

	CWidget::x = x;
	CWidget::y = y;
	CWidget::w = w;
	CWidget::h = h;

	surface = SDL_CreateRGBSurface(SDL_RLEACCEL, w, h, 32, 0, 0, 0, 0);
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xC0, 0xC0, 0xC0));
}

Label::~Label() {
	if (caption != NULL) {
		free(caption);
	}
}

bool Label::update() {
	if (is_updated) {
		draw_caption();
		return true;
	}

	return false;
}

void Label::draw(SDL_Surface *dest) {
	CWidget::draw(dest);
}

void Label::set_caption(std::string c) {
	if (caption != NULL) {
		free(caption);
		caption = NULL;
	}

	int s = c.size();
	caption = (char *)malloc(sizeof(char) * (s + 1));
	caption[s] = 0;
	strncpy(caption, c.c_str(), s);

	is_updated = true;
}

void Label::draw_caption() {
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xC0, 0xC0, 0xC0));

	if (caption != NULL) {
		SDL_Color text_color {
			.r = 32,
			.g = 32,
			.b = 32
		};

		SDL_Surface *surf_text = TTF_RenderUTF8_Blended_Wrapped(font_dialog_text, caption, text_color, w);
		SDL_Rect rect_text {
			.x = 0,
			.y = (int)(((float)h / 2.0) - ((float)surf_text->h / 2.0)),
			.w = surf_text->w,
			.h = surf_text->h
		};

		SDL_BlitSurface(surf_text, NULL, surface, &rect_text);
		SDL_FreeSurface(surf_text);
	}
}

#endif
