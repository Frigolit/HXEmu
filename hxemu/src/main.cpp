// =============================================================================
// @title   HXEmu - Epson HX-20 Emulator
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// @version $Id: main.cpp 167 2012-06-05 15:29:03Z frigolit $
// =============================================================================

using namespace std;

#include <stdio.h>
#include <SDL/SDL.h>

#include "hash.h"
#include "config.h"
#include "version.h"
#include "hx20.h"

#define MACHINECOUNT 1

SDL_Surface *screen;
CHX20 *machines[MACHINECOUNT];
SDL_Thread *threads[MACHINECOUNT];

void sdl_init();
int hx20_thread(void *data);
void shutdown();

int main(int argc, char **argv) {
	sdl_init();
	
	for (int i = 0; i < MACHINECOUNT; i++) {
		machines[i] = new CHX20();
		threads[i] = SDL_CreateThread(hx20_thread, machines[i]);
	}
	
	SDL_Event event;
	
	atexit(shutdown);
	
	while (1) {
		for (int i = 0; i < MACHINECOUNT; i++) {
			machines[i]->draw_lcd(screen, 0, i * 128);
		}
		
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) {
						SDL_Quit();
						return 0;
					}
					else if (event.key.keysym.sym == SDLK_RETURN) machines[0]->keyboard_down('\n');
					else if (event.key.keysym.sym == SDLK_1) machines[0]->keyboard_down('1');
					else if (event.key.keysym.sym == SDLK_2) machines[0]->keyboard_down('2');
					else if (event.key.keysym.sym == SDLK_3) machines[0]->keyboard_down('3');
					else if (event.key.keysym.sym == SDLK_a) machines[0]->keyboard_down('a');
					else if (event.key.keysym.sym == SDLK_r) machines[0]->reset();
					break;
					
				case SDL_KEYUP:
					if (event.key.keysym.sym == SDLK_RETURN) machines[0]->keyboard_up('\n');
					else if (event.key.keysym.sym == SDLK_1) machines[0]->keyboard_up('1');
					else if (event.key.keysym.sym == SDLK_2) machines[0]->keyboard_up('2');
					else if (event.key.keysym.sym == SDLK_3) machines[0]->keyboard_up('3');
					else if (event.key.keysym.sym == SDLK_a) machines[0]->keyboard_up('a');
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
	
	screen = SDL_SetVideoMode(480, 128 * MACHINECOUNT, 32, SDL_DOUBLEBUF | SDL_HWACCEL);
	if (!screen) {
		fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
		exit(1);
	}
	
	// Set window title
	SDL_WM_SetCaption("HXEmu 0.1", "");
}

void shutdown() {
	for (int i = 0; i < MACHINECOUNT; i++) {
		SDL_KillThread(threads[i]);
		delete(machines[i]);
	}
}

int hx20_thread(void *data) {
	CHX20 *hx20 = (CHX20*)data;
	
	int i;
	while (1) {
		for (i = 0; i < 500; i++) {
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
			hx20->think();
		}
		
		SDL_Delay(1);
	}
	
	return 0;
}

