#ifdef FRONTEND_CLI

#include "cli.h"

#include <iostream>
#include <thread>
#include <chrono>

void FrontendCli::start(CHX20 *hx20) {
	printf("Initializing CLI frontend...\n");

	lcd = new CliLcdInterface();

	printf("Configuring LCD interface...\n");
	hx20->set_lcd_interface(lcd);
}

void FrontendCli::stop() {
	std::cout << "\e[?1049l";
	std::cout << "\e[?25h";
}

void FrontendCli::run() {
	std::cout << "\e[?1049h\e[2J\e[1;1H\e[?25l";

	lcd->redraw();

	int redraw_counter = 0;

	while (1) {
		// Render LCD
		if (++redraw_counter == 400) {
			// Fully redraw every 10 sec
			redraw_counter = 0;
			lcd->redraw();
		}
		else {
			lcd->update();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}
}

#endif
