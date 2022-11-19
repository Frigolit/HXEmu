#ifdef FRONTEND_SDL2

#ifndef HXEMU_FRONTENDS_SDL2_WIDGETS_CHECKBOX_H
#define HXEMU_FRONTENDS_SDL2_WIDGETS_CHECKBOX_H

#include <functional>
#include <string>

#include "widget.h"

class Checkbox : public CWidget {
	public:
		Checkbox(int x, int y, int w);
		~Checkbox();

		virtual bool update();
		virtual void draw(SDL_Surface *dest);

		//virtual CWidget* mousedown(int x, int y);
		//virtual CWidget* mouseup(int x, int y);

		void set_caption(std::string c);
		//void set_change_callback(std::function<void(Checkbox*, bool)> callback);

	protected:
		std::string caption;
		bool checked;

		bool updated;

		std::function<void(Checkbox*, bool)> cb_change;

		SDL_Surface *img_unchecked;
		SDL_Surface *img_checked;
};

#endif
#endif
