#ifdef FRONTEND_SDL2

#ifndef HXEMU_FRONTENDS_SDL2_DIALOGS_EXPANSION
#define HXEMU_FRONTENDS_SDL2_DIALOGS_EXPANSION

#include <functional>

#include "../../../ui/widget.h"
#include "../../../hx20.h"

class ExpansionDialog : public CWidget {
	public:
		ExpansionDialog(CHX20 *hx20, int x, int y);
		~ExpansionDialog();

		virtual bool update();
		virtual void draw(SDL_Surface *dest);

		virtual void mousedown(int x, int y);
		virtual void mouseup(int x, int y);

	protected:
		CHX20 *hx20;
};

#endif
#endif
