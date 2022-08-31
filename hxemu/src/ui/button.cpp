#include "button.h"
#include "../fonts.h"
#include <stdio.h>

CButton::CButton(std::string c, int x, int y, int w, int h) {
	::CWidget();

	caption = (char *)malloc(sizeof(char) * 256);
	caption[255] = 0;
	strncpy(caption, c.c_str(), 255);

	init(x, y, w, h, 0xC9, 0xC9, 0xC9);
}

CButton::CButton(std::string c, int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) {
	::CWidget();

	caption = (char *)malloc(sizeof(char) * 256);
	caption[255] = 0;
	strncpy(caption, c.c_str(), 255);

	init(x, y, w, h, r, g, b);
}

void CButton::init(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) {
	cb_click = NULL;

	CWidget::x = x;
	CWidget::y = y;
	CWidget::w = w;
	CWidget::h = h;

	surface = SDL_CreateRGBSurface(SDL_RLEACCEL, w, h, 32, 0, 0, 0, 0);

	button_color = SDL_MapRGB(surface->format, r, g, b);

	button_color_lit = SDL_MapRGB(surface->format, r + 16, g + 16, b + 16);
	button_color_dim = SDL_MapRGB(surface->format, r - 16, g - 16, b - 16);

	updated = false;
}

CButton::~CButton() {
	SDL_FreeSurface(surface);
}

bool CButton::update() {
	if (updated) return false;

	SDL_Color text_fg;

	text_fg.r = text_fg.g = text_fg.b = 255;
	text_fg.a = 255;

	SDL_Rect r;

	r.x = 0; r.y = 0; r.w = w; r.h = h;
	SDL_FillRect(surface, &r, button_color_lit);

	r.x++;
	r.y++;
	SDL_FillRect(surface, &r, button_color_dim);

	r.w -= 2;
	r.h -= 2;
	SDL_FillRect(surface, &r, button_color);

	// Draw label
	if (caption[0] != 0) {
		SDL_Surface *surf_text = TTF_RenderUTF8_Blended_Wrapped(font_buttons, caption, text_fg, w);

		SDL_Rect rect_text;
		rect_text.w = surf_text->w;
		rect_text.h = surf_text->h;
		rect_text.x = (int)(((float)w / 2.0) - ((float)rect_text.w / 2.0));
		rect_text.y = (int)(((float)h / 2.0) - ((float)rect_text.h / 2.0));

		SDL_BlitSurface(surf_text, NULL, surface, &rect_text);
		SDL_FreeSurface(surf_text);
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
	if (cb_click != NULL) {
		cb_click(this);
	}
}
