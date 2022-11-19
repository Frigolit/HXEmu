#ifdef FRONTEND_SDL2

#ifndef HXEMU_FRONTENDS_SDL2_WIDGETS_LABEL_H
#define HXEMU_FRONTENDS_SDL2_WIDGETS_LABEL_H

#include <string>

#include "widget.h"

class Label : public CWidget {
	public:
		Label(CWidget *p, int x, int y, int w, int h);
		~Label();

		virtual bool update();
		virtual void draw(SDL_Surface *dest);

		void set_caption(std::string c);

	protected:
		void draw_caption();

		char *caption;
		bool is_updated;
};

#endif
#endif
