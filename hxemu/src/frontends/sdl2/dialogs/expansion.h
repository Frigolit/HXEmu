#ifdef FRONTEND_SDL2

#ifndef HXEMU_FRONTENDS_SDL2_DIALOGS_EXPANSION
#define HXEMU_FRONTENDS_SDL2_DIALOGS_EXPANSION

#include <functional>

#include "config_dialog.h"

class ExpansionDialog : public ConfigDialog {
	public:
		ExpansionDialog(CHX20 *hx20, int x, int y);
		~ExpansionDialog();

		virtual bool update();
		virtual void draw(SDL_Surface *dest);

	protected:
		CHX20 *hx20;
};

#endif
#endif
