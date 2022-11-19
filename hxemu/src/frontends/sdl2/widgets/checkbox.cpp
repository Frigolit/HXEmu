#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "checkbox.h"

#include "../fonts.h"

#include "../../../globals.h"

Checkbox::Checkbox(int x, int y, int w) {
	::CWidget();

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
	SDL_Rect dst_rect {
		.x = 0,
		.y = 0,
		.w = 32,
		.h = 32
	};

	SDL_BlitScaled(img_unchecked, NULL, surface, &dst_rect);
}

Checkbox::~Checkbox() {
	SDL_FreeSurface(img_unchecked);
	SDL_FreeSurface(img_checked);
}

bool Checkbox::update() {
	return true;
}

void Checkbox::draw(SDL_Surface *dest) {
	CWidget::draw(dest);
}

void Checkbox::set_caption(std::string c) {
	caption = c;

	int s = c.size();
	char *txt = (char *)malloc(sizeof(char) * (s + 1));
	txt[s] = 0;
	strncpy(txt, c.c_str(), s);

	SDL_Color text_color {
		.r = 32,
		.g = 32,
		.b = 32
	};

	SDL_Surface *surf_text = TTF_RenderUTF8_Blended_Wrapped(font_dialog_text, txt, text_color, w);
	SDL_Rect rect_text {
		.x = 34,
		.y = (int)(((float)h / 2.0) - ((float)surf_text->h / 2.0)),
		.w = surf_text->w,
		.h = surf_text->h
	};

	SDL_BlitSurface(surf_text, NULL, surface, &rect_text);
	SDL_FreeSurface(surf_text);
}
