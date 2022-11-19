#ifdef FRONTEND_SDL2

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "checkbox.h"

#include "../fonts.h"

#include "../../../globals.h"

Checkbox::Checkbox(CWidget *p, int x, int y, int w) : CWidget(p) {
	caption = NULL;

	CWidget::x = x;
	CWidget::y = y;
	CWidget::w = w;
	CWidget::h = 32;

	surface = SDL_CreateRGBSurface(SDL_RLEACCEL, w, h, 32, 0, 0, 0, 0);
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xC0, 0xC0, 0xC0));

	char imgpath[512];
	get_data_path(imgpath, "ui/icons/checkbox.png", 512);
	img_unchecked = IMG_Load(imgpath);
	get_data_path(imgpath, "ui/icons/checkbox-checked.png", 512);
	img_checked = IMG_Load(imgpath);

	// Draw unchecked state
	checked = false;
	is_pressed = false;
	draw_state(false, false);
}

Checkbox::~Checkbox() {
	if (caption != NULL) {
		free(caption);
	}

	SDL_FreeSurface(img_unchecked);
	SDL_FreeSurface(img_checked);
}

bool Checkbox::update() {
	return updated;
}

void Checkbox::draw(SDL_Surface *dest) {
	CWidget::draw(dest);
}

void Checkbox::set_caption(std::string c) {
	if (caption != NULL) {
		free(caption);
		caption = NULL;
	}

	int s = c.size();
	caption = (char *)malloc(sizeof(char) * (s + 1));
	caption[s] = 0;
	strncpy(caption, c.c_str(), s);

	draw_caption(false);
	draw_state(false, checked);
}

void Checkbox::draw_caption(bool focus) {
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xC0, 0xC0, 0xC0));

	if (caption != NULL) {
		uint8_t c = focus ? 96 : 32;

		SDL_Color text_color {
			.r = c,
			.g = c,
			.b = c
		};

		SDL_Surface *surf_text = TTF_RenderUTF8_Blended_Wrapped(font_dialog_text, caption, text_color, w);
		SDL_Rect rect_text {
			.x = 34,
			.y = (int)(((float)h / 2.0) - ((float)surf_text->h / 2.0)),
			.w = surf_text->w,
			.h = surf_text->h
		};

		SDL_BlitSurface(surf_text, NULL, surface, &rect_text);
		SDL_FreeSurface(surf_text);
	}
}

CWidget* Checkbox::mousedown(int cx, int cy) {
	mouseenter();
	return this;
}

CWidget* Checkbox::mouseup(int cx, int cy) {
	if (is_pressed) {
		is_pressed = false;
		checked = !checked;
		draw_state(false, checked);
	}
	return this;
}

void Checkbox::mouseleave() {
	is_pressed = false;
	draw_state(true, checked);
}

void Checkbox::mouseenter() {
	is_pressed = true;
	draw_state(true, !checked);
}

void Checkbox::draw_state(bool focus, bool state) {
	draw_caption(focus);

	SDL_Rect dst_rect {
		.x = 0,
		.y = 0,
		.w = 32,
		.h = 32
	};

	SDL_BlitScaled(state ? img_checked : img_unchecked, NULL, surface, &dst_rect);
}

void Checkbox::set_checked(bool c) {
	if (c != checked) {
		checked = c;
		draw_state(false, checked);
	}
}

#endif
