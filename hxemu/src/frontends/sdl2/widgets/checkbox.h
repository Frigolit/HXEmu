#ifdef FRONTEND_SDL2

#ifndef HXEMU_FRONTENDS_SDL2_WIDGETS_CHECKBOX_H
#define HXEMU_FRONTENDS_SDL2_WIDGETS_CHECKBOX_H

#include <functional>
#include <string>

#include "widget.h"

class Checkbox : public CWidget {
	public:
		Checkbox(CWidget *p, int x, int y, int w);
		~Checkbox();

		virtual bool update();
		virtual void draw(SDL_Surface *dest);

		virtual CWidget* mousedown(int x, int y);
		virtual CWidget* mouseup(int x, int y);

		virtual void mouseleave();
		virtual void mouseenter();

		void set_checked(bool c);
		void set_caption(std::string c);
		//void set_change_callback(std::function<void(Checkbox*, bool)> callback);

	protected:
		void draw_caption(bool focus);
		void draw_state(bool focus, bool state);

		char *caption;
		bool checked;

		bool is_pressed;
		bool updated;

		std::function<void(Checkbox*, bool)> cb_change;

		SDL_Surface *img_unchecked;
		SDL_Surface *img_checked;
};

#endif
#endif
