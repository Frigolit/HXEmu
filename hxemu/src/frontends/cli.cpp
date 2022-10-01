#ifdef FRONTEND_CLI

#include "cli.h"

#include <thread>
#include <chrono>

void FrontendCli::start(CHX20 *hx20) {
	printf("Initializing CLI frontend...\n");
}

void FrontendCli::stop() {
}

void FrontendCli::run() {
	printf("\e[2J\e[1;1H");

	while (1) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

#endif
