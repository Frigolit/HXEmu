#ifdef FRONTEND_SDL2

#ifndef HXEMU_FRONTENDS_SDL2_DIALOGS_ABOUT
#define HXEMU_FRONTENDS_SDL2_DIALOGS_ABOUT

#include <functional>

#include "../widgets/widget.h"
#include "../../../hx20.h"

class AboutDialog : public CWidget {
	public:
		AboutDialog(CHX20 *hx20, int x, int y);
		~AboutDialog();

		virtual bool update();
		virtual void draw(SDL_Surface *dest);

		virtual void mousedown(int x, int y);
		virtual void mouseup(int x, int y);

	protected:
		CHX20 *hx20;
};

#endif
#endif
