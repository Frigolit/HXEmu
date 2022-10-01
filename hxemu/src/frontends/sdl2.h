#ifdef FRONTEND_SDL2
#ifndef __FRONTENDS_SDL2__
#define __FRONTENDS_SDL2__

#include "frontend.h"
#include "../ui/hx20_interface.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_timer.h>

class FrontendSdl2 : public Frontend {
	public:
		virtual void start(CHX20 *);
		virtual void stop();
		virtual void run();

		char name[32] = "SDL2";

	protected:
		SDL_Window *sdl_window;
		SDL_Renderer *sdl_renderer;
		SDL_Surface *screen;
		CHX20InterfaceWidget *hx20_interface;

		//static int thread_main(void *data);
};

#endif
#endif
