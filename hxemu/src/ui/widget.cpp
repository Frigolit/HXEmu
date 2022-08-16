#include "widget.h"

CWidget::CWidget() {
	visible = true;
	enabled = true;

	surface = NULL;
	mtx_surface = SDL_CreateMutex();
}

CWidget::~CWidget() {
	if (surface != NULL) {
		SDL_FreeSurface(surface);
	}

	SDL_DestroyMutex(mtx_surface);
}

void CWidget::draw(SDL_Surface *dest) {
	SDL_Rect dst;

	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;

	SDL_BlitSurface(surface, NULL, dest, &dst);
}

bool CWidget::update() {
	return true;
}

