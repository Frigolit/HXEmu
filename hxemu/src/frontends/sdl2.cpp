#ifdef FRONTEND_SDL2

#include "sdl2.h"

#include "../fonts.h"
#include "../version.h"

void FrontendSdl2::start(CHX20 *hx20) {
	printf("Initializing SDL2 frontend...\n");

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	// Clean up nicely on exit
	atexit(SDL_Quit);

	// Initialize SDL_ttf
	if (TTF_Init() < 0) {
		fprintf(stderr, "Unable to initialize SDL_ttf\n");
		exit(1);
	}

	// Create window
	char s_wintitle[256];
	sprintf(s_wintitle, "HXEmu %d.%d.%d", APP_MAJOR, APP_MINOR, APP_REVISION);

	int ui_width = 992;
	int ui_height = 528;

	sdl_window = SDL_CreateWindow(s_wintitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ui_width, ui_height, 0);
	sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	screen = SDL_CreateRGBSurface(SDL_RLEACCEL, ui_width, ui_height, 32, 0, 0, 0, 0);

	if (screen == NULL) {
		fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0xAA, 0xAA, 0xAA));

	// Load fonts
	fonts_init();

	// Create HX-20 interface
	hx20_interface = new CHX20InterfaceWidget(hx20, 0, 0);
}

void FrontendSdl2::stop() {
	//fonts_quit();
	//TTF_Quit();
	//SDL_Quit();
}

void FrontendSdl2::run() {
	FrontendSdl2 *frontend = this;

	SDL_Event event;
	while (1) {
		frontend->hx20_interface->draw(frontend->screen);

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) {
						SDL_Quit();
						return;
					}
					else {
						frontend->hx20_interface->sdl_keydown(event.key.keysym.sym);
					}
					break;

				case SDL_KEYUP:
					frontend->hx20_interface->sdl_keyup(event.key.keysym.sym);
					break;

				case SDL_MOUSEBUTTONDOWN:
					frontend->hx20_interface->mousedown(event.button.x, event.button.y);
					break;

				case SDL_MOUSEBUTTONUP:
					frontend->hx20_interface->mouseup(event.button.x, event.button.y);
					break;

				case SDL_QUIT:
					return;

				default:
					break;
			}
		}

		// Render
		SDL_RenderClear(frontend->sdl_renderer);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(frontend->sdl_renderer, frontend->screen);
		SDL_RenderCopy(frontend->sdl_renderer, texture, NULL, NULL);
		SDL_RenderPresent(frontend->sdl_renderer);
		SDL_DestroyTexture(texture);

		// Wait a little bit (TODO: improve me)
		SDL_Delay(10);
	}
}

#endif
