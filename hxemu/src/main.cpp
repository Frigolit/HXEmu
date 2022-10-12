// =============================================================================
// @title   HXEmu - Epson HX-20 Emulator
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>

#include <unistd.h>
#include <libgen.h>
#include <signal.h>

#ifdef FRONTEND_SDL2
#include "frontends/sdl2.h"
#endif

#ifdef FRONTEND_CLI
#include "frontends/cli.h"
#endif

#include "logging/logger.h"
#include "logging/console_logger.h"
#include "logging/null_logger.h"

#include "hash.h"
#include "config.h"
#include "version.h"
#include "hx20.h"
#include "fonts.h"

Frontend *frontend;

CHX20 *hx20_machine;
std::thread *hx20_thread;

int hx20_run(void);
void shutdown(int);

Logger *logger;

int main(int argc, char **argv) {
	if (argc >= 1 && strlen(argv[0]) < 256) {
		char basepath[256];
		strncpy(basepath, argv[0], 256);
		chdir(dirname(basepath));
	}

	char rompath[256];
	char optionrompath[256];

	if (argc > 1) {
		if (strlen(argv[1]) > 255) {
			std::cerr << "error: argument 1 (firmware path) is too long (max 255 chars)." << std::endl;
			return 1;
		}

		strcpy(rompath, argv[1]);
	}
	else {
		strcpy(rompath, "firmware/v1.1-swe");
	}

	if (argc > 2) {
		if (strlen(argv[2]) > 255) {
			std::cerr << "error: argument 2 (option rom path) is too long (max 255 chars)." << std::endl;
			return 1;
		}

		strcpy(optionrompath, argv[2]);
	}
	else {
		optionrompath[0] = 0;
	}

	#ifdef FRONTEND_SDL2
	logger = new ConsoleLogger();
	#elif FRONTEND_CLI
	logger = new NullLogger();
	#endif

	#ifdef FRONTEND_SDL2
	frontend = new FrontendSdl2();
	#elif FRONTEND_CLI
	frontend = new FrontendCli();
	#endif

	char logbuf[256];

	sprintf(logbuf, "HXEmu v%d.%d.%d", APP_MAJOR, APP_MINOR, APP_REVISION);
	logger->info(logbuf);
	sprintf(logbuf, "Firmware path: %s", rompath);
	logger->info(logbuf);

	// Set up HX-20 instance
	hx20_machine = new CHX20();
	hx20_machine->load_roms(rompath);

	if (optionrompath[0] != 0) {
		hx20_machine->load_option_rom(optionrompath);
	}

	// Bind process termination handler
	signal(2, shutdown);
	signal(15, shutdown);

	// Start frontend
	frontend->start(hx20_machine);

	// Start HX-20 processing thread
	std::thread hx20_thread(hx20_run);

	// Run frontend (blocking)
	frontend->run();

	// Shutdown
	shutdown(0);

	// We're done
	return 0;
}

void shutdown(int sig) {
	frontend->stop();
	//SDL_KillThread(hx20_thread);
	//delete(hx20_machine);
	exit(0);
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
