// =============================================================================
// @title   HXEmu - Epson HX-20 Emulator
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#include <iostream>
using namespace std;

#include <stdio.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "hash.h"
#include "config.h"
#include "version.h"
#include "hx20.h"
#include "fonts.h"

#include "ui.h"

#define MACHINECOUNT 1

SDL_Surface *screen;
CHX20 *machines[MACHINECOUNT];
SDL_Thread *threads[MACHINECOUNT];

void sdl_init();
int hx20_thread(void *data);
void shutdown();

int main(int argc, char **argv) {
	char rompath[128];
	char optionrompath[128];

	if (argc > 1) {
		strcpy(rompath, argv[1]);
	}
	else {
		strcpy(rompath, "firmware/v1.1-swe");
	}

	if (argc > 2) {
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

	for (int i = 0; i < MACHINECOUNT; i++) {
		machines[i] = new CHX20();
		machines[i]->load_roms(rompath);
		if (optionrompath[0]) machines[i]->load_option_rom(optionrompath);

		threads[i] = SDL_CreateThread(hx20_thread, machines[i]);
	}

	SDL_Event event;

	atexit(shutdown);

	while (1) {
		for (int i = 0; i < MACHINECOUNT; i++) {
			machines[i]->draw(screen, 0, i * 128);
		}

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) {
						SDL_Quit();
						return 0;
					}
					else if (event.key.keysym.sym == SDLK_RETURN) machines[0]->keyboard_down('\n');
					else if (event.key.keysym.sym == SDLK_0) machines[0]->keyboard_down('0');
					else if (event.key.keysym.sym == SDLK_1) machines[0]->keyboard_down('1');
					else if (event.key.keysym.sym == SDLK_2) machines[0]->keyboard_down('2');
					else if (event.key.keysym.sym == SDLK_3) machines[0]->keyboard_down('3');
					else if (event.key.keysym.sym == SDLK_4) machines[0]->keyboard_down('4');
					else if (event.key.keysym.sym == SDLK_5) machines[0]->keyboard_down('5');
					else if (event.key.keysym.sym == SDLK_6) machines[0]->keyboard_down('6');
					else if (event.key.keysym.sym == SDLK_7) machines[0]->keyboard_down('7');
					else if (event.key.keysym.sym == SDLK_8) machines[0]->keyboard_down('8');
					else if (event.key.keysym.sym == SDLK_9) machines[0]->keyboard_down('9');
					else if (event.key.keysym.sym == SDLK_a) machines[0]->keyboard_down('a');
					else if (event.key.keysym.sym == SDLK_b) machines[0]->keyboard_down('b');
					else if (event.key.keysym.sym == SDLK_c) machines[0]->keyboard_down('c');
					else if (event.key.keysym.sym == SDLK_d) machines[0]->keyboard_down('d');
					else if (event.key.keysym.sym == SDLK_e) machines[0]->keyboard_down('e');
					else if (event.key.keysym.sym == SDLK_f) machines[0]->keyboard_down('f');
					else if (event.key.keysym.sym == SDLK_g) machines[0]->keyboard_down('g');
					else if (event.key.keysym.sym == SDLK_h) machines[0]->keyboard_down('h');
					else if (event.key.keysym.sym == SDLK_i) machines[0]->keyboard_down('i');
					else if (event.key.keysym.sym == SDLK_j) machines[0]->keyboard_down('j');
					else if (event.key.keysym.sym == SDLK_k) machines[0]->keyboard_down('k');
					else if (event.key.keysym.sym == SDLK_l) machines[0]->keyboard_down('l');
					else if (event.key.keysym.sym == SDLK_m) machines[0]->keyboard_down('m');
					else if (event.key.keysym.sym == SDLK_n) machines[0]->keyboard_down('n');
					else if (event.key.keysym.sym == SDLK_o) machines[0]->keyboard_down('o');
					else if (event.key.keysym.sym == SDLK_p) machines[0]->keyboard_down('p');
					else if (event.key.keysym.sym == SDLK_q) machines[0]->keyboard_down('q');
					else if (event.key.keysym.sym == SDLK_r) machines[0]->keyboard_down('r');
					else if (event.key.keysym.sym == SDLK_s) machines[0]->keyboard_down('s');
					else if (event.key.keysym.sym == SDLK_t) machines[0]->keyboard_down('t');
					else if (event.key.keysym.sym == SDLK_u) machines[0]->keyboard_down('u');
					else if (event.key.keysym.sym == SDLK_v) machines[0]->keyboard_down('v');
					else if (event.key.keysym.sym == SDLK_w) machines[0]->keyboard_down('w');
					else if (event.key.keysym.sym == SDLK_x) machines[0]->keyboard_down('x');
					else if (event.key.keysym.sym == SDLK_y) machines[0]->keyboard_down('y');
					else if (event.key.keysym.sym == SDLK_z) machines[0]->keyboard_down('z');
					else if (event.key.keysym.sym == SDLK_SPACE) machines[0]->keyboard_down(' ');
					else if (event.key.keysym.sym == SDLK_PERIOD) machines[0]->keyboard_down('.');
					else if (event.key.keysym.sym == SDLK_LSHIFT) machines[0]->keyboard_down(0x00);
					else if (event.key.keysym.sym == SDLK_RSHIFT) machines[0]->keyboard_down(0x01);
					else if (event.key.keysym.sym == SDLK_LCTRL) machines[0]->keyboard_down(0x02);
					else if (event.key.keysym.sym == SDLK_RCTRL) machines[0]->keyboard_down('[');
					break;

				case SDL_KEYUP:
					if (event.key.keysym.sym == SDLK_RETURN) machines[0]->keyboard_up('\n');
					else if (event.key.keysym.sym == SDLK_0) machines[0]->keyboard_up('0');
					else if (event.key.keysym.sym == SDLK_1) machines[0]->keyboard_up('1');
					else if (event.key.keysym.sym == SDLK_2) machines[0]->keyboard_up('2');
					else if (event.key.keysym.sym == SDLK_3) machines[0]->keyboard_up('3');
					else if (event.key.keysym.sym == SDLK_4) machines[0]->keyboard_up('4');
					else if (event.key.keysym.sym == SDLK_5) machines[0]->keyboard_up('5');
					else if (event.key.keysym.sym == SDLK_6) machines[0]->keyboard_up('6');
					else if (event.key.keysym.sym == SDLK_7) machines[0]->keyboard_up('7');
					else if (event.key.keysym.sym == SDLK_8) machines[0]->keyboard_up('8');
					else if (event.key.keysym.sym == SDLK_9) machines[0]->keyboard_up('9');
					else if (event.key.keysym.sym == SDLK_a) machines[0]->keyboard_up('a');
					else if (event.key.keysym.sym == SDLK_b) machines[0]->keyboard_up('b');
					else if (event.key.keysym.sym == SDLK_c) machines[0]->keyboard_up('c');
					else if (event.key.keysym.sym == SDLK_d) machines[0]->keyboard_up('d');
					else if (event.key.keysym.sym == SDLK_e) machines[0]->keyboard_up('e');
					else if (event.key.keysym.sym == SDLK_f) machines[0]->keyboard_up('f');
					else if (event.key.keysym.sym == SDLK_g) machines[0]->keyboard_up('g');
					else if (event.key.keysym.sym == SDLK_h) machines[0]->keyboard_up('h');
					else if (event.key.keysym.sym == SDLK_i) machines[0]->keyboard_up('i');
					else if (event.key.keysym.sym == SDLK_j) machines[0]->keyboard_up('j');
					else if (event.key.keysym.sym == SDLK_k) machines[0]->keyboard_up('k');
					else if (event.key.keysym.sym == SDLK_l) machines[0]->keyboard_up('l');
					else if (event.key.keysym.sym == SDLK_m) machines[0]->keyboard_up('m');
					else if (event.key.keysym.sym == SDLK_n) machines[0]->keyboard_up('n');
					else if (event.key.keysym.sym == SDLK_o) machines[0]->keyboard_up('o');
					else if (event.key.keysym.sym == SDLK_p) machines[0]->keyboard_up('p');
					else if (event.key.keysym.sym == SDLK_q) machines[0]->keyboard_up('q');
					else if (event.key.keysym.sym == SDLK_r) machines[0]->keyboard_up('r');
					else if (event.key.keysym.sym == SDLK_s) machines[0]->keyboard_up('s');
					else if (event.key.keysym.sym == SDLK_t) machines[0]->keyboard_up('t');
					else if (event.key.keysym.sym == SDLK_u) machines[0]->keyboard_up('u');
					else if (event.key.keysym.sym == SDLK_v) machines[0]->keyboard_up('v');
					else if (event.key.keysym.sym == SDLK_w) machines[0]->keyboard_up('w');
					else if (event.key.keysym.sym == SDLK_x) machines[0]->keyboard_up('x');
					else if (event.key.keysym.sym == SDLK_y) machines[0]->keyboard_up('y');
					else if (event.key.keysym.sym == SDLK_z) machines[0]->keyboard_up('z');
					else if (event.key.keysym.sym == SDLK_SPACE) machines[0]->keyboard_up(' ');
					else if (event.key.keysym.sym == SDLK_PERIOD) machines[0]->keyboard_up('.');
					else if (event.key.keysym.sym == SDLK_LSHIFT) machines[0]->keyboard_up(0x00);
					else if (event.key.keysym.sym == SDLK_RSHIFT) machines[0]->keyboard_up(0x01);
					else if (event.key.keysym.sym == SDLK_LCTRL) machines[0]->keyboard_up(0x02);
					else if (event.key.keysym.sym == SDLK_RCTRL) machines[0]->keyboard_up('[');
					break;

				case SDL_MOUSEBUTTONDOWN:
					{
						int mx = event.button.x;
						int my = event.button.y;

						if (mx >= 480) {
							machines[my / 128]->controls->mousedown(mx - 480, my % 128);
						}
					}
					break;

				case SDL_MOUSEBUTTONUP:
					{
						int mx = event.button.x;
						int my = event.button.y;

						if (mx >= 480) {
							machines[my / 128]->controls->mouseup(mx - 480, my % 128);
						}
					}
					break;

				case SDL_QUIT:
					SDL_Quit();
					return 0;

				default:
					break;
			}
		}

		// Flip and wait
		SDL_Flip(screen);
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
	screen = SDL_SetVideoMode(480 + 256, 128 * MACHINECOUNT, 32, SDL_DOUBLEBUF | SDL_HWACCEL);
	if (!screen) {
		fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
		exit(1);
	}

	// Set window title
	char buf[256];
	sprintf(buf, "HXEmu %d.%d.%d", APP_MAJOR, APP_MINOR, APP_REVISION);
	SDL_WM_SetCaption(buf, "");
}

void shutdown() {
	for (int i = 0; i < MACHINECOUNT; i++) {
		SDL_KillThread(threads[i]);
		delete(machines[i]);
	}

	fonts_quit();
	TTF_Quit();
	SDL_Quit();
}

int hx20_thread(void *data) {
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

