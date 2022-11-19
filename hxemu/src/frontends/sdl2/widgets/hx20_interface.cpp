#ifdef FRONTEND_SDL2

#include <stdio.h>
#include <SDL2/SDL_image.h>

#include "../../../globals.h"
#include "hx20_interface.h"
#include "keyboard.h"

CHX20InterfaceWidget::CHX20InterfaceWidget(CHX20 *hx20, int x, int y) {
	::CWidget();

	lcd = new Sdl2LcdInterface();

	this->hx20 = hx20;
	hx20->set_lcd_interface(lcd);

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

	char imgpath[512];
	get_data_path(imgpath, "ui/device.png", 512);

	SDL_Surface *bg_surface = IMG_Load(imgpath);
	SDL_BlitScaled(bg_surface, NULL, surface, &bg_rect);
	SDL_FreeSurface(bg_surface);

	keyboard = new CKeyboardWidget(hx20, 32, 230, w - 32, h - 230);
}

CHX20InterfaceWidget::~CHX20InterfaceWidget() {
	SDL_FreeSurface(surface);

	delete(keyboard);
}

bool CHX20InterfaceWidget::update() {
	return true;
}

void CHX20InterfaceWidget::draw(SDL_Surface *dest) {
	lcd->draw(surface, 288, 28);
	keyboard->draw(surface);

	CWidget::draw(dest);
}

CWidget* CHX20InterfaceWidget::mousedown(int x, int y) {
	CWidget *w = keyboard;
	if (w->visible && (x >= w->x) && (y >= w->y) && (x < w->x + w->w) && (y < w->y + w->h)) {
		return w->mousedown(x - w->x, y - w->y);
	}

	return NULL;
}

CWidget* CHX20InterfaceWidget::mouseup(int x, int y) {
	CWidget *w = keyboard;
	if (w->visible && (x >= w->x) && (y >= w->y) && (x < w->x + w->w) && (y < w->y + w->h)) {
		return w->mouseup(x - w->x, y - w->y);
	}

	return NULL;
}

void CHX20InterfaceWidget::sdl_keydown(int keysym) {
	keyboard->sdl_keydown(keysym);
}

void CHX20InterfaceWidget::sdl_keyup(int keysym) {
	keyboard->sdl_keyup(keysym);
}

#endif
