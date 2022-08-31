// =============================================================================
// @title   HXEmu - Epson HX-20 Emulator
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#include <iostream>
using namespace std;

#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_timer.h>

#include "hash.h"
#include "config.h"
#include "version.h"
#include "hx20.h"
#include "fonts.h"

#include "ui/hx20_interface.h"

CHX20 *hx20_machine;
CHX20InterfaceWidget *hx20_interface;
SDL_Thread *hx20_thread;

SDL_Window *sdl_window;
SDL_Renderer *sdl_renderer;
SDL_Surface *screen;

void sdl_init();
int hx20_run(void *data);
void shutdown();

int main(int argc, char **argv) {
	char rompath[256];
	char optionrompath[256];

	if (argc > 1) {
		if (strlen(argv[1]) > 255) {
			fprintf(stderr, "error: argument 1 (firmware path) is too long (max 255 chars).\n");
			return 1;
		}

		strcpy(rompath, argv[1]);
	}
	else {
		strcpy(rompath, "firmware/v1.1-swe");
	}

	if (argc > 2) {
		if (strlen(argv[2]) > 255) {
			fprintf(stderr, "error: argument 2 (option rom path) is too long (max 255 chars).\n");
			return 1;
		}

		strcpy(optionrompath, argv[2]);
	}
	else {
		optionrompath[0] = 0;
	}

	printf("HXEmu v%d.%d.%d\n", APP_MAJOR, APP_MINOR, APP_REVISION);
	printf("Firmware path: %s\n", rompath);
	putchar('\n');

	// Initialize
	sdl_init();
	fonts_init();

	// Set up HX-20 instance
	hx20_machine = new CHX20();
	hx20_machine->load_roms(rompath);

	if (optionrompath[0] != 0) {
		hx20_machine->load_option_rom(optionrompath);
	}

	hx20_interface = new CHX20InterfaceWidget(hx20_machine, 0, 0);

	hx20_thread = SDL_CreateThread(hx20_run, "", hx20_machine);

	// Bind process termination handler
	atexit(shutdown);

	// Main loop
	SDL_Event event;
	while (1) {
		hx20_interface->draw(screen);

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) {
						SDL_Quit();
						return 0;
					}
					else if (event.key.keysym.sym == SDLK_RETURN) hx20_machine->keyboard_down('\n');
					else if (event.key.keysym.sym == SDLK_0) hx20_machine->keyboard_down('0');
					else if (event.key.keysym.sym == SDLK_1) hx20_machine->keyboard_down('1');
					else if (event.key.keysym.sym == SDLK_2) hx20_machine->keyboard_down('2');
					else if (event.key.keysym.sym == SDLK_3) hx20_machine->keyboard_down('3');
					else if (event.key.keysym.sym == SDLK_4) hx20_machine->keyboard_down('4');
					else if (event.key.keysym.sym == SDLK_5) hx20_machine->keyboard_down('5');
					else if (event.key.keysym.sym == SDLK_6) hx20_machine->keyboard_down('6');
					else if (event.key.keysym.sym == SDLK_7) hx20_machine->keyboard_down('7');
					else if (event.key.keysym.sym == SDLK_8) hx20_machine->keyboard_down('8');
					else if (event.key.keysym.sym == SDLK_9) hx20_machine->keyboard_down('9');
					else if (event.key.keysym.sym == SDLK_a) hx20_machine->keyboard_down('a');
					else if (event.key.keysym.sym == SDLK_b) hx20_machine->keyboard_down('b');
					else if (event.key.keysym.sym == SDLK_c) hx20_machine->keyboard_down('c');
					else if (event.key.keysym.sym == SDLK_d) hx20_machine->keyboard_down('d');
					else if (event.key.keysym.sym == SDLK_e) hx20_machine->keyboard_down('e');
					else if (event.key.keysym.sym == SDLK_f) hx20_machine->keyboard_down('f');
					else if (event.key.keysym.sym == SDLK_g) hx20_machine->keyboard_down('g');
					else if (event.key.keysym.sym == SDLK_h) hx20_machine->keyboard_down('h');
					else if (event.key.keysym.sym == SDLK_i) hx20_machine->keyboard_down('i');
					else if (event.key.keysym.sym == SDLK_j) hx20_machine->keyboard_down('j');
					else if (event.key.keysym.sym == SDLK_k) hx20_machine->keyboard_down('k');
					else if (event.key.keysym.sym == SDLK_l) hx20_machine->keyboard_down('l');
					else if (event.key.keysym.sym == SDLK_m) hx20_machine->keyboard_down('m');
					else if (event.key.keysym.sym == SDLK_n) hx20_machine->keyboard_down('n');
					else if (event.key.keysym.sym == SDLK_o) hx20_machine->keyboard_down('o');
					else if (event.key.keysym.sym == SDLK_p) hx20_machine->keyboard_down('p');
					else if (event.key.keysym.sym == SDLK_q) hx20_machine->keyboard_down('q');
					else if (event.key.keysym.sym == SDLK_r) hx20_machine->keyboard_down('r');
					else if (event.key.keysym.sym == SDLK_s) hx20_machine->keyboard_down('s');
					else if (event.key.keysym.sym == SDLK_t) hx20_machine->keyboard_down('t');
					else if (event.key.keysym.sym == SDLK_u) hx20_machine->keyboard_down('u');
					else if (event.key.keysym.sym == SDLK_v) hx20_machine->keyboard_down('v');
					else if (event.key.keysym.sym == SDLK_w) hx20_machine->keyboard_down('w');
					else if (event.key.keysym.sym == SDLK_x) hx20_machine->keyboard_down('x');
					else if (event.key.keysym.sym == SDLK_y) hx20_machine->keyboard_down('y');
					else if (event.key.keysym.sym == SDLK_z) hx20_machine->keyboard_down('z');
					else if (event.key.keysym.sym == SDLK_SPACE) hx20_machine->keyboard_down(' ');
					else if (event.key.keysym.sym == SDLK_PERIOD) hx20_machine->keyboard_down('.');
					else if (event.key.keysym.sym == SDLK_LSHIFT) hx20_machine->keyboard_down(0x00);
					else if (event.key.keysym.sym == SDLK_RSHIFT) hx20_machine->keyboard_down(0x01);
					else if (event.key.keysym.sym == SDLK_LCTRL) hx20_machine->keyboard_down(0x02);
					else if (event.key.keysym.sym == SDLK_RCTRL) hx20_machine->keyboard_down('[');
					break;

				case SDL_KEYUP:
					if (event.key.keysym.sym == SDLK_RETURN) hx20_machine->keyboard_up('\n');
					else if (event.key.keysym.sym == SDLK_0) hx20_machine->keyboard_up('0');
					else if (event.key.keysym.sym == SDLK_1) hx20_machine->keyboard_up('1');
					else if (event.key.keysym.sym == SDLK_2) hx20_machine->keyboard_up('2');
					else if (event.key.keysym.sym == SDLK_3) hx20_machine->keyboard_up('3');
					else if (event.key.keysym.sym == SDLK_4) hx20_machine->keyboard_up('4');
					else if (event.key.keysym.sym == SDLK_5) hx20_machine->keyboard_up('5');
					else if (event.key.keysym.sym == SDLK_6) hx20_machine->keyboard_up('6');
					else if (event.key.keysym.sym == SDLK_7) hx20_machine->keyboard_up('7');
					else if (event.key.keysym.sym == SDLK_8) hx20_machine->keyboard_up('8');
					else if (event.key.keysym.sym == SDLK_9) hx20_machine->keyboard_up('9');
					else if (event.key.keysym.sym == SDLK_a) hx20_machine->keyboard_up('a');
					else if (event.key.keysym.sym == SDLK_b) hx20_machine->keyboard_up('b');
					else if (event.key.keysym.sym == SDLK_c) hx20_machine->keyboard_up('c');
					else if (event.key.keysym.sym == SDLK_d) hx20_machine->keyboard_up('d');
					else if (event.key.keysym.sym == SDLK_e) hx20_machine->keyboard_up('e');
					else if (event.key.keysym.sym == SDLK_f) hx20_machine->keyboard_up('f');
					else if (event.key.keysym.sym == SDLK_g) hx20_machine->keyboard_up('g');
					else if (event.key.keysym.sym == SDLK_h) hx20_machine->keyboard_up('h');
					else if (event.key.keysym.sym == SDLK_i) hx20_machine->keyboard_up('i');
					else if (event.key.keysym.sym == SDLK_j) hx20_machine->keyboard_up('j');
					else if (event.key.keysym.sym == SDLK_k) hx20_machine->keyboard_up('k');
					else if (event.key.keysym.sym == SDLK_l) hx20_machine->keyboard_up('l');
					else if (event.key.keysym.sym == SDLK_m) hx20_machine->keyboard_up('m');
					else if (event.key.keysym.sym == SDLK_n) hx20_machine->keyboard_up('n');
					else if (event.key.keysym.sym == SDLK_o) hx20_machine->keyboard_up('o');
					else if (event.key.keysym.sym == SDLK_p) hx20_machine->keyboard_up('p');
					else if (event.key.keysym.sym == SDLK_q) hx20_machine->keyboard_up('q');
					else if (event.key.keysym.sym == SDLK_r) hx20_machine->keyboard_up('r');
					else if (event.key.keysym.sym == SDLK_s) hx20_machine->keyboard_up('s');
					else if (event.key.keysym.sym == SDLK_t) hx20_machine->keyboard_up('t');
					else if (event.key.keysym.sym == SDLK_u) hx20_machine->keyboard_up('u');
					else if (event.key.keysym.sym == SDLK_v) hx20_machine->keyboard_up('v');
					else if (event.key.keysym.sym == SDLK_w) hx20_machine->keyboard_up('w');
					else if (event.key.keysym.sym == SDLK_x) hx20_machine->keyboard_up('x');
					else if (event.key.keysym.sym == SDLK_y) hx20_machine->keyboard_up('y');
					else if (event.key.keysym.sym == SDLK_z) hx20_machine->keyboard_up('z');
					else if (event.key.keysym.sym == SDLK_SPACE) hx20_machine->keyboard_up(' ');
					else if (event.key.keysym.sym == SDLK_PERIOD) hx20_machine->keyboard_up('.');
					else if (event.key.keysym.sym == SDLK_LSHIFT) hx20_machine->keyboard_up(0x00);
					else if (event.key.keysym.sym == SDLK_RSHIFT) hx20_machine->keyboard_up(0x01);
					else if (event.key.keysym.sym == SDLK_LCTRL) hx20_machine->keyboard_up(0x02);
					else if (event.key.keysym.sym == SDLK_RCTRL) hx20_machine->keyboard_up('[');
					break;

				case SDL_MOUSEBUTTONDOWN:
					hx20_interface->mousedown(event.button.x, event.button.y);
					break;

				case SDL_MOUSEBUTTONUP:
					hx20_interface->mouseup(event.button.x, event.button.y);
					break;

				case SDL_QUIT:
					SDL_Quit();
					return 0;

				default:
					break;
			}
		}

		// Render
		SDL_RenderClear(sdl_renderer);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(sdl_renderer, screen);
		SDL_RenderCopy(sdl_renderer, texture, NULL, NULL);
		SDL_RenderPresent(sdl_renderer);
		SDL_DestroyTexture(texture);

		// Wait a little bit (TODO: improve me)
		SDL_Delay(10);
	}

	return 0;
}

void sdl_init() {
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
}

void shutdown() {
	//SDL_KillThread(hx20_thread);
	//delete(hx20_machine);

	fonts_quit();
	TTF_Quit();
	SDL_Quit();
}

int hx20_run(void *data) {
	CHX20 *hx20 = (CHX20*)data;

	int i;
	while (1) {
		for (i = 0; i < 3200; i++) {
			hx20->think();
		}

		SDL_Delay(1);
	}

	return 0;
}

