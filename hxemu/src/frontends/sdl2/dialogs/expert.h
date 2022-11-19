#ifdef FRONTEND_SDL2

#ifndef HXEMU_FRONTENDS_SDL2_DIALOGS_EXPERT
#define HXEMU_FRONTENDS_SDL2_DIALOGS_EXPERT

#include <functional>

#include "config_dialog.h"

class ExpertDialog : public ConfigDialog {
	public:
		ExpertDialog(CHX20 *hx20, int x, int y);
		~ExpertDialog();

		virtual bool update();
		virtual void draw(SDL_Surface *dest);

	protected:
		CHX20 *hx20;
};

#endif
#endif
