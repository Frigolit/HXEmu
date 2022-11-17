#ifdef FRONTEND_SDL2

#ifndef HXEMU_FRONTENDS_SDL2_DIALOGS_EXPERT
#define HXEMU_FRONTENDS_SDL2_DIALOGS_EXPERT

#include <functional>

#include "../widgets/widget.h"
#include "../../../hx20.h"

class ExpertDialog : public CWidget {
	public:
		ExpertDialog(CHX20 *hx20, int x, int y);
		~ExpertDialog();

		virtual bool update();
		virtual void draw(SDL_Surface *dest);

		virtual void mousedown(int x, int y);
		virtual void mouseup(int x, int y);

	protected:
		CHX20 *hx20;
};

#endif
#endif
