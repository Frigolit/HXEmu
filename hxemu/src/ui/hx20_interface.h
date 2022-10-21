#ifdef FRONTEND_SDL2

#ifndef HXEMU_UI_HX20_INTERFACE_H
#define HXEMU_UI_HX20_INTERFACE_H

#include <functional>

#include "widget.h"
#include "keyboard.h"

#include "../hx20.h"
#include "../frontends/sdl2/lcd_interface.h"

class CHX20InterfaceWidget : public CWidget {
	public:
		CHX20InterfaceWidget(CHX20 *hx20, int x, int y);
		~CHX20InterfaceWidget();

		void sdl_keydown(int keysym);
		void sdl_keyup(int keysym);

		virtual bool update();
		virtual void draw(SDL_Surface *dest);

		virtual void mousedown(int x, int y);
		virtual void mouseup(int x, int y);

	protected:
		CHX20 *hx20;
		CKeyboardWidget *keyboard;
		Sdl2LcdInterface *lcd;
};

#endif
#endif
