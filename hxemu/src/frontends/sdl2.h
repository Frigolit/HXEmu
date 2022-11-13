#ifdef FRONTEND_SDL2
#ifndef HXEMU_FRONTENDS_SDL2
#define HXEMU_FRONTENDS_SDL2

#include "frontend.h"
#include "sdl2/dialogs/system.h"
#include "sdl2/dialogs/option.h"
#include "sdl2/dialogs/serial.h"
#include "sdl2/dialogs/monitor.h"
#include "sdl2/dialogs/expansion.h"
#include "sdl2/dialogs/expert.h"
#include "sdl2/dialogs/about.h"

#include "../ui/hx20_interface.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_timer.h>

#define TOOLBAR_BUTTONS 8

class FrontendSdl2 : public Frontend {
	public:
		virtual void start(CHX20 *);
		virtual void stop();
		virtual void run();

	protected:
		void refresh_toolbar();

		SDL_Window *sdl_window;
		SDL_Renderer *sdl_renderer;
		SDL_Surface *screen;

		// Base widgets
		CHX20InterfaceWidget *hx20_interface;
		SystemDialog *dialog_system;
		OptionDialog *dialog_option;
		SerialDialog *dialog_serial;
		MonitorDialog *dialog_monitor;
		ExpansionDialog *dialog_expansion;
		ExpertDialog *dialog_expert;
		AboutDialog *dialog_about;

		// Active widget for rendering and user interaction
		CWidget *active_widget;

		// Toolbar
		SDL_Surface *toolbar_surface;

		struct s_toolbar_button {
			char icon_path[64];
			CWidget *widget;
		};

		s_toolbar_button *toolbar_buttons[TOOLBAR_BUTTONS];

		int toolbar_selected_index;

		//static int thread_main(void *data);
};

#endif
#endif
