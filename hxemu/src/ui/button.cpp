#include "button.h"
#include "../fonts.h"
#include <stdio.h>

CButton::CButton(const char *c, int x, int y, int w, int h) {
	::CWidget();

	cb_click = NULL;

	caption = (char *)malloc(sizeof(char) * 256);
	caption[255] = 0;
	strncpy(caption, c, 255);

	CWidget::x = x;
	CWidget::y = y;
	CWidget::w = w;
	CWidget::h = h;

	surface = SDL_CreateRGBSurface(SDL_RLEACCEL, w, h, 32, 0, 0, 0, 0);

	updated = false;
}

bool CButton::update() {
	if (updated) return false;

	SDL_Color text_fg;
	SDL_Color text_bg;

	text_fg.r = text_fg.g = text_fg.b = 0; text_fg.a = 255;
	text_bg.r = text_bg.g = text_bg.b = 0xC9; text_bg.a = 255;

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
	if (font_buttons) {
        SDL_Rect rect_text;
        SDL_Surface *surf_text = TTF_RenderText_Shaded(font_buttons, caption, text_fg, text_bg);
        rect_text.w = surf_text->w;
        rect_text.h = surf_text->h;
        rect_text.x = (int)(((float)w / 2.0) - ((float)rect_text.w / 2.0));
        rect_text.y = (int)(((float)h / 2.0) - ((float)rect_text.h / 2.0));
        SDL_BlitSurface(surf_text, NULL, surface, &rect_text);
        SDL_FreeSurface(surf_text);
	} else {
	    printf("Warning: the font is not loaded\n");
	}

	updated = true;
	return true;
}

void CButton::draw(SDL_Surface *dest) {
	if (!updated) update();
	CWidget::draw(dest);
}

void CButton::set_click_callback(std::function<void(CWidget*)> callback) {
	cb_click = callback;
}

void CButton::mouseup(int cx, int cy) {
	if (cb_click != NULL) cb_click(this);
}

