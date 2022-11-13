#ifdef FRONTEND_SDL2

#include "../../../globals.h"

#include "system.h"

SystemDialog::SystemDialog(CHX20 *hx20, int x, int y) {
	::CWidget();

	this->hx20 = hx20;

	CWidget::x = x;
	CWidget::y = y;
	CWidget::w = 992;
	CWidget::h = 528;

	surface = SDL_CreateRGBSurface(SDL_RLEACCEL, w, h, 32, 0, 0, 0, 0);
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0x3a, 0x7e, 0x45));

	SDL_Rect bg_rect;
	bg_rect.x = 0;
	bg_rect.y = 0;
	bg_rect.w = w;
	bg_rect.h = h;
}

SystemDialog::~SystemDialog() {
	SDL_FreeSurface(surface);
}

bool SystemDialog::update() {
	return true;
}

void SystemDialog::draw(SDL_Surface *dest) {
	//lcd->draw(surface, 288, 28);
	//keyboard->draw(surface);

	CWidget::draw(dest);
}

void SystemDialog::mousedown(int x, int y) {
	/*
	CWidget *w = keyboard;
	if (w->visible && (x >= w->x) && (y >= w->y) && (x < w->x + w->w) && (y < w->y + w->h)) {
		w->mousedown(x - w->x, y - w->y);
		return;
	}
	*/
}

void SystemDialog::mouseup(int x, int y) {
	/*
	CWidget *w = keyboard;
	if (w->visible && (x >= w->x) && (y >= w->y) && (x < w->x + w->w) && (y < w->y + w->h)) {
		w->mouseup(x - w->x, y - w->y);
		return;
	}
	*/
}

void SystemDialog::sdl_keydown(int keysym) {
	//keyboard->sdl_keydown(keysym);
}

void SystemDialog::sdl_keyup(int keysym) {
	//keyboard->sdl_keyup(keysym);
}

#endif
