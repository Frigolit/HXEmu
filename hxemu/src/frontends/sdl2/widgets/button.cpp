#ifdef FRONTEND_SDL2

#include "button.h"
#include <cstring>
#include <vector>

#include "../fonts.h"

CButton::CButton(CWidget *p, std::string c, int x, int y, int w, int h) : CWidget(p) {
	caption = (char *)malloc(sizeof(char) * 256);
	caption[255] = 0;
	strncpy(caption, c.c_str(), 255);

	RgbColor rgb {
		.r = 0xC9,
		.g = 0xC9,
		.b = 0xC9
	};

	init(x, y, w, h, rgb);
}

CButton::CButton(CWidget *p, std::string c, int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) : CWidget(p) {
	caption = (char *)malloc(sizeof(char) * 256);
	caption[255] = 0;
	strncpy(caption, c.c_str(), 255);

	RgbColor rgb {
		.r = r,
		.g = g,
		.b = b
	};

	init(x, y, w, h, rgb);
}

void CButton::init(int x, int y, int w, int h, RgbColor &rgb) {
	cb_click = NULL;
	is_pressed = false;
	has_focus = false;

	CWidget::x = x;
	CWidget::y = y;
	CWidget::w = w;
	CWidget::h = h;

	surface = SDL_CreateRGBSurface(SDL_RLEACCEL, w, h, 32, 0, 0, 0, 0);

	button_color = SDL_MapRGB(surface->format, rgb.r, rgb.g, rgb.b);

	HsvColor hsv;
	rgb_to_hsv(rgb, hsv);

	HsvColor c_lit_hsv = hsv;
	HsvColor c_dim_hsv = hsv;

	c_lit_hsv.v += 0.05;
	c_dim_hsv.v -= 0.05;

	if (c_lit_hsv.v > 1.0) { c_lit_hsv.v = 1.0; }
	if (c_dim_hsv.v < 0.0) { c_dim_hsv.v = 0.0; }

	RgbColor c_lit;
	RgbColor c_dim;
	hsv_to_rgb(c_lit_hsv, c_lit);
	hsv_to_rgb(c_dim_hsv, c_dim);

	button_color_lit = SDL_MapRGB(surface->format, c_lit.r, c_lit.g, c_lit.b);
	button_color_dim = SDL_MapRGB(surface->format, c_dim.r, c_dim.g, c_dim.b);

	// Determine if caption has linebreaks in it
	has_breaks = false;
	for (int i = 0; i < strlen(caption); i++) {
		if (caption[i] == '\n') {
			has_breaks = true;
			break;
		}
	}

	updated = false;
}

CButton::~CButton() {
	SDL_FreeSurface(surface);
}

bool CButton::update() {
	if (updated) {
		return false;
	}

	SDL_Color text_fg;

	text_fg.r = text_fg.g = text_fg.b = 255;
	text_fg.a = 255;

	SDL_Rect r {
		.x = 0,
		.y = 0,
		.w = w,
		.h = h
	};

	SDL_FillRect(surface, &r, is_pressed ? button_color_dim : button_color_lit);

	r.x++;
	r.y++;
	SDL_FillRect(surface, &r, is_pressed ? button_color_lit : button_color_dim);

	r.w -= 2;
	r.h -= 2;
	SDL_FillRect(surface, &r, button_color);

	// Draw label
	if (caption[0] != 0) {
		if (has_breaks) {
			// If the button caption contains linebreaks, TTF_RenderUTF8_Blended_Wrapped will left-align the text instead of centering it
			// as it would if it wrapped due to width or if it didn't wrap at all, so we have to split the caption and render each line.

			char *c = caption;
			int sz = strlen(caption);

			char buf[256];

			std::vector<SDL_Surface*> lines;
			for (int i = 0; i < sz; i++) {
				if (c[i] == '\n') {
					strncpy(buf, c, i);
					buf[i] = 0;

					lines.push_back(TTF_RenderUTF8_Blended_Wrapped(font_buttons, buf, text_fg, w));

					c = c + i + 1;
					sz = strlen(c);
					i = 0;
				}
			}

			strncpy(buf, c, strlen(c));
			buf[strlen(c)] = 0;

			lines.push_back(TTF_RenderUTF8_Blended_Wrapped(font_buttons, buf, text_fg, w));

			int total_height = 0;

			for (int i = 0, j = lines.size(); i < j; i++) {
				SDL_Surface *text = lines.at(i);
				total_height += text->h;
			}

			int text_y = ((float)h / 2.0) - ((float)total_height / 2.0);

			for (int i = 0, j = lines.size(); i < j; i++) {
				SDL_Surface *text = lines.at(i);

				SDL_Rect rect_text;
				rect_text.w = text->w;
				rect_text.h = text->h;
				rect_text.x = (int)(((float)w / 2.0) - ((float)text->w / 2.0)) + (is_pressed ? 1 : 0);
				rect_text.y = text_y + (is_pressed ? 1 : 0);

				text_y += text->h;

				SDL_BlitSurface(text, NULL, surface, &rect_text);

				SDL_FreeSurface(text);
			}
		}
		else {
			// Caption doesn't contain any linebreaks, render normally.

			SDL_Surface *surf_text = TTF_RenderUTF8_Blended_Wrapped(font_buttons, caption, text_fg, w);

			SDL_Rect rect_text;
			rect_text.w = surf_text->w;
			rect_text.h = surf_text->h;
			rect_text.x = (int)(((float)w / 2.0) - ((float)rect_text.w / 2.0)) + (is_pressed ? 1 : 0);
			rect_text.y = (int)(((float)h / 2.0) - ((float)rect_text.h / 2.0)) + (is_pressed ? 1 : 0);

			SDL_BlitSurface(surf_text, NULL, surface, &rect_text);
			SDL_FreeSurface(surf_text);
		}
	}

	updated = true;
	return true;
}

void CButton::draw(SDL_Surface *dest) {
	if (!updated) {
		update();
	}

	CWidget::draw(dest);
}

void CButton::set_click_callback(std::function<void(CButton*, int, int)> callback) {
	cb_click = callback;
}

CWidget* CButton::mousedown(int cx, int cy) {
	has_focus = true;
	mouseenter();

	if (cb_click != NULL) {
		cb_click(this, 0, 0);
	}

	return this;
}

CWidget* CButton::mouseup(int cx, int cy) {
	is_pressed = false;
	has_focus = false;
	updated = false;

	if (cb_click != NULL) {
		cb_click(this, 1, 0);
	}

	return this;
}

void CButton::mouseleave() {
	is_pressed = false;
	updated = false;
}

void CButton::mouseenter() {
	is_pressed = true;
	updated = false;
}

#endif
