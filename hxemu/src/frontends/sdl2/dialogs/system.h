#ifdef FRONTEND_SDL2

#ifndef HXEMU_FRONTENDS_SDL2_DIALOGS_SYSTEM
#define HXEMU_FRONTENDS_SDL2_DIALOGS_SYSTEM

#include <functional>

#include "../../../ui/widget.h"
#include "../../../hx20.h"

class SystemDialog : public CWidget {
	public:
		SystemDialog(CHX20 *hx20, int x, int y);
		~SystemDialog();

		virtual void sdl_keydown(int keysym);
		virtual void sdl_keyup(int keysym);

		virtual bool update();
		virtual void draw(SDL_Surface *dest);

		virtual void mousedown(int x, int y);
		virtual void mouseup(int x, int y);

	protected:
		CHX20 *hx20;
};

#endif
#endif
