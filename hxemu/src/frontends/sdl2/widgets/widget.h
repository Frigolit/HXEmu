#ifdef FRONTEND_SDL2

#ifndef HXEMU_FRONTENDS_SDL2_WIDGETS_WIDGET_H
#define HXEMU_FRONTENDS_SDL2_WIDGETS_WIDGET_H

#include <SDL2/SDL.h>

class CWidget {
	public:
		CWidget(CWidget *parent);
		~CWidget();

		virtual void sdl_keydown(int keysym) { };
		virtual void sdl_keyup(int keysym) { };

		virtual bool update() { return false; }
		virtual void draw(SDL_Surface *dest);

		virtual CWidget* mousedown(int x, int y) { return NULL; };
		virtual CWidget* mouseup(int x, int y) { return NULL; };
		virtual void mouseleave() { };
		virtual void mouseenter() { };

		void get_screen_coords(int *x, int *y);

		// Coordinates
		int x, y, w, h;
		bool visible;
		bool enabled;

		CWidget *parent;

	protected:
		SDL_Surface *surface;
		SDL_mutex *mtx_surface;
};

#endif
#endif
