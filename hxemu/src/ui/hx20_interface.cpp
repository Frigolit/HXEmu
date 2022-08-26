#include "hx20_interface.h"

#include <stdio.h>
#include <SDL2/SDL_image.h>

CHX20InterfaceWidget::CHX20InterfaceWidget(CHX20 *hx20, int x, int y) {
	::CWidget();

	this->hx20 = hx20;

	CWidget::x = x;
	CWidget::y = y;
	CWidget::w = 992;
	CWidget::h = 528;

	surface = SDL_CreateRGBSurface(SDL_RLEACCEL, w, h, 32, 0, 0, 0, 0);
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0x99, 0x99, 0x99));

	SDL_Rect bg_rect;
	bg_rect.x = 0;
	bg_rect.y = 0;
	bg_rect.w = w;
	bg_rect.h = h;

	SDL_Surface *bg_surface = IMG_Load("data/ui/device.png");
	SDL_BlitScaled(bg_surface, NULL, surface, &bg_rect);
	SDL_FreeSurface(bg_surface);
}

bool CHX20InterfaceWidget::update() {
	return true;
}

void CHX20InterfaceWidget::draw(SDL_Surface *dest) {
	hx20->lcd->draw(surface, 288, 28);

	CWidget::draw(dest);
}

