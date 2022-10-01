#ifdef FRONTEND_CLI

#include "cli.h"

#include <thread>
#include <chrono>

void FrontendCli::start(CHX20 *hx20) {
	printf("Initializing CLI frontend...\n");
	lcd = new CliLcdInterface();
	hx20->set_lcd_interface(lcd);
}

void FrontendCli::stop() {
}

void FrontendCli::run() {
	while (1) {
		printf("\e[2J\e[1;1H");
		lcd->redraw();
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
}

#endif
