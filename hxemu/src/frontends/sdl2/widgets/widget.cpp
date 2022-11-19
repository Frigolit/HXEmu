#ifdef FRONTEND_SDL2

#include "widget.h"

CWidget::CWidget(CWidget *p) {
	visible = true;
	enabled = true;

	parent = p;

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

void CWidget::get_screen_coords(int *_x, int *_y) {
	if (parent == NULL) {
		*_x = x;
		*_y = y;
	}
	else {
		int px, py;
		parent->get_screen_coords(&px, &py);
		*_x = px + x;
		*_y = py + y;
	}
}

#endif
