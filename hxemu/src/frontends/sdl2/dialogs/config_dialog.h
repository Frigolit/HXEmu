#ifdef FRONTEND_SDL2

#ifndef HXEMU_FRONTENDS_SDL2_DIALOGS_CONFIG_DIALOG_H
#define HXEMU_FRONTENDS_SDL2_DIALOGS_CONFIG_DIALOG_H

#include <functional>
#include <vector>

#include "../widgets/widget.h"
#include "../fonts.h"
#include "../../../hx20.h"

class ConfigDialog : public CWidget {
	public:
		ConfigDialog(CHX20 *hx20, int x, int y);
		~ConfigDialog();

		virtual bool update();
		virtual void draw(SDL_Surface *dest);

		virtual CWidget* mousedown(int x, int y);
		virtual CWidget* mouseup(int x, int y);

		void draw_dialog_title(const char *title);

	protected:
		CHX20 *hx20;
		std::vector<CWidget *> *widgets;

		SDL_Color text_color {
			.r = 32,
			.g = 32,
			.b = 32
		};
};

#endif
#endif
