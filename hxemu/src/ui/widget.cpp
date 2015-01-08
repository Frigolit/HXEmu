#include "widget.h"

CWidget::CWidget() {
	visible = true;
	enabled = true;

	surface = NULL;
	mtx_surface = SDL_CreateMutex();
}

CWidget::~CWidget() {
	if (surface != NULL) SDL_FreeSurface(surface);
	SDL_DestroyMutex(mtx_surface);
}

void CWidget::draw(SDL_Surface *dest) {
	SDL_Rect src, dst;

	dst.x = x; dst.y = y; dst.w = w; dst.h = h;
	src.x = 0; src.y = 0; src.w = 256; src.h = 128;

	SDL_BlitSurface(surface, NULL, dest, &dst);
}

bool CWidget::update() {
	return true;
}

