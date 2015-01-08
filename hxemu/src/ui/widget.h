#ifndef __WIDGET_H__
#define __WIDGET_H__

#include <SDL.h>

class CWidget {
	public:
		CWidget();
		~CWidget();

		virtual bool update();
		virtual void draw(SDL_Surface *dest);

		virtual void mousedown(int x, int y) {};
		virtual void mouseup(int x, int y) {};

		// Coordinates
		int x, y, w, h;
		bool visible;
		bool enabled;

	protected:
		SDL_Surface *surface;
		SDL_mutex *mtx_surface;
};

#endif

