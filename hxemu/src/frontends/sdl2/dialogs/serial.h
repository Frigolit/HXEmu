#ifdef FRONTEND_SDL2

#ifndef HXEMU_FRONTENDS_SDL2_DIALOGS_SERIAL
#define HXEMU_FRONTENDS_SDL2_DIALOGS_SERIAL

#include <functional>

#include "config_dialog.h"

class SerialDialog : public ConfigDialog {
	public:
		SerialDialog(CHX20 *hx20, int x, int y);
		~SerialDialog();

		virtual bool update();
		virtual void draw(SDL_Surface *dest);

	protected:
		CHX20 *hx20;
};

#endif
#endif
