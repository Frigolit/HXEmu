#ifdef FRONTEND_SDL2

#ifndef HXEMU_FRONTENDS_SDL2_DIALOGS_MONITOR
#define HXEMU_FRONTENDS_SDL2_DIALOGS_MONITOR

#include <functional>

#include "config_dialog.h"

class MonitorDialog : public ConfigDialog {
	public:
		MonitorDialog(CHX20 *hx20, int x, int y);
		~MonitorDialog();

		virtual bool update();
		virtual void draw(SDL_Surface *dest);

	protected:
		CHX20 *hx20;
};

#endif
#endif
