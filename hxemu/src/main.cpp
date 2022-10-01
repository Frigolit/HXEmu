// =============================================================================
// @title   HXEmu - Epson HX-20 Emulator
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>

#include <stdio.h>

#ifdef FRONTEND_SDL2
#include "frontends/sdl2.h"
#endif

#ifdef FRONTEND_CLI
#include "frontends/cli.h"
#endif

#include "hash.h"
#include "config.h"
#include "version.h"
#include "hx20.h"
#include "fonts.h"

Frontend *frontend;

CHX20 *hx20_machine;
std::thread *hx20_thread;

int hx20_run(void);
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

	#ifdef FRONTEND_SDL2
	frontend = new FrontendSdl2();
	#elif FRONTEND_CLI
	frontend = new FrontendCli();
	#endif

	printf("HXEmu v%d.%d.%d\n", APP_MAJOR, APP_MINOR, APP_REVISION);
	printf("Firmware path: %s\n", rompath);
	putchar('\n');

	// Set up HX-20 instance
	hx20_machine = new CHX20();
	hx20_machine->load_roms(rompath);

	if (optionrompath[0] != 0) {
		hx20_machine->load_option_rom(optionrompath);
	}

	// Bind process termination handler
	atexit(shutdown);

	// Start frontend
	frontend->start(hx20_machine);

	// Start HX-20 processing thread
	std::thread hx20_thread(hx20_run);

	// Run frontend (blocking)
	frontend->run();

	// We're done
	return 0;
}

void shutdown() {
	frontend->stop();
	//SDL_KillThread(hx20_thread);
	//delete(hx20_machine);
}

int hx20_run(void) {
	int i;
	while (1) {
		for (i = 0; i < 3200; i++) {
			hx20_machine->think();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	return 0;
}
