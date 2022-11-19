#ifdef FRONTEND_SDL2

#include "sdl2.h"
#include <SDL2/SDL_image.h>

#include "../globals.h"
#include "../version.h"

#include "sdl2/fonts.h"

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

	int ui_width = 992 + 36 + 2;
	int ui_height = 528;

	sdl_window = SDL_CreateWindow(s_wintitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ui_width, ui_height, 0);
	sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	screen = SDL_CreateRGBSurface(SDL_RLEACCEL, ui_width, ui_height, 32, 0, 0, 0, 0);
	toolbar_surface = SDL_CreateRGBSurface(SDL_RLEACCEL, 36, ui_height, 32, 0, 0, 0, 0);

	if (screen == NULL) {
		fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));

	// Load fonts
	fonts_init();

	// Create HX-20 interface
	hx20_interface = new CHX20InterfaceWidget(hx20, 38, 0);

	// Create configuration dialogs
	dialog_system = new SystemDialog(hx20, 38, 0);
	dialog_option = new OptionDialog(hx20, 38, 0);
	dialog_serial = new SerialDialog(hx20, 38, 0);
	dialog_monitor = new MonitorDialog(hx20, 38, 0);
	dialog_expansion = new ExpansionDialog(hx20, 38, 0);
	dialog_expert = new ExpertDialog(hx20, 38, 0);
	dialog_about = new AboutDialog(hx20, 38, 0);

	// Set active widget to the HX-20 interface
	active_widget = hx20_interface;

	// Initialize toolbar
	for (int i = 0; i < TOOLBAR_BUTTONS; i++) {
		toolbar_buttons[i] = (s_toolbar_button *)malloc(sizeof(struct s_toolbar_button));
		toolbar_buttons[i]->widget = NULL;
	}

	strcpy(toolbar_buttons[0]->icon_path, "ui/icons/toolbar/hx20.png");
	toolbar_buttons[0]->widget = hx20_interface;

	strcpy(toolbar_buttons[1]->icon_path, "ui/icons/toolbar/system.png");
	toolbar_buttons[1]->widget = dialog_system;

	strcpy(toolbar_buttons[2]->icon_path, "ui/icons/toolbar/tape.png");
	toolbar_buttons[2]->widget = dialog_option;

	strcpy(toolbar_buttons[3]->icon_path, "ui/icons/toolbar/serial.png");
	toolbar_buttons[3]->widget = dialog_serial;

	strcpy(toolbar_buttons[4]->icon_path, "ui/icons/toolbar/monitor.png");
	toolbar_buttons[4]->widget = dialog_monitor;

	strcpy(toolbar_buttons[5]->icon_path, "ui/icons/toolbar/expansion.png");
	toolbar_buttons[5]->widget = dialog_expansion;

	strcpy(toolbar_buttons[6]->icon_path, "ui/icons/toolbar/expert.png");
	toolbar_buttons[6]->widget = dialog_expert;

	strcpy(toolbar_buttons[7]->icon_path, "ui/icons/toolbar/about.png");
	toolbar_buttons[7]->widget = dialog_about;

	toolbar_selected_index = 0;
	refresh_toolbar();

	focused_widget = NULL;
	mouse_inside_widget = false;
}

void FrontendSdl2::stop() {
	//fonts_quit();
	TTF_Quit();
	SDL_Quit();
}

void FrontendSdl2::refresh_toolbar() {
	SDL_FillRect(toolbar_surface, NULL, SDL_MapRGB(toolbar_surface->format, 0x7b, 0x88, 0x93));

	SDL_Rect icon_rect;

	for (int i = 0; i < TOOLBAR_BUTTONS; i++) {
		if (toolbar_selected_index == i) {
			icon_rect.x = 0;
			icon_rect.y = i * 36;
			icon_rect.w = 36;
			icon_rect.h = 36;

			SDL_FillRect(toolbar_surface, &icon_rect, SDL_MapRGB(toolbar_surface->format, 0x3a, 0x5f, 0x7e));
		}

		icon_rect.x = 2;
		icon_rect.y = 2 + (i * 36);
		icon_rect.w = 32;
		icon_rect.h = 32;

		char imgpath[512];
		get_data_path(imgpath, toolbar_buttons[i]->icon_path, 512);

		SDL_Surface *icon_surface = IMG_Load(imgpath);
		SDL_BlitScaled(icon_surface, NULL, toolbar_surface, &icon_rect);
		SDL_FreeSurface(icon_surface);
	}

	SDL_BlitSurface(toolbar_surface, NULL, screen, NULL);
}

void FrontendSdl2::run() {
	SDL_Event event;
	while (1) {
		// Event processing
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_KEYDOWN:
					active_widget->sdl_keydown(event.key.keysym.sym);
					break;

				case SDL_KEYUP:
					active_widget->sdl_keyup(event.key.keysym.sym);
					break;

				case SDL_MOUSEBUTTONDOWN:
					{
						if (event.button.x <= 35) {
							int n = event.button.y / 36;
							if (n < TOOLBAR_BUTTONS && toolbar_buttons[n]->widget != NULL) {
								toolbar_selected_index = n;
								active_widget = toolbar_buttons[n]->widget;
								refresh_toolbar();
							}
						}
						else if (event.button.x >= 38) {
							int x = event.button.x - 38;
							int y = event.button.y;

							CWidget *widget = active_widget->mousedown(event.button.x - 38, event.button.y);
							if (widget != NULL) {
								focused_widget = widget;
								mouse_inside_widget = true;
							}
						}
					}
					break;

				case SDL_MOUSEBUTTONUP:
					if (focused_widget != NULL) {
						int x = event.button.x - 38;
						focused_widget->mouseup(x, event.button.y);
						focused_widget = NULL;
						mouse_inside_widget = false;
					}
					break;

				case SDL_MOUSEMOTION:
					if (focused_widget != NULL) {
						int x = event.button.x;
						int y = event.button.y;

						int fx0, fy0, fx1, fy1;
						focused_widget->get_screen_coords(&fx0, &fy0);
						fx1 = fx0 + focused_widget->w - 1;
						fy1 = fy0 + focused_widget->h - 1;

						if (mouse_inside_widget && (x < fx0 || x > fx1 || y < fy0 || y > fy1)) {
							mouse_inside_widget = false;
							focused_widget->mouseleave();
						}
						else if (!mouse_inside_widget && x >= fx0 && x <= fx1 && y >= fy0 && y <= fy1) {
							mouse_inside_widget = true;
							focused_widget->mouseenter();
						}
					}
					break;

				case SDL_QUIT:
					return;

				default:
					break;
			}
		}

		// Render active widget
		active_widget->update();
		active_widget->draw(screen);

		// Render
		SDL_RenderClear(sdl_renderer);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(sdl_renderer, screen);
		SDL_RenderCopy(sdl_renderer, texture, NULL, NULL);
		SDL_RenderPresent(sdl_renderer);
		SDL_DestroyTexture(texture);

		// Wait a little bit (TODO: improve me)
		SDL_Delay(10);
	}
}

#endif
