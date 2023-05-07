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
#include <pthread.h>

#ifdef FRONTEND_SDL2
#include "frontends/sdl2.h"
#endif

#ifdef FRONTEND_CLI
#include "frontends/cli.h"
#endif

#include "logging/logger.h"
#include "logging/console_logger.h"
#include "logging/null_logger.h"

#include "globals.h"
#include "hash.h"
#include "version.h"
#include "hx20.h"

Frontend *frontend;

CHX20 *hx20_machine;
pthread_t hx20_thread;

enum Frontends {
	#ifdef FRONTEND_SDL2
	sdl2,
	#endif
	#ifdef FRONTEND_CLI
	cli,
	#endif
};

Frontends default_frontend =
	#ifdef FRONTEND_SDL2
	sdl2
	#elif FRONTEND_CLI
	cli
	#endif
;

void *hx20_run(void*);
void shutdown(int);

Logger *logger;

int main(int argc, char **argv) {
	strcpy(datapath, DATAPATH);

	char rompath[256];
	char optionrompath[256];

	#ifdef __LINUX__
	if (datapath[0] != '/') {
		char exepath[256];
		char tmp[256];

		readlink("/proc/self/exe", exepath, 256);

		if (strlen(exepath) + strlen(tmp) + 1 > 255) {
			std::cerr << "error: data path is too long" << std::endl;
		}

		strcpy(tmp, datapath);
		sprintf(datapath, "%s/%s", dirname(exepath), tmp);
	}
	#endif

	while (datapath[strlen(datapath) - 1] == '/') {
		datapath[strlen(datapath) - 1] = 0;
	}

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

	#if defined(__LINUX__) && defined(FRONTEND_SDL2) && defined(FRONTEND_CLI)
	// Detect if we have a display on Linux if SDL2 support is built-in, otherwise default the frontend to CLI
	char *env_display = getenv("DISPLAY");
	if (env_display != NULL && strlen(env_display) > 0) {
		default_frontend = sdl2;
	}
	else {
		default_frontend = cli;
	}
	#endif

	// TODO: Frontend selection via command-line arguments

	Frontends selected_frontend = default_frontend;

	// Prepare frontend
	switch (selected_frontend) {
		#ifdef FRONTEND_SDL2
		case sdl2:
			logger = new ConsoleLogger();
			frontend = new FrontendSdl2();
			break;
		#endif

		#ifdef FRONTEND_CLI
		case cli:
			logger = new NullLogger();
			frontend = new FrontendCli();
			break;
		#endif
	}

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

	// Start HX-20 processing thread (TODO: Clean this up properly)
	pthread_create(&hx20_thread, NULL, hx20_run, NULL);

	// Run frontend (blocking)
	frontend->run();

	// Shutdown
	shutdown(0);

	// We're done
	return 0;
}

void shutdown(int sig) {
	frontend->stop();
	exit(0);
}

void *hx20_run(void *args) {
	std::chrono::time_point<std::chrono::steady_clock> last_tick = std::chrono::steady_clock::now();

	long freq = 2457600 / 4;	// cpu frequency
	double rate_us = (1 / (double)freq);

	double n = 0.0;

	while (1) {
		std::chrono::time_point<std::chrono::steady_clock> tick = std::chrono::steady_clock::now();
		std::chrono::duration<double> diff = tick - last_tick;
		last_tick = tick;

		n += diff.count();

		while (n >= rate_us) {
			hx20_machine->think();
			n -= rate_us;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}
